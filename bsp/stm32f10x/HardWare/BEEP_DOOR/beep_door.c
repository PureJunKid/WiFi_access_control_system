/**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: beep_door.c
**
** ��   ��   ��: �Ž���
**
** �ļ���������: 2016 �� 09 �� 20 ��
**
** ��        ��: ������������������

** ��־:
2016.09.20  �������ļ�
*********************************************************************************************************/

#include "beep_door.h"
#include <rtthread.h>
#include "finsh.h"

/* ��ʱ���� */
#define rt_thread_delayMs(x) rt_thread_delay(rt_tick_from_millisecond(x))

//��ʼ��������������������IOΪ�����.��ʹ���������ڵ�ʱ��		    
void rt_hw_beep_door_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	 //ʹ��PE�˿�ʱ��

	BEEP = 0;
	DOOR = 1;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;	 	//�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 	//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 	//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOE, &GPIO_InitStructure);					 	//�����趨������ʼ��

	BEEP = 0;
	DOOR = 1;
}

/* ���� */
void open_door(void)
{
	BEEP = 1;
	DOOR = 0;
	
	rt_thread_delayMs(100);
	
	BEEP = 0;
	DOOR = 1;
}
FINSH_FUNCTION_EXPORT(open_door, open door and beep)
