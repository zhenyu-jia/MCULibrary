# MultiFunc Button Library

该项目是一个多功能按钮处理库，提供了按钮去抖动、状态机处理以及多种按钮事件的支持，包括短按、长按、单击、双击、重复按下等功能。
该项目是在 [MultiButton](https://github.com/0x1abin/MultiButton) 的基础上进行修改和扩展。

## 功能特性

- **去抖动处理**：通过计数器实现按钮的去抖动功能。
- **状态机设计**：支持多种按钮事件，包括：
  - 按下 (PRESS_DOWN)
  - 释放 (PRESS_UP)
  - 重复按下 (PRESS_REPEAT)
  - 单击 (SINGLE_CLICK)
  - 双击 (DOUBLE_CLICK)
  - 长按开始 (LONG_PRESS_START)
  - 长按保持 (LONG_PRESS_HOLD)
- **回调机制**：通过回调函数处理按钮事件。
- **链表管理**：支持多个按钮的动态管理。

## 文件结构

- **multifunc_button.h**：头文件，包含库的声明。
- **multifunc_button.c**：实现文件，包含库的实现。
- **example_multifunc_button.c**：示例代码。

## 使用方法

### 1. 初始化按钮

在使用按钮之前，需要初始化按钮结构体并设置电平读取函数和事件回调函数。

```c
void my_button_callback(BUTTON *btn, BUTTON_Event event) {
    // 处理按钮事件
}

BUTTON my_button;
button_init(&my_button, my_pin_level_function, my_button_callback);
```

### 2. 启动按钮处理

将按钮添加到处理链表中以开始处理。

```c
button_start(&my_button);
```

### 3. 定期调用 `button_ticks`

在主循环或定时器中断中定期调用 `button_ticks` 函数以更新按钮状态。

```c
while (1) {
    button_ticks();
}
// 或者
void timer_interrupt_handler() {
    button_ticks();
}
```

### 4. 停止按钮处理

如果不再需要处理某个按钮，可以将其从链表中移除。

```c
button_stop(&my_button);
```

## 示例代码

参考 `example_multifunc_button.c` 文件中的完整示例。

## 贡献

欢迎提交 Issue 或 Pull Request 来改进此项目

## 许可证

请遵守原作者的许可证。
