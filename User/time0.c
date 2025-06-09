#include "time0.h"

// u8 ms_cnt = 0;
// volatile bit tmr0_flag = 0;

static volatile u8 cnt_during_power_on = 0;       // ����������������pwmռ�ձ�ʱ��ʹ�õ��ļ���ֵ
volatile bit flag_time_comes_during_power_on = 0; // ��־λ�������������ڼ䣬����ʱ�䵽��

/**
 * @brief ���ö�ʱ��TMR0����ʱ��Ĭ�Ϲر�
 */
void tmr0_config(void)
{
    __EnableIRQ(TMR0_IRQn); // ʹ��timer0�ж�
    IE_EA = 1;              // ʹ�����ж�

#define PEROID_VAL (SYSCLK / 128 / 1000 - 1) // ����ֵ=ϵͳʱ��/��Ƶ/Ƶ�� - 1
    // ����timer0�ļ������ܣ�����һ��Ƶ��Ϊ1kHz���ж�
    TMR_ALLCON = TMR0_CNT_CLR(0x1);                        // �������ֵ
    TMR0_PRH = TMR_PERIOD_VAL_H((PEROID_VAL >> 8) & 0xFF); // ����ֵ
    TMR0_PRL = TMR_PERIOD_VAL_L((PEROID_VAL >> 0) & 0xFF);
    TMR0_CONH = TMR_PRD_PND(0x1) | TMR_PRD_IRQ_EN(0x1);                          // ������������ʱ�������ж�
    TMR0_CONL = TMR_SOURCE_SEL(0x7) | TMR_PRESCALE_SEL(0x7) | TMR_MODE_SEL(0x1); // ѡ��ϵͳʱ�ӣ�128��Ƶ������ģʽ
}

/**
 * @brief ������ʱ��TMR0����ʼ��ʱ
 */
void tmr0_enable(void)
{
    // ���¸�TMR0����ʱ��
    TMR0_CONL &= ~(TMR_SOURCE_SEL(0x07)); // �����ʱ����ʱ��Դ���üĴ���
    TMR0_CONL |= TMR_SOURCE_SEL(0x06);    // ���ö�ʱ����ʱ��Դ��ʹ��ϵͳʱ�ӣ�Լ21MHz��

    __EnableIRQ(TMR0_IRQn); // ʹ���ж�
    IE_EA = 1;              // �����ж�
}

/**
 * @brief �رն�ʱ��0����ռ���ֵ
 */
void tmr0_disable(void)
{
    // ������ʱ���ṩʱ�ӣ�����ֹͣ����
    TMR0_CONL &= ~(TMR_SOURCE_SEL(0x07)); // �����ʱ����ʱ��Դ���üĴ���
    TMR0_CONL |= TMR_SOURCE_SEL(0x05);    // ���ö�ʱ����ʱ��Դ�������κ�ʱ��

    // �����ʱ���ļ���ֵ
    TMR0_CNTL = 0;
    TMR0_CNTH = 0;

    __DisableIRQ(TMR0_IRQn); // �ر��жϣ���ʹ���жϣ�
}

// extern void fun(void);
// ��ʱ��TMR0�жϷ�����
void TIMR0_IRQHandler(void) interrupt TMR0_IRQn
{
    // �����ж�����IP������ɾ��
    __IRQnIPnPush(TMR0_IRQn);

    // ---------------- �û��������� -------------------

    // �����ж�
    if (TMR0_CONH & TMR_PRD_PND(0x1))
    {
        TMR0_CONH |= TMR_PRD_PND(0x1); // ���pending

        // ms_cnt++;
        cnt_during_power_on++;

        // if (ms_cnt >= 25)
        // {
        //     ms_cnt = 0;
        //     // tmr0_flag = 1;
        // }

        if (cnt_during_power_on >= 13) // 13ms
        {
            cnt_during_power_on = 0;
            flag_time_comes_during_power_on = 1; // �����������ڼ䣬����ÿ�ε���PWMռ�ձȵ�ʱ��
        }

        if (rf_key_para.cur_scan_times < 255)
        {
            rf_key_para.cur_scan_times++; // ����433ң��������ɨ��
        }

        { // rf���빦�ܼ�ʱ���� �ϵ�� �� RF_LEARN_TIMES�ڼ䣬ʹ�ܶ��빦��
            static u16 rf_learn_cnt = 0;

            if (flag_is_rf_enable && flag_is_in_rf_learning)
            {
                rf_learn_cnt++;
                if (rf_learn_cnt >= RF_LEARN_TIMES)
                {
                    rf_learn_cnt = 0;
                    flag_is_in_rf_learning = 0;
                }
            }
            else
            {
                rf_learn_cnt = 0;
            }
        }

        { // ����״̬��⣬�ۼ�һ��ʱ������״̬
            static u16 fan_normal_cnt = 0;
            static u16 fan_err_cnt = 0;

            if (flag_tim_scan_fan_is_err) // �����⵽�˷����쳣
            {
                fan_err_cnt++;
                fan_normal_cnt = 0;

                if (fan_err_cnt > FAN_SCAN_TIMES)
                {
                    fan_err_cnt = 0;
                    cur_fan_status = FAN_STATUS_ERROR;
                }
            }
            else
            {
                fan_normal_cnt++;
                fan_err_cnt = 0;

                if (fan_normal_cnt > FAN_SCAN_TIMES)
                {
                    fan_normal_cnt = 0;
                    cur_fan_status = FAN_STATUS_NORMAL;
                }
            }
        } // ����״̬��⣬�ۼ�һ��ʱ������״̬
    }

    // �˳��ж�����IP������ɾ��
    __IRQnIPnPop(TMR0_IRQn);
}
