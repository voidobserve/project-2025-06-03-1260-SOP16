#include "rf_recv.h"

volatile bit flag_is_recved_rf_data = 0; // 是否接收到了rf信号
volatile u32 rf_data = 0; // 存放接收到的rf数据

void rf_recv_init(void)
{
    P0_PU |= GPIO_P01_PULL_UP(0x01); // 上拉
    P0_MD0 &= ~(GPIO_P01_MODE_SEL(0x03)); // 输入模式
}