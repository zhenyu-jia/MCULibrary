# Error Handle

## 项目简介

本项目展示了一个简单的错误处理机制的实现和使用。通过定义错误类型和处理函数，开发者可以方便地捕获和处理程序中的错误。

## 文件结构

```bash
error.c
error.h
```

## 示例代码

以下是 `example_error.c` 的核心代码：

```c
#include "error.h"

int main()
{
    ERROR_HANDLE(-1, "An error occurred in main function.");
    return 0;
}
```

运行后，程序会输出类似以下内容：

```bash
[ERROR] Code: -1
        Message: An error occurred in main function.
        File: [example_error.c](http://_vscodecontentref_/6)
        Function: main
        Line: 13
```

## 错误处理机制

- 错误类型通过 `ERRORType` 结构体定义，包含错误码、错误信息、文件名、函数名和行号。
- 提供了 `ERROR_CHECK` 和 `ERROR_HANDLE` 宏，简化错误处理的调用。
- 默认的错误处理函数 `error_handle` **可以被用户重定义**以满足特定需求。
