#ifndef __TIMER1_H
#define __TIMER1_H

// #include "my_config.h"
#include "include.h"

extern volatile u32 tmr1_cnt; // ��ʱ��TMR1�ļ���ֵ��ÿ�����жϷ������л��һ�������ۼ��¶ȹ��ȵ�ʱ�䣩

void tmr1_config(void);  // ���ö�ʱ������ʱ��Ĭ�Ϲر�
void tmr1_enable(void);  // ������ʱ������ʼ��ʱ
void tmr1_disable(void); // �رն�ʱ������ռ���ֵ

#endif
