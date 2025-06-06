#ifndef _PWM_H
#define _PWM_H

#include "include.h"
#include <stdio.h>

extern volatile u16 limited_pwm_duty_due_to_temp; // 由温度限制的PWM占空比 （对所有PWM通道都生效）

 
extern volatile u16 adjust_duty;  // 要调整到的占空比
extern volatile u16 c_duty;       // 当前设置的占空比
// extern volatile u16 max_pwm_duty; // 存放占空比的上限值

extern volatile u16 cur_pwm_channel_1_duty; // 当前设置的第二路PWN的占空比

extern volatile bit flag_is_pwm_channel_0_enable; // 标志位，pwm_channel_0 是否使能
extern volatile bit flag_is_pwm_channel_1_enable; // 标志位，pwm_channel_1 是否使能

#define MAX_PWM_DUTY (6000) // 100%占空比   (SYSCLK 4800 0000 /  8000  == 6000)
enum
{
    PWM_DUTY_100_PERCENT = 6000, // 100%占空比
    PWM_DUTY_80_PERCENT = 4800,  // 80%
    PWM_DUTY_75_PERCENT = 4500,  // 75%占空比
    PWM_DUTY_50_PERCENT = 3000,  // 50%占空比
    PWM_DUTY_30_PERCENT = 1800,  // 30%占空比
    PWM_DUTY_25_PERCENT = 1500,  // 25%占空比
};

void pwm_init(void);
// void _My_Adjust_Pwm(float Val);
// void _My_Adjust_Pwm(u16 Val);
extern void set_pwm_duty(void);
// extern void set_p15_pwm_duty(u8 set_duty); // 设置P15 15脚的PWM占空比
// void Adaptive_Duty(void);

// 电源电压低于170V-AC,启动低压保护，电源电压高于170V-AC，关闭低压保护
void according_pin9_to_adjust_pwm(void);
void according_pin9_to_adjust_pin16(void); // 根据9脚的电压来设定16脚的电平

extern u8 get_pwm_channel_0_status(void); // 获取第一路PWM的运行状态
extern u8 get_pwm_channel_1_status(void); // 获取第二路PWM的运行状态
extern void pwm_channel_0_enable(void);
extern void pwm_channel_0_disable(void);
extern void pwm_channel_1_enable(void);
extern void pwm_channel_1_disable(void);

u16 get_pwm_channel_0_adjust_duty(u16 pwm_adjust_duty);

#endif