/**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: rtc_thread.c
**
** ��   ��   ��: �Ž���
**
** �ļ���������: 2016 �� 09 �� 11 ��
**
** ��        ��: ʵʱʱ������߳�

** ��־:
2016.09.11  �������ļ�
*********************************************************************************************************/
#include <rtthread.h>
#include "rtc_thread.h"
#include "soft_iic.h"
#include "ds1307.h"
#include "finsh.h"

#define rt_thread_delayMs(x) rt_thread_delay(rt_tick_from_millisecond(x))

rt_uint8_t rtc_stack[ 1024 ];	//�߳�ջ
struct rt_thread rtc_thread; 	//�߳̿��ƿ�

/*******************************************************************************
* ������ 	: rtc_thread_entry
* ����   	: ͨ������߳�
* ����     	: - parameter: �߳���ڲ���
* ���     	: None
* ����ֵ    : None
*******************************************************************************/
void rtc_thread_entry(void* parameter)
{
	IIC_GPIO_Init();
	
	if(DS1307_Check() != 0)
	{
		rt_kprintf("DS1307 check failed, init it!\r\n");
		TimeValue.year = 0;
		TimeValue.month = 1;
		TimeValue.date = 1;
		TimeValue.hour = 0;
		TimeValue.minute = 0;
		TimeValue.second = 0;
		TimeValue.week = 6;
		DS1307_Time_Init(&TimeValue);
	}
	if(DS1307_Check() != 0)
	{
		rt_kprintf("init ds1307 failed!\r\n");
	}
	
	while(1)
	{
		DS1307_ReadWrite_Time(1);
		rt_kprintf("20%02d-%02d-%02d %02d-%02d-%02d weed: %d\r\n", TimeValue.year, TimeValue.month, 
					TimeValue.date, TimeValue.hour, TimeValue.minute, TimeValue.second, TimeValue.week);
		rt_thread_delayMs(1000);
	}	
}

/*******************************************************************************
* ������ 	: set_time
* ����   	: ����ʱ��
* ����     	: - ������ʱ��������
* ���     	: None
* ����ֵ    : None
*******************************************************************************/
void set_time(u8 year, u8 month, u8 date, u8 hour, u8 minute, u8 second, u8 week)
{
	TimeValue.year = year;
	TimeValue.month = month;
	TimeValue.date = date;
	TimeValue.hour = hour;
	TimeValue.minute = minute;
	TimeValue.second = second;
	TimeValue.week = week;
	DS1307_Time_Init(&TimeValue);
}
FINSH_FUNCTION_EXPORT(set_time, set time and date)

