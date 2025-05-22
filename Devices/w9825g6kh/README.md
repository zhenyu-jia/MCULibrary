# W9825G6KH STM32 驱动库

本库用于在 STM32 平台（如 STM32F429）上驱动 W9825G6KH SDRAM 芯片，支持 8/16/32 位数据的读写与 DMA 操作。

## 文件结构

- `w9825g6kh.h`：W9825G6KH 设备驱动头文件，包含寄存器定义、结构体和 API 声明。
- `w9825g6kh.c`：W9825G6KH 设备驱动实现，包含初始化、8/16/32 位读写、DMA 读写等函数。
- `example.c`：驱动使用示例，包括单次和分块读写测试。
- `README.md`：说明文档。

## 主要特性

- 支持 8/16/32 位数据的阻塞读写
- 支持 32 位数据的 DMA 读写
- 支持 HAL、直接指针、memcpy 三种访问方式
- 提供分块写入和校验测试用例
- 详细初始化流程和错误码

## 快速开始

1. **包含头文件**

   ```c
   #include "w9825g6kh.h"
   ```

2. **初始化设备**

   ```c
   W9825G6KH hw9825g6kh;
   if (w9825g6kh_init(&hw9825g6kh, &hsdram1, W9825G6KH_BASEADDR1, FMC_SDRAM_CMD_TARGET_BANK1) != 0) {
       // 错误处理
   }
   ```

3. **读写示例**

   ```c
   uint8_t buf[16] = {0};
   w9825g6kh_write8(&hw9825g6kh, 0, buf, 16);
   w9825g6kh_read8(&hw9825g6kh, 0, buf, 16);
   ```

4. **更多用法参考 [example.c](example.c)**

## API 说明

详见 [`w9825g6kh.h`](w9825g6kh.h) 和 [`w9825g6kh.c`](w9825g6kh.c)。

- `int w9825g6kh_init(...)`：初始化 SDRAM
- 支持 HAL、memcpy、直接指针三种方式，详见头文件

### 1. HAL 库访问方式

- `int w9825g6kh_read8_hal(...)` / `w9825g6kh_write8_hal(...)`
- `int w9825g6kh_read16_hal(...)` / `w9825g6kh_write16_hal(...)`
- `int w9825g6kh_read32_hal(...)` / `w9825g6kh_write32_hal(...)`
- `int w9825g6kh_read_dma_hal(...)` / `w9825g6kh_write_dma_hal(...)`

### 2. 直接指针访问方式

- `int w9825g6kh_read8(...)` / `w9825g6kh_write8(...)`
- `int w9825g6kh_read16(...)` / `w9825g6kh_write16(...)`
- `int w9825g6kh_read32(...)` / `w9825g6kh_write32(...)`

### 3. memcpy 访问方式

- `int w9825g6kh_read8_memcpy(...)` / `w9825g6kh_write8_memcpy(...)`
- `int w9825g6kh_read16_memcpy(...)` / `w9825g6kh_write16_memcpy(...)`
- `int w9825g6kh_read32_memcpy(...)` / `w9825g6kh_write32_memcpy(...)`

## 注意事项

- 使用前需正确配置 STM32 FMC/SDRAM 外设。
- 地址、长度需对齐，详见各 API 注释。
- 具体硬件连接和时序参数请参考 W9825G6KH 数据手册和 STM32 HAL 库文档。
- 需根据实际 MCU 修改 `#include "stm32f4xx_hal.h"`。
- 写操作有写周期延时，建议不要频繁写入。
- 设备地址需根据硬件连线设置。

## 贡献者

作者：Jia Zhenyu  
日期：2024-07-30  
版本：v1.0.0

## 许可证

MIT License
