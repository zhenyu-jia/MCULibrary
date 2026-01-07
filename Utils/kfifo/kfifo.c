/**
 * @file kfifo.c
 * @brief 通用环形 FIFO 缓冲区的实现文件
 *
 * 本文件实现了 `kfifo.h` 中定义的环形 FIFO 缓冲区接口，提供了 FIFO 的初始化、数据写入、读取、
 * 跳过、查看等功能。该实现支持固定大小和动态分配的 FIFO 缓冲区，并提供基于记录的 FIFO 操作。
 *
 * 该 FIFO 是在 linux kfifo 的基础删除了一些不用的代码，然后修改以适配单片机。
 *
 * 功能概述：
 * - `__kfifo_alloc`：动态分配 FIFO 缓冲区。
 * - `__kfifo_free`：释放动态分配的 FIFO 缓冲区。
 * - `__kfifo_in` 和 `__kfifo_out`：向 FIFO 写入和读取数据。
 * - `__kfifo_in_r` 和 `__kfifo_out_r`：基于记录的写入和读取操作。
 * - `__kfifo_len_r`：获取记录的长度。
 * - `__kfifo_skip_r`：跳过记录。
 *
 * 设计限制：
 * - FIFO 的大小必须为 2 的幂。
 * - 不适用于内存非常有限的系统，因为动态分配需要额外的内存。
 *
 * 使用建议：
 * - 对于实时性要求较高的场景，建议使用静态分配的 FIFO。
 *
 * @version 2.0.1
 * @date 2025-04-27
 * @author Jia Zhenyu
 */

#include "kfifo.h"

#define min(x, y) ((x) < (y) ? (x) : (y))
#define min3(x, y, z) ({                                                               \
    typeof(x) _min1 = (x);                                                             \
    typeof(y) _min2 = (y);                                                             \
    typeof(z) _min3 = (z);                                                             \
    _min1 < _min2 ? (_min1 < _min3 ? _min1 : _min3) : (_min2 < _min3 ? _min2 : _min3); \
})
#define is_power_of_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))

/* 找到最高位的 1，返回位置（1 开始计数），0 表示没有 1 */
static inline unsigned int fls(unsigned int x)
{
    if (x == 0)
        return 0;

    unsigned int pos = 0;
    while (x != 0)
    {
        pos++;
        x >>= 1;
    }
    return pos;
}

/* 向上取最近的 2 的幂 */
static inline unsigned int roundup_pow_of_two(unsigned int n)
{
    if (n == 0)
        return 1;

    return 1U << fls(n - 1);
}

/* 向下取最近的 2 的幂 */
static inline unsigned int rounddown_pow_of_two(unsigned int n)
{
    if (n == 0)
        return 0;

    return 1U << (fls(n) - 1);
}

/*
 * internal helper to calculate the unused elements in a fifo
 */
static inline unsigned int kfifo_unused(struct __kfifo *fifo)
{
    return (fifo->mask + 1) - (fifo->in - fifo->out);
}

int __kfifo_alloc(struct __kfifo *fifo, unsigned int size,
                  size_t esize)
{
    /*
     * round up to the next power of 2, since our 'let the indices
     * wrap' technique works only in this case.
     */
    size = roundup_pow_of_two(size);

    fifo->in = 0;
    fifo->out = 0;
    fifo->esize = esize;

    if (size < 2)
    {
        fifo->data = NULL;
        fifo->mask = 0;
        return -EINVAL;
    }

    fifo->data = malloc(esize * size);

    if (!fifo->data)
    {
        fifo->mask = 0;
        return -ENOMEM;
    }
    fifo->mask = size - 1;

    return 0;
}

void __kfifo_free(struct __kfifo *fifo)
{
    free(fifo->data);
    fifo->in = 0;
    fifo->out = 0;
    fifo->esize = 0;
    fifo->data = NULL;
    fifo->mask = 0;
}

int __kfifo_init(struct __kfifo *fifo, void *buffer,
                 unsigned int size, size_t esize)
{
    size /= esize;

    if (!is_power_of_2(size))
        size = rounddown_pow_of_two(size);

    fifo->in = 0;
    fifo->out = 0;
    fifo->esize = esize;
    fifo->data = buffer;

    if (size < 2)
    {
        fifo->mask = 0;
        return -EINVAL;
    }
    fifo->mask = size - 1;

    return 0;
}

static void kfifo_copy_in(struct __kfifo *fifo, const void *src,
                          unsigned int len, unsigned int off)
{
    unsigned int size = fifo->mask + 1;
    unsigned int esize = fifo->esize;
    unsigned int l;
    unsigned char *data = (unsigned char *)fifo->data;
    const unsigned char *s = (const unsigned char *)src;

    off &= fifo->mask;
    if (esize != 1)
    {
        off *= esize;
        size *= esize;
        len *= esize;
    }
    l = min(len, size - off);

    // memcpy(fifo->data + off, src, l);
    // memcpy(fifo->data, src + l, len - l);
    memcpy(data + off, s, l);
    memcpy(data, s + l, len - l);
    /*
     * make sure that the data in the fifo is up to date before
     * incrementing the fifo->in index counter
     */
    smp_wmb();
}

unsigned int __kfifo_in(struct __kfifo *fifo,
                        const void *buf, unsigned int len)
{
    unsigned int l;

    l = kfifo_unused(fifo);
    if (len > l)
        len = l;

    kfifo_copy_in(fifo, buf, len, fifo->in);
    fifo->in += len;
    return len;
}

static void kfifo_copy_out(struct __kfifo *fifo, void *dst,
                           unsigned int len, unsigned int off)
{
    unsigned int size = fifo->mask + 1;
    unsigned int esize = fifo->esize;
    unsigned int l;
    unsigned char *data = (unsigned char *)fifo->data;
    unsigned char *d = (unsigned char *)dst;

    off &= fifo->mask;
    if (esize != 1)
    {
        off *= esize;
        size *= esize;
        len *= esize;
    }
    l = min(len, size - off);

    // memcpy(dst, fifo->data + off, l);
    // memcpy(dst + l, fifo->data, len - l);
    memcpy(d, data + off, l);
    memcpy(d + l, data, len - l);
    /*
     * make sure that the data is copied before
     * incrementing the fifo->out index counter
     */
    smp_wmb();
}

unsigned int __kfifo_out_peek(struct __kfifo *fifo,
                              void *buf, unsigned int len)
{
    unsigned int l;

    l = fifo->in - fifo->out;
    if (len > l)
        len = l;

    kfifo_copy_out(fifo, buf, len, fifo->out);
    return len;
}

unsigned int __kfifo_out_linear(struct __kfifo *fifo,
                                unsigned int *tail, unsigned int n)
{
    unsigned int size = fifo->mask + 1;
    unsigned int off = fifo->out & fifo->mask;

    if (tail)
        *tail = off;

    return min3(n, fifo->in - fifo->out, size - off);
}

unsigned int __kfifo_out(struct __kfifo *fifo,
                         void *buf, unsigned int len)
{
    len = __kfifo_out_peek(fifo, buf, len);
    fifo->out += len;
    return len;
}

unsigned int __kfifo_max_r(unsigned int len, size_t recsize)
{
    unsigned int max = (1 << (recsize << 3)) - 1;

    if (len > max)
        return max;
    return len;
}

#define __KFIFO_PEEK(data, out, mask) \
    ((data)[(out) & (mask)])
/*
 * __kfifo_peek_n internal helper function for determinate the length of
 * the next record in the fifo
 */
static unsigned int __kfifo_peek_n(struct __kfifo *fifo, size_t recsize)
{
    unsigned int l;
    unsigned int mask = fifo->mask;
    unsigned char *data = fifo->data;

    l = __KFIFO_PEEK(data, fifo->out, mask);

    if (--recsize)
        l |= __KFIFO_PEEK(data, fifo->out + 1, mask) << 8;

    return l;
}

#define __KFIFO_POKE(data, in, mask, val) \
    (                                     \
        (data)[(in) & (mask)] = (unsigned char)(val))

/*
 * __kfifo_poke_n internal helper function for storing the length of
 * the record into the fifo
 */
static void __kfifo_poke_n(struct __kfifo *fifo, unsigned int n, size_t recsize)
{
    unsigned int mask = fifo->mask;
    unsigned char *data = fifo->data;

    __KFIFO_POKE(data, fifo->in, mask, n);

    if (recsize > 1)
        __KFIFO_POKE(data, fifo->in + 1, mask, n >> 8);
}

unsigned int __kfifo_len_r(struct __kfifo *fifo, size_t recsize)
{
    return __kfifo_peek_n(fifo, recsize);
}

unsigned int __kfifo_in_r(struct __kfifo *fifo, const void *buf,
                          unsigned int len, size_t recsize)
{
    if (len + recsize > kfifo_unused(fifo))
        return 0;

    __kfifo_poke_n(fifo, len, recsize);

    kfifo_copy_in(fifo, buf, len, fifo->in + recsize);
    fifo->in += len + recsize;
    return len;
}

static unsigned int kfifo_out_copy_r(struct __kfifo *fifo,
                                     void *buf, unsigned int len, size_t recsize, unsigned int *n)
{
    *n = __kfifo_peek_n(fifo, recsize);

    if (len > *n)
        len = *n;

    kfifo_copy_out(fifo, buf, len, fifo->out + recsize);
    return len;
}

unsigned int __kfifo_out_peek_r(struct __kfifo *fifo, void *buf,
                                unsigned int len, size_t recsize)
{
    unsigned int n;

    if (fifo->in == fifo->out)
        return 0;

    return kfifo_out_copy_r(fifo, buf, len, recsize, &n);
}

unsigned int __kfifo_out_linear_r(struct __kfifo *fifo,
                                  unsigned int *tail, unsigned int n, size_t recsize)
{
    if (fifo->in == fifo->out)
        return 0;

    if (tail)
        *tail = fifo->out + recsize;

    return min(n, __kfifo_peek_n(fifo, recsize));
}

unsigned int __kfifo_out_r(struct __kfifo *fifo, void *buf,
                           unsigned int len, size_t recsize)
{
    unsigned int n;

    if (fifo->in == fifo->out)
        return 0;

    len = kfifo_out_copy_r(fifo, buf, len, recsize, &n);
    fifo->out += n + recsize;
    return len;
}

void __kfifo_skip_r(struct __kfifo *fifo, size_t recsize)
{
    unsigned int n;

    n = __kfifo_peek_n(fifo, recsize);
    fifo->out += n + recsize;
}
