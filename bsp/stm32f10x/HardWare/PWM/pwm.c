/**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: pwm.c
**
** ��   ��   ��: �Ž���
**
** �ļ���������: 2016 �� 09 �� 29 ��
**
** ��        ��: Һ������PWM����

** ��־:
2016.09.08  �������ļ�
*********************************************************************************************************/

#include "pwm.h"
#include "finsh.h"

/*******************************************************************************
* ������ 	: set_lcd_led
* ����   	: ����LCD���������
* ����     	: - device: 0���� 1���� - permillage: ����(0-1000)
* ���     	: None
* ����ֵ    	: None
*******************************************************************************/
void set_lcd_led(u8 device, u16 permillage)
{
	switch(device)
	{
		case 0:
		{
			TIM_SetCompare2(TIM3,permillage*14400/1000);
		}	break;
		case 1:
		{
			TIM_SetCompare1(TIM3,permillage*14400/1000);		   
		}	break;
		default: rt_kprintf("�豸ѡ����� device is %d , 0: ���� 1: ����",device);
	}
}
FINSH_FUNCTION_EXPORT(set_lcd_led, set lcd led)

/*******************************************************************************
* ������ 	: rt_hw_lcd_led_init
* ����   	: ��ʼ��LCD�����
* ����     	: - arr: �Զ���װֵ - psc: ʱ��Ԥ��Ƶ��
* ���     	: None
* ����ֵ    	: None
*******************************************************************************/
void rt_hw_lcd_led_init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); //Timer3��ȫ��ӳ��
 
   //���ø�����Ϊ�����������,���TIM3��PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //TIM3_CH1��TIM_CH2 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIO
 
   //��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	set_lcd_led(0, 0);	//�ر�����Һ������
	set_lcd_led(1, 0);	//�ر�����Һ������
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
	
	//��ʼ��TIM3 Channel PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ե�
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC1
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC2
	
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3
}
