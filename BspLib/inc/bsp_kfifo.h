/**
 * @file bsp_kfifo.h
 * @brief 环形缓冲区 (KFIFO) 的头文件。
 *
 * 本文件定义了一个环形缓冲区的数据结构和相关操作函数。环形缓冲区是一种用于
 * 高效数据存储和管理的缓冲区，适用于需要循环读写的数据处理场景。
 *
 * 主要功能包括：
 * - 分配和初始化 KFIFO 结构体
 * - 向 KFIFO 缓冲区中写入数据
 * - 从 KFIFO 缓冲区中读取数据
 * - 释放 KFIFO 结构体及其缓冲区
 * - 获取 KFIFO 缓冲区的大小
 *
 * @version 1.0
 * @date 2024-07-23
 * @author [Jia Zhenyu]
 */

#ifndef __BSP_KFIFO_H__
#define __BSP_KFIFO_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#ifdef min
#error "min 宏已经定义，请检查项目中是否有其他定义或包含的库中是否有定义。"
#else
#define min(x, y) ((uint32_t)(x) < (uint32_t)(y) ? (uint32_t)(x) : (uint32_t)(y))
#endif

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
    void kfifo_free(KFIFO *fifo);                                         /* 释放 KFIFO 缓冲区和结构体 */
    uint32_t kfifo_get_size(KFIFO *fifo);                                 /* 获取 KFIFO 缓冲区的大小 */
    uint32_t kfifo_get_data_length(KFIFO *fifo);                          /* 获取 KFIFO 缓冲区中的数据量 */
    uint32_t kfifo_get_available_space(KFIFO *fifo);                      /* 获取 KFIFO 缓冲区中的可用空间 */
    void kfifo_reset(KFIFO *fifo);                                        /* 重置 KFIFO 缓冲区 */

#ifdef __cplusplus
}
#endif

#endif /* __BSP_KFIFO_H__ */
