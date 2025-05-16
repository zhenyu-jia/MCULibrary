#include "at24cxx.h"

AT24CXX hat24c02;

// 按钮回调函数
void button_callback(BUTTON *hbutton, uint8_t pressed, uint8_t released)
{
    if (hbutton == &hbutton1)
    {
        if (pressed == (1 << 0))
        {
            const char write_string[] = "Once there was a king. He likes to write stories, but his stories were not good. As people were afraid of him, they all said his stories were good.";
            if (at24cxx_write(&hat24c02, 0x02, (uint8_t *)write_string, strlen(write_string) + 1)) // +1 用于包括字符串的终止符 '\0'
            {
                ERROR_HANDLE(ERROR_IO, "AT2402 write failed");
            }
        }
        else if (pressed == (1 << 1))
        {
            char read_string[256]; // 用于存储从 EEPROM 读取的数据
            if (at24cxx_read(&hat24c02, 0x02, (uint8_t *)read_string, 148))
            {
                ERROR_HANDLE(ERROR_IO, "AT2402 write failed");
            }
            else
            {
                log_printf(LOG_INFO, "Read from EEPROM: %s\r\n", read_string);
            }
        }
    }
}

void app_init(void)
{
    /* 设备初始化 vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
    /* AT24C02 初始化 */
    if (at24cxx_init(&hat24c02, AT24C02, &hi2c2, 0x00))
    {
        ERROR_HANDLE(ERROR_NOT_INITIALIZED, "AT2402 is init failed");
    }
}
