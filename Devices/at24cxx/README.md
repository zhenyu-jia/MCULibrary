# AT24CXX EEPROM 驱动库

本库实现了对 AT24CXX 系列 I2C EEPROM（如 AT24C01/02/04/08/16/32/64/128/256/512 等）的初始化、就绪检查、读写等操作，适用于 STM32 平台。

## 主要文件

- `at24cxx.h`：AT24CXX 驱动头文件，包含类型定义与 API 声明
- `at24cxx.c`：AT24CXX 驱动实现文件

## 主要接口

- `int at24cxx_init(AT24CXX *hat24cxx, AT24CXX_Type memory_type, I2C_HandleTypeDef *hi2c, uint8_t device_address);`
- `int at24cxx_ready(AT24CXX *hat24cxx, uint32_t trials, uint32_t timeout);`
- `int at24cxx_write(AT24CXX *hat24cxx, uint32_t mem_address, uint8_t *pData, uint32_t size);`
- `int at24cxx_read(AT24CXX *hat24cxx, uint32_t mem_address, uint8_t *pData, uint32_t size);`

详细参数说明见 `at24cxx.h`。

## 使用方法

1. **初始化设备**

    ```c
    AT24CXX hat24c02;
    at24cxx_init(&hat24c02, AT24C02, &hi2c2, 0x00);
    ```

2. **写入数据**

    ```c
    uint8_t data[] = {1, 2, 3, 4};
    at24cxx_write(&hat24c02, 0x00, data, sizeof(data));
    ```

3. **读取数据**

    ```c
    uint8_t buf[4];
    at24cxx_read(&hat24c02, 0x00, buf, sizeof(buf));
    ```

## 注意事项

- 需根据实际 MCU 修改 `#include "stm32f4xx_hal.h"`。
- 写操作有写周期延时，建议不要频繁写入。
- 设备地址需根据硬件连线设置。

## 贡献者

作者：Jia Zhenyu  
日期：2024-07-30
版本：v1.0.0

## 许可证

MIT License
