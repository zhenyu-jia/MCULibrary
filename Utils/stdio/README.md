# stdio_redirect

将标准输入输出（`printf` / `scanf` / `fprintf(stderr, ...)` 等）重定向到 `UART` 或 `USB`，兼容 `MDK` 与 `GCC` 编译器。

## ✅ 特性

- 支持 `UART` 或 `USB` 作为重定向外设。
- 支持 `MDK` 与 `GCC` 编译器（通过宏定义切换）。
- 标准输入输出 `printf`、`scanf`、`stderr` 全部支持。
- 自动添加串口回显与换行处理，提升终端交互体验。

## 📁 文件结构

```bash
stdio_redirect.c      // 重定向实现源文件
README.md             // 本说明文档
```

## 🛠 使用方式

### 1. 配置宏定义

在 `stdio_redirect.c` 顶部设置如下两个宏定义：

```c
#define STDIO_REDIRECT_PERIPHERALS _UART // 选择 _UART 或 _USB
#define STDIO_REDIRECT_COMPILER _MDK     // 选择 _MDK 或 _GCC
```

### 2. UART 配置要求

- 默认使用 `huart1`，可修改：

    ```c
    #define STDIO_REDIRECT_UART huart1
    ```

- 确保 `usart.c` 中初始化完成 `UART1`，并开启中断（建议用阻塞模式收发）。

### 3. USB 配置要求（选用时）

- 需启用 USB CDC 类。
- 并在 `usbd_cdc_if.c` 中实现 `CDC_Transmit_FS()`。
- 注意：接收功能暂未实现。

### 4. Keil 下的配置

想使用哪个就启用相应的功能，`STDERR` 是标准错误，`STDIN` 是标准输入，`STDOUT` 是标准输出，将相应的 `Variant` 设置为 `User`.
![Keil](./images/keil.png)

## 🧪 示例

```c
char str[100];
printf("请输入一行字符串：");
scanf("%s", str);
printf("你输入的是：%s\n", str);
```

## ⚠️ 注意事项

- 推荐使用回显功能以便交互时可见输入内容。
- 若使用 USB，务必保证 USB 设备初始化完成后再调用打印函数。

## 👨‍💻 作者

- **作者**: Jia Zhenyu
- **日期**: 2024-07-03
- **版本**: 1.0.0

## 更新记录

- **v1.0.0**（2024-07-03）：初始版本发布。

## 许可证

该项目为开源项目，用户可以自由使用、修改和分发。
