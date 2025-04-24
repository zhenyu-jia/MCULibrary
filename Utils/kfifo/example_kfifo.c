#include <stdio.h>
#include "kfifo.h"

// 示例 1: 分配和释放 KFIFO
void example_kfifo_alloc_free()
{
    KFIFO *fifo = kfifo_alloc(128); // 分配 128 字节的 KFIFO
    if (!fifo)
    {
        printf("KFIFO 分配失败\n");
        return;
    }

    printf("KFIFO 分配成功，大小: %u 字节\n", kfifo_size(fifo));
    kfifo_free(&fifo);
    printf("KFIFO 已释放\n");
}

// 示例 2: 写入和读取数据
void example_kfifo_put_get()
{
    KFIFO *fifo = kfifo_alloc(64);
    if (!fifo)
    {
        printf("KFIFO 分配失败\n");
        return;
    }

    uint8_t data_to_write[] = {10, 20, 30, 40, 50};
    uint8_t data_read[5] = {0};

    uint32_t written = kfifo_put(fifo, data_to_write, sizeof(data_to_write));
    if (written == 0)
    {
        printf("写入数据失败\n");
        kfifo_free(&fifo);
        return;
    }
    printf("写入数据长度: %u\n", written);

    uint32_t read = kfifo_get(fifo, data_read, sizeof(data_read));
    if (read == 0)
    {
        printf("读取数据失败\n");
        kfifo_free(&fifo);
        return;
    }
    printf("读取数据长度: %u\n读取的数据: ", read);
    for (uint32_t i = 0; i < read; i++)
    {
        printf("%u ", data_read[i]);
    }
    printf("\n");

    kfifo_free(&fifo);
}

// 示例 3: 缓冲区溢出处理
void example_kfifo_overflow()
{
    KFIFO *fifo = kfifo_alloc(4); // 分配 4 字节的 KFIFO
    if (!fifo)
    {
        printf("KFIFO 分配失败\n");
        return;
    }

    uint8_t data_to_write[] = {1, 2, 3, 4, 5};
    uint32_t written = kfifo_put(fifo, data_to_write, sizeof(data_to_write));
    printf("尝试写入 5 字节，实际写入: %u 字节\n", written);

    uint8_t data_read[4] = {0};
    uint32_t read = kfifo_get(fifo, data_read, sizeof(data_read));
    printf("读取数据长度: %u\n读取的数据: ", read);
    for (uint32_t i = 0; i < read; i++)
    {
        printf("%u ", data_read[i]);
    }
    printf("\n");

    kfifo_free(&fifo);
}

// 示例 4: 多次写入和读取
void example_kfifo_multiple_operations()
{
    KFIFO *fifo = kfifo_alloc(8); // 分配 8 字节的 KFIFO
    if (!fifo)
    {
        printf("KFIFO 分配失败\n");
        return;
    }

    uint8_t data1[] = {1, 2, 3};
    uint8_t data2[] = {4, 5, 6, 7};
    uint8_t data_read[8] = {0};

    if (kfifo_put(fifo, data1, sizeof(data1)) == 0)
    {
        printf("写入数据失败\n");
        kfifo_free(&fifo);
        return;
    }

    if (kfifo_put(fifo, data2, sizeof(data2)) == 0)
    {
        printf("写入数据失败\n");
        kfifo_free(&fifo);
        return;
    }

    uint32_t read = kfifo_get(fifo, data_read, sizeof(data_read));
    if (read == 0)
    {
        printf("读取数据失败\n");
        kfifo_free(&fifo);
        return;
    }
    printf("读取数据长度: %u\n读取的数据: ", read);
    for (uint32_t i = 0; i < read; i++)
    {
        printf("%u ", data_read[i]);
    }
    printf("\n");

    kfifo_free(&fifo);
}

// 示例 5: 检查缓冲区状态
void example_kfifo_status_check()
{
    KFIFO *fifo = kfifo_alloc(16); // 分配 16 字节的 KFIFO
    if (!fifo)
    {
        printf("KFIFO 分配失败\n");
        return;
    }

    uint8_t data[] = {1, 2, 3, 4};
    if (kfifo_put(fifo, data, sizeof(data)) == 0)
    {
        printf("写入数据失败\n");
        kfifo_free(&fifo);
        return;
    }

    printf("缓冲区是否为空: %s\n", kfifo_is_empty(fifo) ? "是" : "否");
    printf("缓冲区是否已满: %s\n", kfifo_is_full(fifo) ? "是" : "否");
    printf("缓冲区已用空间: %u 字节\n", kfifo_len(fifo));
    printf("缓冲区可用空间: %u 字节\n", kfifo_avail(fifo));

    kfifo_reset(fifo);
    printf("缓冲区已重置\n");
    printf("缓冲区是否为空: %s\n", kfifo_is_empty(fifo) ? "是" : "否");

    kfifo_free(&fifo);
}

int main()
{
    example_kfifo_alloc_free();
    example_kfifo_put_get();
    example_kfifo_overflow();
    example_kfifo_multiple_operations();
    example_kfifo_status_check();

    return 0;
}