# 日志框架使用文档

## 简介

该日志框架提供了一种灵活的日志记录功能，支持多种日志级别（`DEBUG`、`INFO`、`WARN`、`ERROR`）以及自定义日志输出方式。通过简单的接口，用户可以快速集成日志功能，并根据需要定制日志输出。

---

## 功能概览

- **日志级别**：支持 `DEBUG`、`INFO`、`WARN` 和 `ERROR` 四种日志级别。
- **自定义输出**：支持通过回调函数自定义日志输出方式（如输出到文件、网络等）。
- **调试模式**：通过定义 `_DEBUG` 宏启用调试信息输出。
- **格式化日志**：支持格式化字符串输出日志内容。
- **颜色支持**：支持 ANSI 转义序列，为不同日志级别添加颜色（可选）。

---

## 文件结构

- **`log.h`**：日志框架的头文件，定义了接口和日志级别。
- **`log.c`**：日志框架的实现文件，包含日志记录和输出的具体实现。
- **`example_log.c`**：日志框架的使用示例。

---

## 快速开始

### 1. 引入头文件

在需要使用日志功能的文件中引入 `log.h`：

```c
#include "log.h"
```

### 2. 设置日志输出方式

通过 `set_log_output` 函数设置自定义日志输出方式。如果不设置，默认使用 `printf` 输出日志。

#### 示例：输出到控制台

```c
void log_to_console(const char *log)
{
    printf("[Console] %s", log);
}

set_log_output(log_to_console);
```

#### 示例：输出到文件

```c
void log_to_file(const char *log)
{
    FILE *file = fopen("log.txt", "a");
    if (file)
    {
        fputs(log, file);
        fclose(file);
    }
}

set_log_output(log_to_file);
```

#### 示例：同时输出到控制台和文件

```c
void log_to_multiple_targets(const char *log)
{
    // 输出到控制台
    printf("[Multiple Targets] %s", log);

    // 输出到文件
    FILE *file = fopen("log.txt", "a");
    if (file)
    {
        fputs(log, file);
        fclose(file);
    }
}

set_log_output(log_to_multiple_targets);
```

### 3. 记录日志

使用 `log_printf` 宏记录日志。该宏会自动传递当前函数名和行号。

#### 示例

```c
log_printf(LOG_INFO, "This is an informational message.\n");
log_printf(LOG_WARN, "This is a warning message.\n");
log_printf(LOG_ERROR, "This is an error message.\n");
log_printf(LOG_DEBUG, "This is a debug message.\n");
```

---

## 示例代码

以下是完整的使用示例：

```c
#include "log.h"
#include <stdio.h>

/* 自定义日志输出函数：输出到控制台 */
void log_to_console(const char *log)
{
    printf("[Console] %s", log);
}

/* 自定义日志输出函数：输出到文件 */
void log_to_file(const char *log)
{
    FILE *file = fopen("log.txt", "a");
    if (file)
    {
        fputs(log, file);
        fclose(file);
    }
}

/* 自定义日志输出函数：同时输出到控制台和文件 */
void log_to_multiple_targets(const char *log)
{
    // 输出到控制台
    printf("[Multiple Targets] %s", log);

    // 输出到文件
    FILE *file = fopen("log.txt", "a");
    if (file)
    {
        fputs(log, file);
        fclose(file);
    }
}

int main(void)
{
    // 设置日志输出到控制台
    set_log_output(log_to_console);
    log_printf(LOG_INFO, "This is an informational message.\n");

    // 设置日志输出到文件
    set_log_output(log_to_file);
    log_printf(LOG_WARN, "This is a warning message.\n");

    // 设置日志输出到多个目标
    set_log_output(log_to_multiple_targets);
    log_printf(LOG_ERROR, "This is an error message.\n");

    // 调试日志（需要定义 _DEBUG 宏）
    log_printf(LOG_DEBUG, "This is a debug message.\n");

    return 0;
}
```

---

## 接口说明

### 1. `log_printf`

记录日志的宏，简化日志记录调用。

**定义**：

```c
#define log_printf(level, fmt...) log_message(level, __FUNCTION__, __LINE__, fmt)
```

**参数**：

- `level`：日志级别（`LOG_DEBUG`、`LOG_INFO`、`LOG_WARN`、`LOG_ERROR`）。
- `fmt`：格式化字符串及其参数。

### 2. `set_log_output`

设置自定义日志输出函数。

**定义**：

```c
void set_log_output(log_output_func func);
```

**参数**：

- `func`：自定义日志输出函数，类型为 `void (*log_output_func)(const char *)`。

### 3. 日志级别枚举

**定义**：

```c
typedef enum
{
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
} LOGLEVEL;
```

**说明**：

- `LOG_DEBUG`：调试信息。
- `LOG_INFO`：普通信息。
- `LOG_WARN`：警告信息。
- `LOG_ERROR`：错误信息。

---

## 调试模式

通过定义 `_DEBUG` 宏启用调试模式。在调试模式下，日志功能会输出更多详细信息。

**启用方式**：

在编译时定义 `_DEBUG`：

```c
#define _DEBUG
```

或者通过编译器选项：

```bash
gcc -D_DEBUG -o example_log example_log.c log.c
```

---

## 注意事项

1. **日志级别过滤**：日志框架会根据 `LOG_LEVEL` 宏过滤低于指定级别的日志。默认级别为 `LOG_DEBUG`。

     ```c
     #define LOG_LEVEL LOG_WARN
     ```

2. **线程安全**：当前实现未考虑线程安全。如果在多线程环境中使用，请自行添加同步机制。

3. **缓冲区大小**：日志消息的缓冲区大小为 `LOG_BUF_SIZE`（默认 256 字节）。如果日志内容较长，请自行调整。

---

## 常见问题

### 1. 为什么没有输出日志？

- 确保定义了 `_DEBUG` 宏。
- 检查是否设置了合适的日志级别（`LOG_LEVEL`）。
- 确保自定义日志输出函数工作正常。

### 2. 如何输出到多个目标？

可以在自定义日志输出函数中实现多目标输出。例如，同时输出到文件和控制台：

```c
void log_to_multiple_targets(const char *log)
{
    printf("%s", log); // 输出到控制台

    FILE *file = fopen("log.txt", "a");
    if (file)
    {
        fputs(log, file); // 输出到文件
        fclose(file);
    }
}

set_log_output(log_to_multiple_targets);
```

---

## 更新日志

- **v1.1.1**（2025-04-21）：取消了 **RT-Thread** 支持。
- **v1.1.0**（2025-04-21）：添加 **颜色** 支持和 **RT-Thread** 支持，优化日志输出功能。
- **v1.0.0**（2024-07-23）：初始版本发布。

---

## 作者信息

- **作者**：Jia Zhenyu
- **日期**：2025-04-22
- **版本**：1.1.1

---

## 许可证

该日志框架为开源项目，用户可以自由使用、修改和分发。
