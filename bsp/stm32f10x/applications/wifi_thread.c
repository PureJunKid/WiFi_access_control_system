/**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: wifi_thread.c
**
** ��   ��   ��: �Ž���
**
** �ļ���������: 2016 �� 09 �� 11 ��
**
** ��        ��: ͨ������߳�

** ��־:
2016.09.11  �������ļ�
*********************************************************************************************************/
#include <rtthread.h>
#include "wifi_thread.h"
#include "esp8266.h"

#define rt_thread_delayMs(x) rt_thread_delay(rt_tick_from_millisecond(x))

rt_uint8_t wifi_stack[ 1024 ];	//�߳�ջ
struct rt_thread wifi_thread; 	//�߳̿��ƿ�

/*******************************************************************************
* ������ 	: wifi_thread_entry
* ����   	: ͨ������߳�
* ����     	: - parameter: �߳���ڲ���
* ���     	: None
* ����ֵ    : None
*******************************************************************************/
void wifi_thread_entry(void* parameter)
{
//	rt_thread_delayMs(500);
	init_esp8266();
//	check_state_line();
	WriteTest();
	while(1)
	{
		rt_thread_delayMs(5000);
//		ReadTest();
//		rt_thread_delayMs(10);
	}	
}
