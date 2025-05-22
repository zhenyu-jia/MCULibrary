#include "w9825g6kh.h"

W9825G6KH hw9825g6kh;

// 测试 W9825G6KH
#define BLOCK_SIZE_BYTES 1024 // 分块大小 1KB

int test_8bit_rw()
{
    uint8_t write_val = 0x5A;
    uint8_t read_val = 0;

    if (w9825g6kh_write8(&hw9825g6kh, 0, &write_val, 1) != 0)
    {
        printf("8-bit write failed\r\n");
        return -1;
    }
    if (w9825g6kh_read8(&hw9825g6kh, 0, &read_val, 1) != 0)
    {
        printf("8-bit read failed\r\n");
        return -1;
    }
    if (read_val != write_val)
    {
        printf("8-bit mismatch: wrote 0x%02X read 0x%02X\r\n", write_val, read_val);
        return -1;
    }
    printf("8-bit single R/W pass\r\n");
    return 0;
}

// 测试单个 16 位读写
int test_16bit_rw()
{
    uint16_t write_val = 0xA55A;
    uint16_t read_val = 0;

    if (w9825g6kh_write16(&hw9825g6kh, 0, &write_val, 1) != 0)
    {
        printf("16-bit write failed\r\n");
        return -1;
    }
    if (w9825g6kh_read16(&hw9825g6kh, 0, &read_val, 1) != 0)
    {
        printf("16-bit read failed\r\n");
        return -1;
    }
    if (read_val != write_val)
    {
        printf("16-bit mismatch: wrote 0x%04X read 0x%04X\n", write_val, read_val);
        return -1;
    }
    printf("16-bit single R/W pass\r\n");
    return 0;
}

// 测试单个 32 位读写
int test_32bit_rw()
{
    uint32_t write_val = 0xDEADBEEF;
    uint32_t read_val = 0;

    if (w9825g6kh_write32(&hw9825g6kh, 0, &write_val, 1) != 0)
    {
        printf("32-bit write failed\r\n");
        return -1;
    }
    if (w9825g6kh_read32(&hw9825g6kh, 0, &read_val, 1) != 0)
    {
        printf("32-bit read failed\r\n");
        return -1;
    }
    if (read_val != write_val)
    {
        printf("32-bit mismatch: wrote 0x%08X read 0x%08X\r\n", write_val, read_val);
        return -1;
    }
    printf("32-bit single R/W pass\r\n");
    return 0;
}

// 8bit 写满 + 读回校验
int test_write_full_and_read_8bit_block()
{
    printf("8-bit block write/read test start\r\n");

    uint8_t write_buf[BLOCK_SIZE_BYTES];
    uint8_t read_buf[BLOCK_SIZE_BYTES];
    uint32_t total_blocks = W9825G6KH_SIZEBYTES / BLOCK_SIZE_BYTES;

    for (uint32_t block = 0; block < total_blocks; block++)
    {
        // 准备数据，简单用地址低 8 位 + 块编号做测试数据
        for (uint32_t i = 0; i < BLOCK_SIZE_BYTES; i++)
        {
            write_buf[i] = (uint8_t)((block + i) & 0xFF);
        }
        if (w9825g6kh_write8_hal(&hw9825g6kh, block * BLOCK_SIZE_BYTES, write_buf, BLOCK_SIZE_BYTES) != 0)
        {
            printf("8-bit write failed at block %u\r\n", block);
            return -1;
        }

        // 每 128KB 进度提示
        if ((block % 128) == 0)
        {
            printf("8-bit write progress: %u KB written\r\n", block + 128);
        }
    }

    for (uint32_t block = 0; block < total_blocks; block++)
    {
        if (w9825g6kh_read8_hal(&hw9825g6kh, block * BLOCK_SIZE_BYTES, read_buf, BLOCK_SIZE_BYTES) != 0)
        {
            printf("8-bit read failed at block %u\r\n", block);
            return -1;
        }
        for (uint32_t i = 0; i < BLOCK_SIZE_BYTES; i++)
        {
            uint8_t expected = (uint8_t)((block + i) & 0xFF);
            if (read_buf[i] != expected)
            {
                printf("8-bit data mismatch at block %u offset %u, expected 0x%02X got 0x%02X\r\n", block, i, expected, read_buf[i]);
                return -1;
            }
        }

        // 每 128KB 进度提示
        if ((block % 128) == 0)
        {
            printf("8-bit read progress: %u KB readed\r\n", block + 128);
        }
    }

    printf("8-bit block write/read test passed\r\n");
    return 0;
}

// 16bit 写满 + 读回校验
int test_write_full_and_read_16bit_block()
{
    printf("16-bit block write/read test start\r\n");

    uint16_t write_buf[BLOCK_SIZE_BYTES / 2];
    uint16_t read_buf[BLOCK_SIZE_BYTES / 2];
    uint32_t total_halfwords = W9825G6KH_SIZEBYTES / 2;
    uint32_t blocks = total_halfwords / (BLOCK_SIZE_BYTES / 2);

    for (uint32_t block = 0; block < blocks; block++)
    {
        for (uint32_t i = 0; i < BLOCK_SIZE_BYTES / 2; i++)
        {
            write_buf[i] = (uint16_t)((block + i) & 0xFFFF);
        }
        if (w9825g6kh_write16(&hw9825g6kh, block * BLOCK_SIZE_BYTES, write_buf, BLOCK_SIZE_BYTES / 2) != 0)
        {
            printf("16-bit write failed at block %u\r\n", block);
            return -1;
        }

        // 每 128KB 进度提示
        if ((block % 128) == 0)
        {
            printf("16-bit write progress: %u KB written\r\n", block + 128);
        }
    }

    for (uint32_t block = 0; block < blocks; block++)
    {
        if (w9825g6kh_read16(&hw9825g6kh, block * BLOCK_SIZE_BYTES, read_buf, BLOCK_SIZE_BYTES / 2) != 0)
        {
            printf("16-bit read failed at block %u\r\n", block);
            return -1;
        }
        for (uint32_t i = 0; i < BLOCK_SIZE_BYTES / 2; i++)
        {
            uint16_t expected = (uint16_t)((block + i) & 0xFFFF);
            if (read_buf[i] != expected)
            {
                printf("16-bit data mismatch at block %u offset %u, expected 0x%04X got 0x%04X\r\n", block, i, expected, read_buf[i]);
                return -1;
            }
        }

        // 每 128KB 进度提示
        if ((block % 128) == 0)
        {
            printf("16-bit read progress: %u KB readed\r\n", block + 128);
        }
    }

    printf("16-bit block write/read test passed\r\n");
    return 0;
}

// 32bit 写满 + 读回校验
int test_write_full_and_read_32bit_block()
{
    printf("32-bit block write/read test start\r\n");

    uint32_t write_buf[BLOCK_SIZE_BYTES / 4];
    uint32_t read_buf[BLOCK_SIZE_BYTES / 4];
    uint32_t total_words = W9825G6KH_SIZEBYTES / 4;
    uint32_t blocks = total_words / (BLOCK_SIZE_BYTES / 4);

    for (uint32_t block = 0; block < blocks; block++)
    {
        for (uint32_t i = 0; i < BLOCK_SIZE_BYTES / 4; i++)
        {
            write_buf[i] = 0x12340000 + block * (BLOCK_SIZE_BYTES / 4) + i;
        }
        if (w9825g6kh_write32_memcpy(&hw9825g6kh, block * BLOCK_SIZE_BYTES, write_buf, BLOCK_SIZE_BYTES / 4) != 0)
        {
            printf("32-bit write failed at block %u\r\n", block);
            return -1;
        }

        // 每 128KB 进度提示
        if ((block % 128) == 0)
        {
            printf("32-bit write progress: %u KB written\r\n", block + 128);
        }
    }

    for (uint32_t block = 0; block < blocks; block++)
    {
        if (w9825g6kh_read32_memcpy(&hw9825g6kh, block * BLOCK_SIZE_BYTES, read_buf, BLOCK_SIZE_BYTES / 4) != 0)
        {
            printf("32-bit read failed at block %u\r\n", block);
            return -1;
        }
        for (uint32_t i = 0; i < BLOCK_SIZE_BYTES / 4; i++)
        {
            uint32_t expected = 0x12340000 + block * (BLOCK_SIZE_BYTES / 4) + i;
            if (read_buf[i] != expected)
            {
                printf("32-bit data mismatch at block %u offset %u, expected 0x%08X got 0x%08X\r\n", block, i, expected, read_buf[i]);
                return -1;
            }
        }

        // 每 128KB 进度提示
        if ((block % 128) == 0)
        {
            printf("32-bit read progress: %u KB readed\r\n", block + 128);
        }
    }

    printf("32-bit block write/read test passed\r\n");
    return 0;
}

void app_init(void)
{
    /* 设备初始化 vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
    /* W9825G6KH */
    if (w9825g6kh_init(&hw9825g6kh, &hsdram1, W9825G6KH_BASEADDR1, FMC_SDRAM_CMD_TARGET_BANK1))
    {
        ERROR_HANDLE(ERROR_NOT_INITIALIZED, "W9825G6KH is init failed");
    }
}
