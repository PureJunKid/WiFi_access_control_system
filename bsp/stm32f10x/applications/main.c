/**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: main.c
**
** ��   ��   ��: �Ž���
**
** �ļ���������: 2016 �� 09 �� 06 ��
**
** ��        ��: ����ϵͳ�ദ������ʼ������

** ��־:
2016.09.06  �������ļ�
*********************************************************************************************************/
#include <rtthread.h>

#include <rfid_thread.h>
#include <user_check_thread.h>
#include <wifi_thread.h>
#include "rtc_thread.h"

#include "p_database.h"
#include "device_config.h"
#include "esp8266.h"
#include "beep_door.h"

#ifdef  TOUCH_SCREEN
#include "touch_screen.h"
#endif  /* TOUCH_SCREEN */

#ifdef  TFT
#include "ILI93xx.h"
#endif  /* TFT */

#ifdef  STemWin
#include "GUI.h"
#include "WM.h"
#endif  /* STemWin */

#ifdef  RT_USING_SDIO
#include <drivers/mmcsd_core.h>
#include <drivers/sdio.h>
#include <drivers/sd.h>
#endif  /* RT_USING_SDIO */

#ifdef RT_USING_DFS
#include <dfs.h>
#include <dfs_posix.h>
#endif  /* RT_USING_DFS */

#ifdef  RT_USING_SPI
#include "spi_bus.h"
#endif  /* RT_USING_SPI */

#ifdef  EmWin_Demo
static rt_uint8_t emwin_demo_stack[ 8192 ];	//�߳�ջ
static struct rt_thread emwin_demo_thread; 	//�߳̿��ƿ�
#endif  /* EmWin_Demo */

#ifdef  TOUCH_SCREEN
static rt_uint8_t touch_screen_stack[ 2048 ];	//�߳�ջ
static struct rt_thread touch_screen_thread; 	//�߳̿��ƿ�
#endif  /* TOUCH_SCREEN */

#define rt_thread_delayMs(x) rt_thread_delay(rt_tick_from_millisecond(x))

/*******************************************************************************
* ������ 	: touch_screen_thread_entry
* ����   	: ������ɨ���߳�
* ����     	: - parameter: �߳���ڲ���
* ���     	: None
* ����ֵ    : None
*******************************************************************************/
#ifdef  TOUCH_SCREEN
static void touch_screen_thread_entry(void* parameter)
{	
	while(1)
	{
		#ifdef  STemWin
		GUI_TOUCH_Exec();
		#endif  /* STemWin */
		rt_thread_delayMs(5);
	}	
}
#endif  /* TOUCH_SCREEN */

/*******************************************************************************
* ������ 	: emwin_demo_thread_entry
* ����   	: emwin_demo�߳�
* ����     	: - parameter: �߳���ڲ���
* ���     	: None
* ����ֵ    : None
*******************************************************************************/
#ifdef  EmWin_Demo
static void emwin_demo_thread_entry(void* parameter)
{	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);//����CRCʱ��
	WM_SetCreateFlags(WM_CF_MEMDEV);
	GUI_Init();  			//STemWin��ʼ��
	GUI_DispString("hello world");
	while(1)
	{
		#ifdef  EmWin_Demo
		#include "GUIDEMO.h"
		GUIDEMO_Main();	
		#endif  /* EmWin_Demo */
		rt_thread_delayMs(5);
	}	
}
#endif  /* EmWin_Demo */

/*******************************************************************************
* ������ 	: user_init_thread_entry
* ����   	: �û���ʼ������
* ����     	: - parameter: �߳���ڲ���
* ���     	: None
* ����ֵ    	: None
*******************************************************************************/
void user_init_thread_entry(void* parameter)
{
	rt_err_t result;

	void rt_enter_critical(void); /* �����ٽ���*/
		
	rt_hw_beep_door_init();
	/* ��ʼ���豸��ַ */
	get_set_device_addr(&device_addr,GET_DEVICE);	
	rt_kprintf("device addr is %d\r\n",device_addr);
	/* ��ʼ���豸���� */
//	rt_memset(this_device_name, 0, sizeof(this_device_name));
//	sprintf(this_device_name,"411");
//	get_set_device_name(this_device_name,SET_DEVICE);	
	get_set_device_name(this_device_name,GET_DEVICE);	
	/* ��ʼ���������� */
	if (init_card_array(card_id_array) == -1)
	{
		rt_kprintf("init card array failed\r\n");
	}
	/* ��ʼ���¼����� */
	rt_event_init(&user_check_event, "user_check_event", RT_IPC_FLAG_FIFO);	

    /* ��ʼ��SPI���� */
	rt_hw_stm32_spi_bus_init();
#ifdef  TFT
	/* ��ʼ��TFTҺ�� */
	TFTLCD_Init();
	POINT_COLOR = WHITE;
	BACK_COLOR = BLACK;
	LCD_ShowString(0,0,320,16,16,(u8 *)"WiFi Access Control System");
#endif  /* TFT */

#ifdef  TOUCH_SCREEN
	/* ��ʼ�������� */
	TP_Init();
#endif  /* TOUCH_SCREEN */
		
#ifdef  EmWin_Demo
    /* ��ʼ��emwin demo�߳� */
    result = rt_thread_init(&emwin_demo_thread,
                            "emwin_demo",
                            emwin_demo_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&emwin_demo_stack[0],
                            sizeof(emwin_demo_stack),
                            15,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&emwin_demo_thread);
    }
#endif  /* EmWin_Demo */
	
#ifdef  TOUCH_SCREEN
    /* ��ʼ��touch_screen�߳� */
    result = rt_thread_init(&touch_screen_thread,
                            "touch_screen",
                            touch_screen_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&touch_screen_stack[0],
                            sizeof(touch_screen_stack),
                            18,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&touch_screen_thread);
    }
#endif  /* TOUCH_SCREEN */

#ifdef  RC522
    /* ��ʼ��rc522�߳� */
    result = rt_thread_init(&rc522_thread,
                            "rc522",
                            rc522_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&rc522_stack[0],
                            sizeof(rc522_stack),
                            25,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&rc522_thread);
    }
#endif  /* RC522 */
	
    /* ��ʼ��Ȩ�޼���߳� */
    result = rt_thread_init(&user_check_thread,
                            "user_check",
                            user_check_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&user_check_stack[0],
                            sizeof(user_check_stack),
                            19,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&user_check_thread);
    }
	
    /* ��ʼ��wifi�߳� */
    result = rt_thread_init(&wifi_thread,
                            "wifi",
                            wifi_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&wifi_stack[0],
                            sizeof(wifi_stack),
                            19,
                            5);
    if (result == RT_EOK)
    {
        //rt_thread_startup(&wifi_thread);
    }
	
	void rt_exit_critical(void); /* �˳��ٽ���*/
	
    /* ��ʼ��rtc�߳� */
    result = rt_thread_init(&rtc_thread,
                            "rtc",
                            rtc_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&rtc_stack[0],
                            sizeof(rtc_stack),
                            5,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&rtc_thread);
    }
	
	void rt_exit_critical(void); /* �˳��ٽ���*/
}
