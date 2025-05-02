# Button Handler Module

## 简介

该模块提供了一个通用的按钮处理框架，支持多按键检测、状态变化回调、以及可选的组合按键识别功能，适用于 STM32 等嵌入式系统。

- 支持**多键状态检测**
- 支持**按下/释放事件回调**
- 支持**组合按键识别（可选）**
- **非阻塞扫描方式**，适用于定时器中断调用

## 文件说明

- `button.h`：按钮结构体定义及函数声明。
- `button.c`：按钮处理的实现文件。
- `example_button.c`：示例代码，展示如何初始化并使用按钮模块。

## 快速使用

### 1. 定义按钮对象

```c
BUTTON hbutton1;
```

### 2. 实现按键状态读取函数

```c
uint8_t get_level(BUTTON *hbutton) {
    uint8_t status = 0;
    // 根据实际按键电平读取状态并设置 status 各 bit 位
    return status;
}
```

### 3. 实现按钮回调函数

```c
void button_callback(BUTTON *hbutton, uint8_t pressed, uint8_t released) {
    // 处理按下/释放事件
}
```

### 4. 初始化按钮

```c
button_init(&hbutton1, get_level, button_callback);
```

### 5. 定时调用扫描函数

建议在 **定时器中断** 或 **系统 tick 回调中** 调用：

```c
void SysTick_Handler(void) {
    // 10ms 扫描一次
    button_scan(&hbutton1);
}
```

## 启用组合按键功能（可选）

打开 `button.h` 中的 `#define BUTTON_COMBINATION` 宏定义，并实现组合键回调：

```c
#define BUTTON_COMBINATION

void combination_button_callback(BUTTON *hbutton, uint8_t index) {
    // index 对应组合键序号
}
```

配置组合键：

```c
button_enable_combination(&hbutton1, 0, 0x03, combination_button_callback); // KEY1 + KEY2
```

## 配置参数

- `BUTTON_COMBINATION_COUNT`：最大组合按键数量（默认 15）
- `COMBINATION_TICKS`：组合键识别所需持续时间（默认 10 tick）

## 适配建议

- 可将 `get_level()` 实现为读取 `GPIO` 电平状态。
- 可将 `button_scan()` 放入 `1~10ms` 间隔的定时中断中。
- 使用组合按键时，原有的按下、释放功能将无法使用，不过可以注册单独的组合按键（只注册一个按键作为组合按键），这样可以具有之前的按键按下功能。

## 更新日志

- **v1.0.0**（2024-07-30）：初始版本发布。

## 作者信息

- **作者**：Jia Zhenyu
- **日期**：2024-07-30
- **版本**：1.0.0

## 许可证

该按键框架为开源项目，用户可以自由使用、修改和分发。
