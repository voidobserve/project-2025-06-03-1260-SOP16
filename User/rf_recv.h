#ifndef __RF_RECV_H
#define __RF_RECV_H

#include "my_config.h"

// #define RFIN_PIN P03 // rf信号接收引脚， P03 芯片第6脚
#define RFIN_PIN P01 // rf信号接收引脚 （测试时使用）

extern volatile bit flag_is_recved_rf_data; // 是否接收到了rf信号
extern volatile u32 rf_data; // 存放接收到的rf数据





#endif 