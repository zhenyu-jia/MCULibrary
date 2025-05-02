# USB 功能板级支持包 (BSP)

该项目实现了用于控制和重置 USB 功能的板级支持包 (BSP)。通过提供的功能，可以实现 USB 设备的重新枚举操作，从而解决某些 USB 连接问题或重新初始化 USB 设备。

## 文件结构

```text
usb.c
usb.h
```


## 功能说明

### `USB_Reset`

`USB_Reset` 函数通过控制 USB 数据正 (DP) 引脚实现 USB 设备的重新枚举操作。  
该过程包括以下步骤：
    1. 将 USB DP 引脚设置为推挽输出模式。
    2. 将 DP 引脚拉低 100 毫秒。
    3. 将 DP 引脚拉高，触发 USB 设备重新枚举。

### 示例代码

以下是使用 `USB_Reset` 函数的示例：

```c
#include "usb.h"
#include "usb_device.h"

int main(void)
{
    // 初始化硬件
    HAL_Init();
    SystemClock_Config();

    // 触发 USB 重新枚举
    USB_Reset();

    // 初始化 USB 设备
    MX_USB_DEVICE_Init();

    while (1)
    {
        // 主循环
    }
}
```

### 注意事项

在调用 USB_Reset 函数之前，请确保已经启用了 USB DP 引脚所在端口的 GPIO 时钟。
该功能适用于需要通过软件触发 USB 重新枚举的场景。

## 更新日志

- **V1.0.0**（2024-07-23）：初始版本发布。

## 作者信息

- **作者**：Jia Zhenyu
- **日期**：2024-07-23
- **版本**：1.0.0
