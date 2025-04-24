# KFIFO 使用说明

## 简介

KFIFO 是一个高效的环形缓冲区实现，适用于需要循环读写的数据处理场景，例如生产者-消费者模型、数据流缓冲等。它支持单生产者和单消费者的无锁操作，确保线程安全，适合嵌入式系统和实时数据流处理。

---

## 功能特性

- **动态内存分配**：支持分配和初始化 KFIFO 结构体。
- **数据操作**：支持向缓冲区写入和读取数据，支持部分写入和读取。
- **状态检查**：支持检查缓冲区是否为空或已满。
- **辅助功能**：支持获取缓冲区大小、已用空间和剩余空间。
- **缓冲区重置**：支持清空缓冲区中的所有数据。
- **内存管理**：支持释放 KFIFO 结构体及其缓冲区，避免内存泄漏。

---

## 文件结构

- **`kfifo.h`**：KFIFO 的头文件，定义了数据结构和操作函数。
- **`kfifo.c`**：KFIFO 的实现文件，包含所有操作函数的实现。
- **`example_kfifo.c`**：示例代码，展示了 KFIFO 的常见使用场景。

---

## API 函数

### 初始化与释放

- `KFIFO *kfifo_alloc(uint32_t size)`  
  分配并初始化一个 KFIFO 结构体，缓冲区大小向上取整到最近的 2 的幂次。

- `void kfifo_free(KFIFO **fifo)`  
  释放 KFIFO 结构体及其缓冲区，避免内存泄漏。

### 数据操作

- `uint32_t kfifo_put(KFIFO *fifo, const uint8_t *buffer, uint32_t len)`  
  向 KFIFO 缓冲区中写入数据，返回实际写入的字节数。

- `uint32_t kfifo_get(KFIFO *fifo, uint8_t *buffer, uint32_t len)`  
  从 KFIFO 缓冲区中读取数据，返回实际读取的字节数。

### 状态检查

- `uint8_t kfifo_is_empty(KFIFO *fifo)`  
  检查缓冲区是否为空。

- `uint8_t kfifo_is_full(KFIFO *fifo)`  
  检查缓冲区是否已满。

### 辅助功能

- `uint32_t kfifo_size(KFIFO *fifo)`  
  获取缓冲区的总大小。

- `uint32_t kfifo_len(KFIFO *fifo)`  
  获取缓冲区中当前存储的数据量。

- `uint32_t kfifo_avail(KFIFO *fifo)`  
  获取缓冲区中的剩余可用空间。

- `void kfifo_reset(KFIFO *fifo)`  
  重置缓冲区，清空所有数据。

---

## 示例代码

以下是一些常见的 KFIFO 使用场景：

### 分配和释放 KFIFO

```c
KFIFO *fifo = kfifo_alloc(128);
if (fifo) {
    printf("KFIFO 分配成功，大小: %u 字节\n", kfifo_size(fifo));
    kfifo_free(&fifo);
}
```

### 写入和读取数据

```c
KFIFO *fifo = kfifo_alloc(64);
if (!fifo) {
    printf("KFIFO 分配失败\n");
    return;
}

uint8_t data_to_write[] = {10, 20, 30, 40, 50};
uint8_t data_read[5] = {0};

uint32_t written = kfifo_put(fifo, data_to_write, sizeof(data_to_write));
if (written == 0) {
    printf("写入数据失败\n");
    kfifo_free(&fifo);
    return;
}
printf("写入数据长度: %u\n", written);

uint32_t read = kfifo_get(fifo, data_read, sizeof(data_read));
if (read == 0) {
    printf("读取数据失败\n");
    kfifo_free(&fifo);
    return;
}
printf("读取数据长度: %u\n读取的数据: ", read);
for (uint32_t i = 0; i < read; i++) {
    printf("%u ", data_read[i]);
}
printf("\n");

kfifo_free(&fifo);
```

### 检查缓冲区状态

```c
KFIFO *fifo = kfifo_alloc(16);
if (!fifo) {
    printf("KFIFO 分配失败\n");
    return;
}

uint8_t data[] = {1, 2, 3, 4};
if (kfifo_put(fifo, data, sizeof(data)) == 0) {
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
```

---

## 编译与运行

使用 GCC 编译并运行示例代码：

1. 打开终端，进入项目目录。
2. 执行以下命令编译示例代码：

   ```bash
   gcc -o example_kfifo example_kfifo.c kfifo.c
   ```

3. 编译完成后运行生成的可执行文件：

   ```bash
   ./example_kfifo
   ```

---

## 注意事项

- KFIFO 的实现假设单生产者和单消费者的使用场景，不适用于多线程并发访问。如果需要在多线程环境中使用，请自行添加线程同步机制。

---

## 更新记录

- **v1.0.0**（2024-07-23）：初始版本发布。

---

## 版本信息

- **作者**：Jia Zhenyu
- **日期**：2024-07-23
- **版本**：1.0.0

---

## 许可证

该 KFIFO 为开源项目，用户可以自由使用、修改和分发。
