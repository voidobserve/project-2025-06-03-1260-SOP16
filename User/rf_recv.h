#ifndef __RF_RECV_H
#define __RF_RECV_H

#include "my_config.h"
#include "key_driver.h"

#define RF_ENABLE_PIN P11 // 检测是否有433功能的引脚

#ifdef USE_MY_TEST_PIN // 旧版的编译器会报警告，要加上这一对#ifdef语句
#if USE_MY_TEST_PIN
#define RFIN_PIN P01 // rf信号接收引脚 （测试时使用）
#else
#define RFIN_PIN P03 // rf信号接收引脚， P03 芯片第6脚
#endif               // #if USE_MY_TEST_PIN
#endif               // #ifdef USE_MY_TEST_PIN // 旧版的编译器会报警告，要加上这一对#ifdef语句

/*
    按键的扫描周期，单位：ms
    用在定时器中断，注意不能超过变量的最大值
*/
#define RF_KEY_SCAN_CIRCLE_TIMES (50)
#define RF_KEY_FILTER_TIMES (0) // 按键消抖次数
// #define RF_DETECT_DOUBLE_CLICK_INTERVAL (150) // 检测双击的时间间隔(单位：ms)
#define RF_LONG_PRESS_TIME_THRESHOLD_MS (500) // 长按时间阈值（单位：ms）
// #define HOLD_PRESS_TIME_THRESHOLD_MS (25) // 长按持续(不松手)的时间阈值(单位：ms)，每隔 xx 时间认为有一次长按持续事件
// #define RF_HOLD_PRESS_TIME_THRESHOLD_MS (150) // 长按持续(不松手)的时间阈值(单位：ms)，每隔 xx 时间认为有一次长按持续事件
#define RF_HOLD_PRESS_TIME_THRESHOLD_MS (50) // 长按持续(不松手)的时间阈值(单位：ms)，每隔 xx 时间认为有一次长按持续事件
// #define RF_LOOSE_PRESS_CNT_MS (30)            // 松手计时，松开手多久，才认为是真的松手了

#define RF_ADJUST_TOTAL_TIMES_FOR_HOLD (6000) // 长按调节亮度的总时间（从0%占空比 调节到 100%占空比的时间），单位：ms
#define RF_LEARN_TIMES ((u16)30000)           // 每次上电后，可以进行对码的时间，单位：ms

// 定义rf按键键值，键值在波形上看是从左到右排列
enum RF_KEY_ID
{
    RF_KEY_ID_NONE = 0x00,

    // 控制两路PWM遥控器的按键：
    RF_KEY_ID_1 = 0x14, // R1C1，第一行第一列
    RF_KEY_ID_2 = 0x18, // R1C2
    RF_KEY_ID_3 = 0x10, // R2C1
    RF_KEY_ID_4 = 0x15,
    RF_KEY_ID_5 = 0x05,
    RF_KEY_ID_6 = 0x08,
    RF_KEY_ID_7 = 0x06,
    RF_KEY_ID_8 = 0x09,

    // 控制一路PWM遥控器的按键键值：
    RF_KEY_ID_9 = 0x98,
    RF_KEY_ID_10 = 0x94,
    RF_KEY_ID_11 = 0x92,
    RF_KEY_ID_12 = 0x91,

#ifdef USE_MY_TEST_433_REMOTE // 旧版的编译器会报警告，要加上这一对#ifdef语句
#if USE_MY_TEST_433_REMOTE

    // 测试时使用到的遥控器的按键值：
    RF_KEY_ID_TEST_1 = 0xF8,
    RF_KEY_ID_TEST_2 = 0xF4,
    RF_KEY_ID_TEST_3 = 0xF2,
    RF_KEY_ID_TEST_4 = 0xF1,

#endif //     #if USE_MY_TEST_433_REMOTE
#endif // #ifdef USE_MY_TEST_433_REMOTE
};

// 定义存放rf遥控器的相关信息的结构体类型：
typedef struct
{
    u16 rf_addr;
    // u8 is_first_power_on; // 是否第一次上电 (如果是 0xC5，说明不是第一次上电，说明已经存放了一次遥控器的地址)
    u8 is_addr_valid; // 按键地址是否有效（非0xC5--未记录按键地址，0xC5--记录了按键地址）
} rf_remote_info_t;

extern volatile u8 flag_is_in_rf_learning; // 标志位，是否处于rf对码模式
extern volatile bit flag_is_rf_enable;      // 标志位，是否使能rf遥控器的功能
extern volatile bit flag_is_recved_rf_data; // 是否接收到了rf信号
extern volatile u32 rf_data;                // 存放接收到的rf数据
extern volatile struct key_driver_para rf_key_para;

extern void rf_key_handle(void);

extern void rf_recv_init(void);

#endif