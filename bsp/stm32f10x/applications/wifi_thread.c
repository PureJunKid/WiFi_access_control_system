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
	rt_err_t status = RT_EOK;
	rt_uint32_t recved_event = 0;
	
	/* ��ʼ��ESP8266 */
	init_esp8266();
	while(1)
	{
		/* �ȴ�ˢ����ˢָ���¼� */
		status = rt_event_recv(	&esp8266_event,                       //�¼�����ľ��
								hspi_rx,                              //�����̸߳���Ȥ���¼�
								RT_EVENT_FLAG_CLEAR|RT_EVENT_FLAG_OR, //�߼�������¼�
								RT_WAITING_FOREVER,                   //������ʱ
								&recved_event                         //ָ���յ����¼�
							  );
		/* ���ݽ��յ����¼�ִ�в�ͬ�Ĵ��� */
		if(status == RT_EOK)
		{
			/* hspi�����¼� */
			if (recved_event & hspi_rx)
			{
				esp8266_spi_read();
			}
		}
	}	
}
