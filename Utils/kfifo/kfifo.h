/**
 * @file kfifo.h
 * @brief 环形缓冲区 (KFIFO) 的头文件。
 *
 * 本文件定义了一个环形缓冲区的数据结构和相关操作函数。环形缓冲区是一种高效的
 * 数据存储和管理结构，适用于需要循环读写的数据处理场景，例如生产者-消费者模型、
 * 数据流缓冲等。
 *
 * 主要功能包括：
 * - 分配和初始化 KFIFO 结构体，支持动态内存分配
 * - 向 KFIFO 缓冲区中写入数据，支持部分写入
 * - 从 KFIFO 缓冲区中读取数据，支持部分读取
 * - 释放 KFIFO 结构体及其缓冲区，避免内存泄漏
 * - 获取 KFIFO 缓冲区的总大小、已用空间和剩余空间
 * - 检查 KFIFO 缓冲区是否为空或已满
 * - 重置缓冲区，清空所有数据
 *
 * 该实现支持单生产者和单消费者的无锁操作，确保线程安全。
 *
 * @version 1.0.0
 * @date 2024-07-23
 * @author [Jia Zhenyu]
 */

#ifndef __KFIFO_H__
#define __KFIFO_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

    /**
     * @brief KFIFO 结构体定义。
     *
     * 该结构体定义了一个环形缓冲区（KFIFO），用于存储和管理数据。
     */
    typedef struct __KFIFO
    {
        uint8_t *buffer; /* 保存数据的缓冲区 */
        uint32_t size;   /* 已分配缓冲区的大小 */
        uint32_t in;     /* 添加数据的位置 (in % size) */
        uint32_t out;    /* 读取数据的位置 (out % size) */
    } KFIFO;

    KFIFO *kfifo_alloc(uint32_t size);                                    /* 分配并初始化一个 KFIFO 结构体 */
    uint32_t kfifo_put(KFIFO *fifo, const uint8_t *buffer, uint32_t len); /* 向 KFIFO 缓冲区中写入数据 */
    uint32_t kfifo_get(KFIFO *fifo, uint8_t *buffer, uint32_t len);       /* 从 KFIFO 缓冲区中读取数据 */
    void kfifo_free(KFIFO **fifo);                                        /* 释放 KFIFO 缓冲区和结构体 */
    uint32_t kfifo_size(KFIFO *fifo);                                     /* 获取 KFIFO 缓冲区的大小 */
    uint32_t kfifo_len(KFIFO *fifo);                                      /* 获取 KFIFO 缓冲区中的数据量 */
    uint32_t kfifo_avail(KFIFO *fifo);                                    /* 获取 KFIFO 缓冲区中的可用空间 */
    void kfifo_reset(KFIFO *fifo);                                        /* 重置 KFIFO 缓冲区 */
    uint8_t kfifo_is_empty(KFIFO *fifo);                                  /* 检查 KFIFO 缓冲区是否为空 */
    uint8_t kfifo_is_full(KFIFO *fifo);                                   /* 检查 KFIFO 缓冲区是否已满 */

#ifdef __cplusplus
}
#endif

#endif /* __KFIFO_H__ */
