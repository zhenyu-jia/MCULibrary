/**
 * @file kfifo.c
 * @brief 实现 KFIFO（环形缓冲区）的操作函数。
 *
 * 该文件包含用于操作 KFIFO（环形缓冲区）的函数的实现。KFIFO 是一种高效的环形数据结构，
 * 适用于在数据生产和消费之间进行缓冲处理。文件中实现了缓冲区的分配、初始化、数据写入、
 * 数据读取、释放以及其他辅助操作。
 *
 * 该实现支持单生产者和单消费者的无锁操作，确保线程安全，适用于嵌入式系统、实时数据流
 * 处理等场景。
 *
 * @note
 * - `roundup_pow_of_two`：将给定的数字向上取整到最近的 2 的幂次。
 * - `kfifo_alloc`：分配并初始化一个 KFIFO 结构体。
 * - `kfifo_put`：向 KFIFO 缓冲区中写入数据。
 * - `kfifo_get`：从 KFIFO 缓冲区中读取数据。
 * - `kfifo_free`：释放 KFIFO 缓冲区和结构体。
 * - `kfifo_size`：获取 KFIFO 缓冲区的总大小。
 * - `kfifo_len`：获取 KFIFO 缓冲区中当前存储的数据量。
 * - `kfifo_avail`：获取 KFIFO 缓冲区中的剩余可用空间。
 * - `kfifo_reset`：重置 KFIFO 缓冲区，清空所有数据。
 * - `kfifo_is_empty`：检查 KFIFO 缓冲区是否为空。
 * - `kfifo_is_full`：检查 KFIFO 缓冲区是否已满。
 *
 * @version 1.0.0
 * @date 2024-07-23
 * @author [Jia Zhenyu]
 *
 * @par Example
 * @code
 * KFIFO* my_fifo = kfifo_alloc(128); // 分配一个大小为 128 字节的环形缓冲区
 * uint8_t data[] = {1, 2, 3, 4, 5};  // 要写入的数据
 * kfifo_put(my_fifo, data, sizeof(data)); // 写入数据到缓冲区
 * uint8_t recv_data[5];              // 用于存储读取的数据
 * kfifo_get(my_fifo, recv_data, sizeof(recv_data)); // 从缓冲区读取数据
 * kfifo_free(&my_fifo);              // 释放缓冲区和结构体
 * @endcode
 */

#include "kfifo.h"

#define min(x, y) ((x) < (y) ? (x) : (y))

/**
 * @brief 将给定的数字向上取整到最近的 2 的幂次。
 *
 * 该函数接受一个整数输入，并返回大于或等于该输入的最接近的 2 的幂次。
 * 如果输入已经是 2 的幂次，则返回相同的数字。如果输入为 0，则返回 1。
 *
 * @param num 要向上取整的输入数字。
 * @return 大于或等于输入数字的最近的 2 的幂次。
 *
 * @note 该函数使用位运算来高效地计算下一个 2 的幂次。
 */
static uint32_t roundup_pow_of_two(uint32_t num)
{
    if (num == 0)
    {
        return 1; // 0 的情况下返回 1，因为 2 的 0 次方是 1
    }

    /* 通过连续的位移和或操作，所有低位都被设置为 1，然后通过加 1 得到下一个 2 的幂次 */
    num--;            // 减 1 是为了处理已经是 2 的幂次的情况
    num |= num >> 1;  // 将高位的 1 扩展到低位
    num |= num >> 2;  // 将 2 位的高位 1 扩展到低位
    num |= num >> 4;  // 将 4 位的高位 1 扩展到低位
    num |= num >> 8;  // 将 8 位的高位 1 扩展到低位
    num |= num >> 16; // 将 16 位的高位 1 扩展到低位（适用于 32 位整数）

    return num + 1; // 加 1 得到下一个 2 的幂次
}

/**
 * @brief 分配并初始化一个 KFIFO 结构体。
 *
 * 该函数动态分配一个 KFIFO 结构体，并为其缓冲区分配内存。
 * 缓冲区的大小将被向上取整到最近的 2 的幂次。如果任何内存分配失败，则返回 NULL。
 *
 * @param size 缓冲区的初始大小。
 * @return 指向已分配的 KFIFO 结构体的指针，如果分配失败则返回 NULL。
 */
KFIFO *kfifo_alloc(uint32_t size)
{
    KFIFO *fifo = (KFIFO *)malloc(sizeof(KFIFO));
    if (!fifo)
    {
        return NULL;
    }

    /* 计算 size 的大小，向上扩展成 2 的幂 */
    if (size & (size - 1))
    {
        if (size > 0x80000000)
        {
            free(fifo);
            return NULL;
        }
        size = roundup_pow_of_two(size);
    }

    fifo->buffer = (uint8_t *)malloc(size * sizeof(uint8_t));
    if (!fifo->buffer)
    {
        free(fifo); // 分配失败，释放掉 fifo
        return NULL;
    }

    fifo->size = size;
    fifo->in = 0;
    fifo->out = 0;

    return fifo;
}

/**
 * @brief 向 KFIFO 缓冲区中写入数据。
 *
 * 该函数将指定长度的数据写入 KFIFO 缓冲区。如果缓冲区中的可用空间小于数据长度，
 * 则只写入可用空间的大小。
 *
 * @param fifo 指向 KFIFO 结构体的指针。
 * @param buffer 指向要写入的数据缓冲区。
 * @param len 要写入的数据长度。
 * @return 实际写入缓冲区的数据长度。
 */
uint32_t kfifo_put(KFIFO *fifo, const uint8_t *buffer, uint32_t len)
{
    uint32_t l;

    if (!fifo || !buffer || len == 0)
    {
        return 0; // 检查参数有效性
    }

    len = min(len, fifo->size - (fifo->in - fifo->out));

    /* 第一部分的拷贝:从环形缓冲区写入数据直至缓冲区最后一个地址 */
    l = min(len, fifo->size - (fifo->in & (fifo->size - 1)));
    memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buffer, l);

    /* 如果溢出则在缓冲区头写入剩余的部分
       如果没溢出这句代码相当于无效 */
    memcpy(fifo->buffer, buffer + l, len - l);

    fifo->in += len;

    return len;
}

/**
 * @brief 从 KFIFO 缓冲区中读取数据。
 *
 * 该函数从 KFIFO 缓冲区中读取指定长度的数据。如果缓冲区中的数据量小于
 * 读取长度，则只读取可用的数据量。
 *
 * @param fifo 指向 KFIFO 结构体的指针。
 * @param buffer 指向存储读取数据的缓冲区。
 * @param len 要读取的数据长度。
 * @return 实际读取到缓冲区的数据长度。
 */
uint32_t kfifo_get(KFIFO *fifo, uint8_t *buffer, uint32_t len)
{
    uint32_t l;

    if (!fifo || !buffer || len == 0)
    {
        return 0; // 检查参数有效性
    }

    len = min(len, fifo->in - fifo->out);

    /* 第一部分的拷贝:从环形缓冲区读取数据直至缓冲区最后一个 */
    l = min(len, fifo->size - (fifo->out & (fifo->size - 1)));
    memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);

    /* 如果溢出则在缓冲区头读取剩余的部分
       如果没溢出这句代码相当于无效 */
    memcpy(buffer + l, fifo->buffer, len - l);

    fifo->out += len;

    return len;
}

/**
 * @brief 释放 KFIFO 缓冲区和结构体。
 *
 * 该函数释放 KFIFO 结构体及其内部的缓冲区内存，以避免内存泄漏。
 * 在释放内存后，将指针置为 NULL，以防止悬空指针问题。
 *
 * @param fifo 指向 KFIFO 结构体指针的指针。
 *             如果传入的指针为 NULL 或指向的 KFIFO 结构体为 NULL，则函数不会执行任何操作。
 */
void kfifo_free(KFIFO **fifo)
{
    if (fifo && *fifo)
    {
        if ((*fifo)->buffer)
        {
            free((*fifo)->buffer);
            (*fifo)->buffer = NULL;
        }

        free(*fifo);
        *fifo = NULL;
    }
}

/**
 * @brief 获取 KFIFO 缓冲区的大小。
 *
 * 该函数返回 KFIFO 缓冲区的总大小，以字节为单位。如果传入的指针为 NULL，
 * 则函数返回 0。
 *
 * @param fifo 指向 KFIFO 结构体的指针。
 * @return KFIFO 缓冲区的大小。如果指针为 NULL，则返回 0。
 */
uint32_t kfifo_size(KFIFO *fifo)
{
    if (!fifo)
    {
        return 0; // 如果指针为 NULL，则返回 0
    }

    return fifo->size;
}

/**
 * @brief 获取 KFIFO 缓冲区中的数据量。
 *
 * 该函数返回 KFIFO 缓冲区中当前存储的数据量（字节数）。
 * 如果传入的指针为 NULL，则返回 0。
 *
 * @param fifo 指向 KFIFO 结构体的指针。
 * @return uint32_t 当前缓冲区中的数据量（字节数）。如果指针为 NULL，则返回 0。
 */
uint32_t kfifo_len(KFIFO *fifo)
{
    return fifo ? (fifo->in - fifo->out) : 0;
}

/**
 * @brief 获取 KFIFO 缓冲区中的可用空间。
 *
 * 该函数返回 KFIFO 缓冲区中当前可用的空间量（字节数）。
 * 如果传入的指针为 NULL，则返回 0。
 *
 * @param fifo 指向 KFIFO 结构体的指针。
 * @return uint32_t 当前缓冲区中的可用空间量（字节数）。如果指针为 NULL，则返回 0。
 */
uint32_t kfifo_avail(KFIFO *fifo)
{
    return fifo ? (fifo->size - (fifo->in - fifo->out)) : 0;
}

/**
 * @brief 重置 KFIFO 缓冲区。
 *
 * 该函数将 KFIFO 缓冲区中的读写指针重置为 0，从而清空缓冲区中的所有数据。
 * 如果传入的指针为 NULL，则函数不会执行任何操作。
 *
 * @param fifo 指向 KFIFO 结构体的指针。
 */
void kfifo_reset(KFIFO *fifo)
{
    if (fifo)
    {
        fifo->in = fifo->out = 0;
    }
}

/**
 * @brief 检查 KFIFO 缓冲区是否为空。
 *
 * 该函数检查 KFIFO 缓冲区是否为空。如果缓冲区为空，则返回 1；
 * 否则返回 0。如果传入的指针为 NULL，则返回 0。
 *
 * @param fifo 指向 KFIFO 结构体的指针。
 * @return uint32_t 如果缓冲区为空，则返回 1；否则返回 0。
 */
uint8_t kfifo_is_empty(KFIFO *fifo)
{
    return fifo ? (fifo->in == fifo->out) : 0;
}

/**
 * @brief 检查 KFIFO 缓冲区是否已满。
 *
 * 该函数用于判断 KFIFO 缓冲区是否已满。如果缓冲区中存储的数据量等于缓冲区的总大小，
 * 则返回 1，表示缓冲区已满；否则返回 0。如果传入的指针为 NULL，则返回 0。
 *
 * @param fifo 指向 KFIFO 结构体的指针。
 * @return uint8_t 如果缓冲区已满，则返回 1；否则返回 0。
 */
uint8_t kfifo_is_full(KFIFO *fifo)
{
    return fifo ? (fifo->in - fifo->out == fifo->size) : 0;
}
