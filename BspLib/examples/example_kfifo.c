#include <stdio.h>
#include "bsp_kfifo.h"

// 测试例程 1: 分配和释放 KFIFO
void test_kfifo_alloc_free()
{
    // 分配大小为 64 字节的 KFIFO
    KFIFO *fifo = kfifo_alloc(64);
    if (fifo)
    {
        printf("KFIFO 分配成功，大小: %u\n", kfifo_get_size(fifo));
        kfifo_free(fifo);
        printf("KFIFO 释放成功\n");
    }
    else
    {
        printf("KFIFO 分配失败\n");
    }
}

// 测试例程 2: 写入和读取数据
void test_kfifo_put_get()
{
    // 分配大小为 64 字节的 KFIFO
    KFIFO *fifo = kfifo_alloc(64);
    if (fifo)
    {
        uint8_t write_data[] = {1, 2, 3, 4, 5};
        uint8_t read_data[5] = {0};

        // 向 KFIFO 中写入数据
        uint32_t written = kfifo_put(fifo, write_data, sizeof(write_data));
        printf("写入数据长度: %u\n", written);

        // 从 KFIFO 中读取数据
        uint32_t read_len = kfifo_get(fifo, read_data, sizeof(read_data));
        printf("读取数据长度: %u\n", read_len);

        // 打印读取的数据
        printf("读取的数据: ");
        for (uint32_t i = 0; i < read_len; i++)
        {
            printf("%u ", read_data[i]);
        }
        printf("\n");

        kfifo_free(fifo);
    }
    else
    {
        printf("KFIFO 分配失败\n");
    }
}

// 测试例程 3: 数据溢出处理
void test_kfifo_overflow()
{
    // 分配大小为 4 字节的 KFIFO
    KFIFO *fifo = kfifo_alloc(4);
    if (fifo)
    {
        uint8_t write_data[] = {1, 2, 3, 4, 5, 6};
        uint8_t read_data[6] = {0};

        // 向 KFIFO 中写入数据（超过缓冲区大小）
        uint32_t written = kfifo_put(fifo, write_data, sizeof(write_data));
        printf("写入数据长度: %u\n", written);

        // 从 KFIFO 中读取数据
        uint32_t read_len = kfifo_get(fifo, read_data, sizeof(read_data));
        printf("读取数据长度: %u\n", read_len);

        // 打印读取的数据
        printf("读取的数据: ");
        for (uint32_t i = 0; i < read_len; i++)
        {
            printf("%u ", read_data[i]);
        }
        printf("\n");

        kfifo_free(fifo);
    }
    else
    {
        printf("KFIFO 分配失败\n");
    }
}

// 测试例程 4: 多次写入和读取
void test_kfifo_multiple_operations()
{
    // 分配大小为 8 字节的 KFIFO
    KFIFO *fifo = kfifo_alloc(8);
    if (fifo)
    {
        uint8_t write_data1[] = {1, 2, 3, 4};
        uint8_t write_data2[] = {5, 6, 7, 8};
        uint8_t read_data[8] = {0};

        // 向 KFIFO 中写入数据
        kfifo_put(fifo, write_data1, sizeof(write_data1));
        kfifo_put(fifo, write_data2, sizeof(write_data2));

        // 从 KFIFO 中读取数据
        uint32_t read_len = kfifo_get(fifo, read_data, sizeof(read_data));
        printf("读取数据长度: %u\n", read_len);

        // 打印读取的数据
        printf("读取的数据: ");
        for (uint32_t i = 0; i < read_len; i++)
        {
            printf("%u ", read_data[i]);
        }
        printf("\n");

        kfifo_free(fifo);
    }
    else
    {
        printf("KFIFO 分配失败\n");
    }
}

// 测试例程 5: 字符串写入和读取
void test_kfifo_string_operations()
{
    // 分配大小为 20 字节的 KFIFO
    KFIFO *fifo = kfifo_alloc(20);

    char writeStr[100];
    char readStr[100];

    if (fifo)
    {
        printf("KFIFO 分配成功，大小: %u\n", kfifo_get_size(fifo));
        while (1)
        {
            printf("请输入字符串 (输入 'exit' 退出): \n");
            printf("put: ");
            scanf("%s", writeStr);

            if (strcmp(writeStr, "exit") == 0)
            {
                break; // 如果输入 "exit"，则退出循环
            }

            kfifo_put(fifo, (uint8_t *)writeStr, strlen(writeStr) + 1); // 包括 '\0'
            printf("数据长度：%d\t", kfifo_get_data_length(fifo));
            printf("可用空间：%d\n", kfifo_get_available_space(fifo));

            memset(readStr, 0, sizeof(readStr)); // 清空读取缓冲区
            kfifo_get(fifo, (uint8_t *)readStr, kfifo_get_data_length(fifo));
            printf("get: %s\n", readStr);
        }
        kfifo_free(fifo);
        printf("KFIFO 释放成功\n");
    }
    else
    {
        printf("KFIFO 分配失败\n");
    }
}

int main()
{
    // test_kfifo_alloc_free();
    // test_kfifo_put_get();
    // test_kfifo_overflow();
    // test_kfifo_multiple_operations();
    test_kfifo_string_operations();

    return 0;
}
