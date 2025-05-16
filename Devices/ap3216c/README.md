# AP3216C 传感器驱动

本项目为 AP3216C 红外、光照、接近传感器的 STM32 驱动，基于 HAL 库的 I2C 通信实现。

## 文件结构

- [`ap3216c.h`](ap3216c.h)：AP3216C 传感器驱动头文件，包含数据结构、宏定义及 API 声明。
- [`ap3216c.c`](ap3216c.c)：AP3216C 传感器驱动实现文件。
- [`example_ap3216c.c`](example_ap3216c.c)：AP3216C 传感器驱动使用示例。
- [`README.md`](README.md)：项目说明文档。

## 使用方法

1. **硬件连接**  
   将 AP3216C 传感器的 I2C 接口与 STM32 的 I2C 端口连接。

2. **软件配置**  
   - 确保工程已集成 STM32 HAL 库。
   - 包含头文件：

     ```c
     #include "ap3216c.h"
     ```

   - 初始化 I2C 外设（如 `hi2c2`）。

3. **初始化传感器**

   ```c
   AP3216C hap3216c;
   if (ap3216c_init(&hap3216c, &hi2c2, ALS_PS_IR)) {
       // 错误处理
   }
   ```

4. **读取数据**

   ```c
   int ir = ap3216c_get_ir_data(&hap3216c);
   int als = ap3216c_get_als_data(&hap3216c);
   int ps = ap3216c_get_ps_data(&hap3216c);
   int obj = ap3216c_get_ps_object_detect_data(&hap3216c);
   ```

5. **示例代码**  
   详见 [`example_ap3216c.c`](example_ap3216c.c)。

## 主要 API

- `int ap3216c_init(AP3216C *hap3216c, I2C_HandleTypeDef *hi2c, AP3216C_SystemModeType system_mode);`
- `int ap3216c_get_ir_data(AP3216C *hap3216c);`
- `int ap3216c_get_als_data(AP3216C *hap3216c);`
- `int ap3216c_get_ps_data(AP3216C *hap3216c);`
- `int ap3216c_get_ps_object_detect_data(AP3216C *hap3216c);`

详细 API 说明见 [`ap3216c.h`](ap3216c.h)。

## 注意事项

- 需根据实际 MCU 修改头文件包含（如 `stm32f4xx_hal.h`）。
- I2C 地址默认为 `0x1E`。
- 初始化后建议延时等待传感器稳定。

## 贡献者

作者：Jia Zhenyu  
日期：2024-08-02
版本：v1.0.0

## 许可证

MIT License
