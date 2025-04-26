#include <stdio.h>
#include "kfifo.h"

// 示例数据类型
typedef struct
{
    uint8_t id;
    uint16_t value;
} sensor_data_t;

// 示例 1：静态分配 FIFO
void example_static_fifo1(void)
{
    printf("Example 1.1 第一种方式，数据类型：任意基本类型\n");
    // 分步初始化 FIFO
    DECLARE_KFIFO(my_fifo, int, 8);
    INIT_KFIFO(my_fifo);

    // DEFINE_KFIFO(my_fifo, int, 8);

    if (!kfifo_initialized(&my_fifo))
    {
        printf("静态 FIFO 初始化失败.\n");
        return;
    }

    // 打印 FIFO 信息
    printf("FIFO 信息：esize = %d, recsize = %d, size = %d\n", kfifo_esize(&my_fifo), kfifo_recsize(&my_fifo), kfifo_size(&my_fifo));

    int input_data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 测试 in 操作
    unsigned int in_len = kfifo_in(&my_fifo, input_data, 7); // 写入 7 个数据
    printf("静态 FIFO in 操作写入长度：%d\n", in_len);

    // 测试 put 操作
    printf("静态 FIFO 还可以写入的空间：%d\n", kfifo_avail(&my_fifo));
    kfifo_put(&my_fifo, 10); // 写入一个数据

    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));
    if (kfifo_is_full(&my_fifo))
    {
        // 虽然 FIFO 已经满了，但是 peek 操作不会影响 FIFO
        printf("FIFO 已满.\n");
    }

    int output_data[4];

    // 测试 skip 操作
    kfifo_skip_count(&my_fifo, 3); // 跳过 3 个数据
    kfifo_skip(&my_fifo);          // 跳过一个数据
    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));

    // 测试 peek 操作
    unsigned int peek_len = kfifo_out_peek(&my_fifo, output_data, 4); // 读取 (peek) 剩余的 4 个数据
    printf("静态 FIFO peek 操作读取长度：%d\n", peek_len);
    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));
    printf("静态 FIFO peek 操作输出：");
    for (int i = 0; i < 4; i++)
    {
        printf("%d ", output_data[i]);
    }
    printf("\n");
    if (kfifo_is_empty(&my_fifo))
    {
        printf("FIFO 已空.\n");
    }

    // 测试 out 操作
    unsigned int out_len = kfifo_out(&my_fifo, output_data, 4);
    printf("静态 FIFO out 操作读取长度：%d\n", out_len);
    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));
    printf("静态 FIFO out 操作输出：");
    for (int i = 0; i < 4; i++)
    {
        printf("%d ", output_data[i]);
    }
    printf("\n");
    if (kfifo_is_empty(&my_fifo))
    {
        printf("FIFO 已空.\n");
    }
}

void example_static_fifo2(void)
{
    printf("Example 1.2 第二种方式，数据类型：结构体\n");
    // DECLARE_KFIFO(my_fifo, sensor_data_t, 4);
    // INIT_KFIFO(my_fifo);

    // 一步初始化 FIFO
    DEFINE_KFIFO(my_fifo, sensor_data_t, 8);

    if (!kfifo_initialized(&my_fifo))
    {
        printf("静态 FIFO 初始化失败.\n");
        return;
    }

    // 打印 FIFO 信息
    printf("FIFO 信息：esize = %d, recsize = %d, size = %d\n", kfifo_esize(&my_fifo), kfifo_recsize(&my_fifo), kfifo_size(&my_fifo));

    sensor_data_t input_data[] = {{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}, {6, 60}, {7, 70}, {8, 80}, {9, 90}, {10, 100}};

    // 测试 in 操作
    unsigned int in_len = kfifo_in(&my_fifo, input_data, 7); // 写入 7 个数据
    printf("静态 FIFO in 操作写入长度：%d\n", in_len);

    // 测试 put 操作
    printf("静态 FIFO 还可以写入的空间：%d\n", kfifo_avail(&my_fifo));
    kfifo_put(&my_fifo, input_data[9]); // 写入一个数据

    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));
    if (kfifo_is_full(&my_fifo))
    {
        // 虽然 FIFO 已经满了，但是 peek 操作不会影响 FIFO
        printf("FIFO 已满.\n");
    }

    sensor_data_t output_data[4];

    // 测试 skip 操作
    kfifo_skip_count(&my_fifo, 3); // 跳过 3 个数据
    kfifo_skip(&my_fifo);          // 跳过一个数据
    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));

    // 测试 peek 操作
    unsigned int peek_len = kfifo_out_peek(&my_fifo, output_data, 4); // 读取 (peek) 剩余的 4 个数据
    printf("静态 FIFO peek 操作读取长度：%d\n", peek_len);
    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));
    printf("静态 FIFO peek 操作输出：");
    for (int i = 0; i < 4; i++)
    {
        printf("{%d, %d} ", output_data[i].id, output_data[i].value);
    }
    printf("\n");
    if (kfifo_is_empty(&my_fifo))
    {
        printf("FIFO 已空.\n");
    }

    // 测试 out 操作
    unsigned int out_len = kfifo_out(&my_fifo, output_data, 4);
    printf("静态 FIFO out 操作读取长度：%d\n", out_len);
    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));
    printf("静态 FIFO out 操作输出：");
    for (int i = 0; i < 4; i++)
    {
        printf("{%d, %d} ", output_data[i].id, output_data[i].value);
    }
    printf("\n");
    if (kfifo_is_empty(&my_fifo))
    {
        printf("FIFO 已空.\n");
    }
}

// 示例 2：动态分配 FIFO
void example_dynamic_fifo1(void)
{
    printf("Example 2.1 第一种方式，数据类型：任意基本类型\n");
    DECLARE_KFIFO_PTR(my_fifo, int);
    if (kfifo_alloc(&my_fifo, 8)) // 申请 FIFO, 这里的 8 是 FIFO 中 type 的个数，必须是 2 的幂
    {
        printf("动态 FIFO 分配失败.\n");
        return;
    }
    if (!kfifo_initialized(&my_fifo)) // 这里有点多余，只是为了演示
    {
        printf("动态 FIFO 初始化失败.\n");
        return;
    }

    // 打印 FIFO 信息
    printf("FIFO 信息：esize = %d, recsize = %d, size = %d\n", kfifo_esize(&my_fifo), kfifo_recsize(&my_fifo), kfifo_size(&my_fifo));

    int input_data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 测试 in 操作
    unsigned int in_len = kfifo_in(&my_fifo, input_data, 7);
    printf("动态 FIFO in 操作写入长度：%d\n", in_len);

    // 测试 put 操作
    printf("动态 FIFO 还可以写入的空间：%d\n", kfifo_avail(&my_fifo));
    kfifo_put(&my_fifo, 100); // 写入一个数据

    printf("动态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));
    if (kfifo_is_full(&my_fifo))
    {
        // 虽然 FIFO 已经满了，但是 peek 操作不会影响 FIFO
        printf("FIFO 已满.\n");
    }

    int output_data[4];

    // 测试 skip 操作
    kfifo_skip_count(&my_fifo, 3); // 跳过 3 个数据
    kfifo_skip(&my_fifo);          // 跳过一个数据
    printf("动态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));

    // 测试 peek 操作
    unsigned int peek_len = kfifo_out_peek(&my_fifo, output_data, 4); // 读取 (peek) 剩余的 4 个数据
    printf("静态 FIFO peek 操作读取长度：%d\n", peek_len);
    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));
    printf("静态 FIFO peek 操作输出：");
    for (int i = 0; i < 4; i++)
    {
        printf("%d ", output_data[i]);
    }
    printf("\n");
    if (kfifo_is_empty(&my_fifo))
    {
        printf("FIFO 已空.\n");
    }

    // 测试 out 操作
    unsigned int out_len = kfifo_out(&my_fifo, output_data, 4);
    printf("静态 FIFO out 操作读取长度：%d\n", out_len);
    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));
    printf("静态 FIFO out 操作输出：");
    for (int i = 0; i < 4; i++)
    {
        printf("%d ", output_data[i]);
    }
    printf("\n");
    if (kfifo_is_empty(&my_fifo))
    {
        printf("FIFO 已空.\n");
    }

    // 一定记得释放 FIFO
    kfifo_free(&my_fifo);
}
void example_dynamic_fifo2(void)
{
    printf("Example 2.2 第二种方式，数据类型：结构体\n");
    DECLARE_KFIFO_PTR(my_fifo, sensor_data_t);
    unsigned int size = 8;                // FIFO 的大小（必须是 2 的幂）
    size_t esize = sizeof(sensor_data_t); // 每个元素的大小
    void *buffer = malloc(size * esize);  // 分配缓冲区

    if (!buffer)
    {
        printf("buffer 分配失败.\n");
        return;
    }

    // 初始化 FIFO
    if (kfifo_init(&my_fifo, buffer, size * esize) != 0)
    {
        printf("动态 FIFO 初始化失败.\n");
        free(buffer);
        return;
    }
    if (!kfifo_initialized(&my_fifo)) // 这里有点多余，只是为了演示
    {
        printf("动态 FIFO 初始化失败.\n");
        return;
    }
    printf("mask = %u\n", my_fifo.kfifo.mask);

    // 打印 FIFO 信息
    printf("FIFO 信息：esize = %d, recsize = %d, size = %d\n", kfifo_esize(&my_fifo), kfifo_recsize(&my_fifo), kfifo_size(&my_fifo));

    sensor_data_t input_data[] = {{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}, {6, 60}, {7, 70}, {8, 80}, {9, 90}, {10, 100}};

    // 测试 in 操作
    unsigned int in_len = kfifo_in(&my_fifo, input_data, 7); // 写入 7 个数据
    printf("静态 FIFO in 操作写入长度：%d\n", in_len);

    // 测试 put 操作
    printf("静态 FIFO 还可以写入的空间：%d\n", kfifo_avail(&my_fifo));
    kfifo_put(&my_fifo, input_data[9]); // 写入一个数据

    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));
    if (kfifo_is_full(&my_fifo))
    {
        // 虽然 FIFO 已经满了，但是 peek 操作不会影响 FIFO
        printf("FIFO 已满.\n");
    }

    sensor_data_t output_data[4];

    // 测试 skip 操作
    kfifo_skip_count(&my_fifo, 3); // 跳过 3 个数据
    kfifo_skip(&my_fifo);          // 跳过一个数据
    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));

    // 测试 peek 操作
    unsigned int peek_len = kfifo_out_peek(&my_fifo, output_data, 4); // 读取 (peek) 剩余的 4 个数据
    printf("静态 FIFO peek 操作读取长度：%d\n", peek_len);
    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));
    printf("静态 FIFO peek 操作输出：");
    for (int i = 0; i < 4; i++)
    {
        printf("{%d, %d} ", output_data[i].id, output_data[i].value);
    }
    printf("\n");
    if (kfifo_is_empty(&my_fifo))
    {
        printf("FIFO 已空.\n");
    }

    // 测试 out 操作
    unsigned int out_len = kfifo_out(&my_fifo, output_data, 4);
    printf("静态 FIFO out 操作读取长度：%d\n", out_len);
    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));
    printf("静态 FIFO out 操作输出：");
    for (int i = 0; i < 4; i++)
    {
        printf("{%d, %d} ", output_data[i].id, output_data[i].value);
    }
    printf("\n");
    if (kfifo_is_empty(&my_fifo))
    {
        printf("FIFO 已空.\n");
    }

    // 释放缓冲区
    free(buffer);
}

void example_dynamic_fifo3(void)
{
    printf("Example 2.3 第三种方式，数据类型：任意基本类型\n");
    struct kfifo my_fifo;
    // DECLARE_KFIFO_PTR(my_fifo, int);

    if (kfifo_alloc(&my_fifo, 8 * sizeof(int))) // 申请 FIFO, 这里的 8 是 FIFO 中 type 的个数，必须是 2 的幂，用结构体定义的话，type 默认是 unsigned char，这里也就是 8 个字节
    {
        printf("动态 FIFO 分配失败.\n");
        return;
    }
    if (!kfifo_initialized(&my_fifo)) // 这里有点多余，只是为了演示
    {
        printf("动态 FIFO 初始化失败.\n");
        return;
    }

    // 打印 FIFO 信息
    printf("FIFO 信息：esize = %d, recsize = %d, size = %d\n", kfifo_esize(&my_fifo), kfifo_recsize(&my_fifo), kfifo_size(&my_fifo));

    int input_data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 测试 in 操作
    unsigned int in_len = kfifo_in(&my_fifo, input_data, 8 * sizeof(input_data[0]));
    printf("动态 FIFO in 操作写入长度：%d\n", in_len);

    // 测试 put 操作
    printf("动态 FIFO 还可以写入的空间：%d\n", kfifo_avail(&my_fifo));
    // kfifo_put(&my_fifo, 100); // 写入一个数据

    printf("动态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));
    if (kfifo_is_full(&my_fifo))
    {
        // 虽然 FIFO 已经满了，但是 peek 操作不会影响 FIFO
        printf("FIFO 已满.\n");
    }

    int output_data[4];

    // 测试 skip 操作
    kfifo_skip_count(&my_fifo, 4 * sizeof(input_data[0])); // 跳过 4 个数据
    // kfifo_skip(&my_fifo);          // 跳过一个数据
    printf("动态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));

    // 测试 peek 操作
    unsigned int peek_len = kfifo_out_peek(&my_fifo, output_data, 4 * sizeof(input_data[0])); // 读取 (peek) 剩余的 4 个数据
    printf("静态 FIFO peek 操作读取长度：%d\n", peek_len);
    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));
    printf("静态 FIFO peek 操作输出：");
    for (int i = 0; i < 4; i++)
    {
        printf("%d ", output_data[i]);
    }
    printf("\n");
    if (kfifo_is_empty(&my_fifo))
    {
        printf("FIFO 已空.\n");
    }

    // 测试 out 操作
    unsigned int out_len = kfifo_out(&my_fifo, output_data, 4 * sizeof(input_data[0]));
    printf("静态 FIFO out 操作读取长度：%d\n", out_len);
    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));
    printf("静态 FIFO out 操作输出：");
    for (int i = 0; i < 4; i++)
    {
        printf("%d ", output_data[i]);
    }
    printf("\n");
    if (kfifo_is_empty(&my_fifo))
    {
        printf("FIFO 已空.\n");
    }

    // 一定记得释放 FIFO
    kfifo_free(&my_fifo);
    if (my_fifo.kfifo.data == NULL)
    {
        printf("FIFO 释放成功.\n");
    }
}

void example_dynamic_fifo4(void)
{
    printf("Example 2.4 第四种方式，数据类型：unsigned char\n");
    struct kfifo my_fifo;
    // DECLARE_KFIFO_PTR(my_fifo, sensor_data_t);
    unsigned int size = 8 * sizeof(sensor_data_t); // FIFO 的大小（必须是 2 的幂）
    void *buffer = malloc(size);                   // 分配缓冲区

    if (!buffer)
    {
        printf("buffer 分配失败.\n");
        return;
    }

    // 初始化 FIFO
    if (kfifo_init(&my_fifo, buffer, size) != 0)
    {
        printf("动态 FIFO 初始化失败.\n");
        free(buffer);
        return;
    }
    if (!kfifo_initialized(&my_fifo)) // 这里有点多余，只是为了演示
    {
        printf("动态 FIFO 初始化失败.\n");
        return;
    }

    // 打印 FIFO 信息
    printf("FIFO 信息：esize = %d, recsize = %d, size = %d\n", kfifo_esize(&my_fifo), kfifo_recsize(&my_fifo), kfifo_size(&my_fifo));

    sensor_data_t input_data[] = {{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}, {6, 60}, {7, 70}, {8, 80}, {9, 90}, {10, 100}};

    // 测试 in 操作
    unsigned int in_len = kfifo_in(&my_fifo, input_data, 8 * sizeof(input_data[0])); // 写入 8 个数据
    printf("静态 FIFO in 操作写入长度：%d\n", in_len);

    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));
    if (kfifo_is_full(&my_fifo))
    {
        // 虽然 FIFO 已经满了，但是 peek 操作不会影响 FIFO
        printf("FIFO 已满.\n");
    }

    sensor_data_t output_data[4];

    // 测试 skip 操作
    kfifo_skip_count(&my_fifo, 4 * sizeof(input_data[0])); // 跳过 4 个数据
    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));

    // 测试 peek 操作
    unsigned int peek_len = kfifo_out_peek(&my_fifo, output_data, 4 * sizeof(input_data[0])); // 读取 (peek) 剩余的 4 个数据
    printf("静态 FIFO peek 操作读取长度：%d\n", peek_len);
    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));
    printf("静态 FIFO peek 操作输出：");
    for (int i = 0; i < 4; i++)
    {
        printf("{%d, %d} ", output_data[i].id, output_data[i].value);
    }
    printf("\n");
    if (kfifo_is_empty(&my_fifo))
    {
        printf("FIFO 已空.\n");
    }

    // 测试 out 操作
    unsigned int out_len = kfifo_out(&my_fifo, output_data, 4 * sizeof(input_data[0]));
    printf("静态 FIFO out 操作读取长度：%d\n", out_len);
    printf("静态 FIFO 内的数据总长度：%d\n", kfifo_len(&my_fifo));
    printf("静态 FIFO out 操作输出：");
    for (int i = 0; i < 4; i++)
    {
        printf("{%d, %d} ", output_data[i].id, output_data[i].value);
    }
    printf("\n");
    if (kfifo_is_empty(&my_fifo))
    {
        printf("FIFO 已空.\n");
    }

    // 释放缓冲区
    free(buffer);
}

// 示例 3：清空 FIFO
void example_reset_fifo(void)
{
    DECLARE_KFIFO(my_fifo, int, 8);
    INIT_KFIFO(my_fifo);

    int input_data[] = {1, 2, 3, 4};
    kfifo_in(&my_fifo, input_data, 4);

    kfifo_reset(&my_fifo);

    if (kfifo_is_empty(&my_fifo))
    {
        printf("FIFO 已经清空.\n");
    }
}

// 示例 4：record 模式
void example_record_mode_fifo(void)
{
    // 定义一个 record 模式的 FIFO，记录头大小为 1 字节
    STRUCT_KFIFO_REC_1(64)
    my_fifo;
    INIT_KFIFO(my_fifo);

    if (!kfifo_initialized(&my_fifo))
    {
        printf("Failed to initialize FIFO.\n");
        return;
    }

    printf("FIFO info: esize = %d, recsize = %d, size = %d\n",
           kfifo_esize(&my_fifo), kfifo_recsize(&my_fifo), kfifo_size(&my_fifo));

    // 写入变长数据
    sensor_data_t input_data1 = {1, 100};
    sensor_data_t input_data2 = {2, 200};
    sensor_data_t input_data3 = {3, 300};

    kfifo_in(&my_fifo, &input_data1, sizeof(input_data1));
    kfifo_in(&my_fifo, &input_data2, sizeof(input_data2));
    kfifo_in(&my_fifo, &input_data3, sizeof(input_data3));

    printf("Data written to FIFO.\n");

    // 从 FIFO 中读取数据
    sensor_data_t output_data;
    while (!kfifo_is_empty(&my_fifo))
    {
        unsigned int len = kfifo_out(&my_fifo, &output_data, sizeof(output_data));
        if (len > 0)
        {
            printf("Read record: id = %d, value = %d\n", output_data.id, output_data.value);
        }
        else
        {
            printf("Failed to read record.\n");
        }
    }

    printf("FIFO is now empty.\n");
}

// 示例 5：record 模式下的不定长数组写入和读取
void example_record_mode_variable_length(void)
{
    // 定义一个 record 模式的 FIFO，记录头大小为 2 字节
    STRUCT_KFIFO_REC_2(128)
    my_fifo;
    INIT_KFIFO(my_fifo);

    if (!kfifo_initialized(&my_fifo))
    {
        printf("Failed to initialize FIFO.\n");
        return;
    }

    printf("FIFO info: esize = %d, recsize = %d, size = %d\n",
           kfifo_esize(&my_fifo), kfifo_recsize(&my_fifo), kfifo_size(&my_fifo));

    // 写入不定长数组
    int array1[] = {1, 2, 3};
    int array2[] = {10, 20, 30, 40, 50};
    int array3[] = {100, 200};

    kfifo_in(&my_fifo, array1, sizeof(array1));
    kfifo_in(&my_fifo, array2, sizeof(array2));
    kfifo_in(&my_fifo, array3, sizeof(array3));

    printf("Data written to FIFO.\n");

    // 从 FIFO 中读取不定长数组
    char buffer[128]; // 临时缓冲区
    while (!kfifo_is_empty(&my_fifo))
    {
        // 获取下一条记录的长度
        unsigned int record_len = kfifo_peek_len(&my_fifo);
        printf("Next record length: %u bytes\n", record_len);

        // 读取记录
        unsigned int read_len = kfifo_out(&my_fifo, buffer, record_len);
        if (read_len > 0)
        {
            printf("Read record: ");
            for (unsigned int i = 0; i < read_len / sizeof(int); i++)
            {
                printf("%d ", ((int *)buffer)[i]);
            }
            printf("\n");
        }
        else
        {
            printf("Failed to read record.\n");
        }
    }

    printf("FIFO is now empty.\n");
}

// 主函数，调用所有示例
int main()
{
    printf("** Example 1: 静态分配 FIFO ****************\n");
    example_static_fifo1();
    example_static_fifo2();

    printf("\n** Example 2: 动态分配 FIFO ****************\n");
    example_dynamic_fifo1();
    example_dynamic_fifo2();
    example_dynamic_fifo3();
    example_dynamic_fifo4();

    printf("\nExample 3: 重置 FIFO\n");
    example_reset_fifo();

    printf("\nExample 4: Record mode FIFO\n");
    example_record_mode_fifo();

    printf("\nExample 5: Record mode with variable length arrays\n");
    example_record_mode_variable_length();
    
    return 0;
}
