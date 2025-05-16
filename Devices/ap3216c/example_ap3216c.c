#include "ap3216c.h"

AP3216C hap3216c;


void task3(void *arg)
{
    printf("IR Data: %d\r\n", ap3216c_get_ir_data(&hap3216c));
    printf("ALS Data: %d\r\n", ap3216c_get_als_data(&hap3216c));
    printf("PS Data: %d\r\n", ap3216c_get_ps_data(&hap3216c));
    printf("PSObjectDetect Data: %d\r\n", ap3216c_get_ps_object_detect_data(&hap3216c));
}

void app_init(void)
{
    /* 设备初始化 vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
    /* AP3216C */
    if (ap3216c_init(&hap3216c, &hi2c2, ALS_PS_IR))
    {
        ERROR_HANDLE(ERROR_NOT_INITIALIZED, "AP3216C is init failed");
    }
}
