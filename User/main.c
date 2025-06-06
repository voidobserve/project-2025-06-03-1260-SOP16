/**
 ******************************************************************************
 * @file    main.c
 * @author  HUGE-IC Application Team
 * @version V1.0.0
 * @date    02-09-2022
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2021 HUGE-IC</center></h2>
 *
 * 版权说明后续补上
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "my_config.h"
#include "include.h"
#include <math.h>
#include <stdio.h>

float step = 70;
float mi; // 幂
// float rus; // 10的幂次方
// float r_ms = 0;
// #define USER_BAUD (115200UL)
// #define USER_UART_BAUD ((SYSCLK - USER_BAUD) / (USER_BAUD))

volatile bit flag_is_in_power_on; // 是否处于开机缓启动

#if USE_MY_DEBUG // 打印串口配置

#define UART0_BAUD 115200
#define USER_UART0_BAUD ((SYSCLK - UART0_BAUD) / (UART0_BAUD))
// 重写puchar()函数
char putchar(char c)
{
    while (!(UART0_STA & UART_TX_DONE(0x01)))
        ;
    UART0_DATA = c;
    return c;
}

void my_debug_config(void)
{
    // 作为发送引脚
    // P1_MD0 &= (GPIO_P13_MODE_SEL(0x3));
    // P1_MD0 |= GPIO_P13_MODE_SEL(0x1);            // 配置为输出模式
    // FOUT_S13 |= GPIO_FOUT_UART0_TX;              // 配置为UART0_TX
    // UART0_BAUD1 = (USER_UART0_BAUD >> 8) & 0xFF; // 配置波特率高八位
    // UART0_BAUD0 = USER_UART0_BAUD & 0xFF;        // 配置波特率低八位
    // UART0_CON0 = UART_STOP_BIT(0x0) |
    //              UART_EN(0x1); // 8bit数据，1bit停止位

    P0_MD0 &= (GPIO_P00_MODE_SEL(0x3));
    P0_MD0 |= GPIO_P00_MODE_SEL(0x1);            // 配置为输出模式
    FOUT_S00 |= GPIO_FOUT_UART0_TX;              // 配置为UART0_TX
    UART0_BAUD1 = (USER_UART0_BAUD >> 8) & 0xFF; // 配置波特率高八位
    UART0_BAUD0 = USER_UART0_BAUD & 0xFF;        // 配置波特率低八位
    UART0_CON0 = UART_STOP_BIT(0x0) |
                 UART_EN(0x1); // 8bit数据，1bit停止位
}
#endif // USE_MY_DEBUG // 打印串口配置

// 开机缓启动，调节占空比：
void adjust_pwm_duty_when_power_on(void)
{
    // if (jump_flag == 1)
    // {
    //     // break;
    //     return
    // }
    if (c_duty < 6000)
    {
        mi = (step - 1) / (253 / 3) - 1;
        step += 0.5;
        c_duty = pow(5, mi) * 60; // C 库函数 double pow(double x, double y) 返回 x 的 y 次幂
    }

    if (c_duty >= 6000)
    {
        c_duty = 6000;
    }
    // printf("c_duty %d\n",c_duty);

    // delay_ms(16); // 每16ms调整一次PWM的脉冲宽度 ---- 校验码A488对应的时间
    // delay_ms(11); // 16 * 0.666 约为10.656   ---- 校验码B5E3对应的时间
}

void main(void)
{
    // 看门狗默认打开, 复位时间2s
    WDT_KEY = WDT_KEY_VAL(0xDD); //  关闭看门狗 (如需配置看门狗请查看“WDT\WDT_Reset”示例)

    system_init();

    // 关闭HCK和HDA的调试功能
    WDT_KEY = 0x55;  // 解除写保护
    IO_MAP &= ~0x01; // 清除这个寄存器的值，实现关闭HCK和HDA引脚的调试功能（解除映射）
    WDT_KEY = 0xBB;  // 写一个无效的数据，触发写保护

#if USE_MY_DEBUG // 打印串口配置
    // 初始化打印
    my_debug_config();
    printf("sys reset\n");

    // 输出模式：
    // P1_MD0 &= (GPIO_P13_MODE_SEL(0x3));
    // P1_MD0 |= GPIO_P13_MODE_SEL(0x1); // 配置为输出模式
    // FOUT_S13 = GPIO_FOUT_AF_FUNC;     // 选择AF功能输出
#endif // 打印串口配置

    // 过压保护  16脚-----P14
    //		P1_MD1   &= ~GPIO_P14_MODE_SEL(0x03);
    //		P1_MD1   |=  GPIO_P14_MODE_SEL(0x01);
    //		FOUT_S14  =  GPIO_FOUT_AF_FUNC;
    ///////////////////////////////////////////

#if 1
    adc_pin_config(); // 配置使用到adc的引脚
    // adc_sel_pin(ADC_SEL_PIN_GET_TEMP);
    tmr0_config(); // 配置定时器，默认关闭
    pwm_init();    // 配置pwm输出的引脚
    tmr1_config();

    timer2_config();
#endif

    rf_recv_init(); // rf功能初始化

    // adc_sel_pin(ADC_SEL_PIN_GET_VOL); // 切换到9脚，准备检测9脚的电压

    limited_max_pwm_duty = MAX_PWM_DUTY;
    // limited_adjust_pwm_duty = MAX_PWM_DUTY;
// ===================================================================
#if 0 // 开机缓慢启动（PWM信号变化平缓）
    P14 = 0; // 16脚先输出低电平
    c_duty = 0;
    flag_is_in_power_on = 1; // 表示到了开机缓启动
    // while (c_duty < 6000)
    while (c_duty < limited_max_pwm_duty) // 当c_duty 大于 限制的最大占空比后，退出
    {
        adc_update_pin_9_adc_val();        // 采集并更新9脚的ad值
        update_max_pwm_duty_coefficient(); // 更新当前的最大占空比

#if USE_MY_DEBUG // 直接打印0，防止在串口+图像上看到错位

        // printf(",b=0,"); // 防止在串口图像错位

#endif

        if (flag_is_pwm_sub_time_comes) // pwm递减时间到来
        {
            flag_is_pwm_sub_time_comes = 0;

            /*
                只要有一次跳动，退出开机缓启动(改成等到变为100%占空比再退出)，
                由于adjust_duty初始值为6000，直接退出会直接设置占空比为adjust_duty对应的值，
                会导致灯光闪烁一下
            */
            if (adc_val_pin_9 >= ADC_VAL_WHEN_UNSTABLE)
            {
                // if (c_duty >= PWM_DUTY_100_PERCENT)
                if (c_duty >= limited_max_pwm_duty)
                {
                    // adjust_duty = c_duty;
                    break;
                }
            }
        }

        if (flag_time_comes_during_power_on) // 如果调节时间到来 -- 13ms
        {
            flag_time_comes_during_power_on = 0;
            adjust_pwm_duty_when_power_on();
        }

        set_pwm_duty(); // 将 c_duty 写入pwm对应的寄存器
        set_p15_pwm_duty(c_duty);

#if USE_MY_DEBUG
        // printf("power_on_duty %u\n", c_duty);
#endif //  USE_MY_DEBUG
    }
#endif // 开机缓慢启动（PWM信号变化平缓）

// MY_DEBUG:
    c_duty = MAX_PWM_DUTY; // 测试用
    cur_pwm_channel_0_duty = MAX_PWM_DUTY; // 测试用
    set_pwm_channel_0_duty(cur_pwm_channel_0_duty); // 测试用

    adjust_duty = c_duty;    // 缓启动后，立即更新 adjust_duty 的值
    flag_is_in_power_on = 0; // 表示退出了开机缓启动
    // ===================================================================

    // 测试样机的最大功率：
    // c_duty = MAX_PWM_DUTY;
    // set_pwm_duty();

    // 测试是不是由于频繁检测到电压在开机和关机之间，导致闪灯：
    // c_duty = MAX_PWM_DUTY * 1 / 100;
    // c_duty = MAX_PWM_DUTY * 3 / 100;
    // c_duty = MAX_PWM_DUTY * 5 / 1000;
    // delay_ms(1000);

    while (1)
    {

#if 1
        adc_update_pin_9_adc_val(); // 采集并更新9脚的ad值（9脚，检测发动机功率是否稳定的引脚）
        update_max_pwm_duty_coefficient(); // 根据当前旋钮的挡位，限制能调节到的最大的pwm占空比
        // temperature_scan();               // 检测热敏电阻一端的电压值
        set_duty();                       // 设定到要调节到的脉宽 (设置adjust_duty)
        according_pin9_to_adjust_pin16(); // 根据9脚的电压来设定16脚的电平

#if USE_MY_DEBUG
        // printf("adjust_duty %u\n", adjust_duty);
        // printf(",b=%u,", adjust_duty);
#endif //  USE_MY_DEBUG

#endif

        // if (flag_is_recved_rf_data)
        // {
        //     flag_is_recved_rf_data = 0;

        //     printf("recv data: 0x %lx\n", rf_data);
        // }

        {
            if (flag_is_rf_enable) // 如果使能了rf遥控器的功能
            {
                key_driver_scan(&rf_key_para);
                rf_key_handle();
            }
        }
    }
}

/**
 * @}
 */

/*************************** (C) COPYRIGHT 2022 HUGE-IC ***** END OF FILE *****/
