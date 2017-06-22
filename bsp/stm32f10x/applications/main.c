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
#include <finsh.h>
#include <stdint.h>
#include <rtdevice.h>

#include "rfid_thread.h"
#include "user_check_thread.h"
#include "wifi_thread.h"
#include "rtc_thread.h"

#include "p_database.h"
#include "att_database.h"

#include "device_config.h"
#include "esp8266.h"
#include "beep_door.h"
#include "esp8266_cmd.h"
#include "global.h"
#include "misc.h"

#ifdef  TOUCH_SCREEN
#include "touch_screen.h"
#endif  /* TOUCH_SCREEN */

#ifdef  TFT
#include "ILI93xx.h"
#endif  /* TFT */

#ifdef  STemWin
#include "GUI.h"
#include "WM.h"

#include "WindowDLG.h"
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

#ifdef  STemWin
static rt_uint8_t emwin_stack[ 8192 ];    //�߳�ջ
static struct rt_thread emwin_thread;     //�߳̿��ƿ�
#endif  /* STemWin */

#ifdef  EmWin_Demo
static rt_uint8_t emwin_demo_stack[ 8192 ];    //�߳�ջ
static struct rt_thread emwin_demo_thread;     //�߳̿��ƿ�
#endif  /* EmWin_Demo */

#ifdef  TOUCH_SCREEN
static rt_uint8_t touch_screen_stack[ 2048 ];    //�߳�ջ
static struct rt_thread touch_screen_thread;     //�߳̿��ƿ�
#endif  /* TOUCH_SCREEN */

/*******************************************************************************
* ������     : touch_screen_thread_entry
* ����       : ������ɨ���߳�
* ����         : - parameter: �߳���ڲ���
* ���         : None
* ����ֵ    : None
*******************************************************************************/
#ifdef  TOUCH_SCREEN
static void touch_screen_thread_entry(void* parameter)
{
    GUI_PID_STATE State0, State1;
    uint16_t touch_x0 = 0, touch_x1 = 0;
    uint16_t touch_y0 = 0, touch_y1 = 0;
    while(1)
    {
        #ifdef  STemWin
        if (tp_irq0 == 0)
        {
            TP_Read_XY(LCD0, &touch_y0, &touch_x0);
            
             State0.x = tp_dev0.xfac*touch_x0+tp_dev0.xoff;//�����ת��Ϊ��Ļ����
            State0.y = tp_dev0.yfac*touch_y0+tp_dev0.yoff;  
            State0.Pressed = 1;
            State0.Layer = 0;
            GUI_TOUCH_StoreStateEx(&State0);
        }
        else
        {
            State0.Pressed = 0;
            State0.Layer = 0;    
            GUI_TOUCH_StoreStateEx(&State0);
        }
        if (tp_irq1 == 0)
        {
            TP_Read_XY(LCD1, &touch_y1, &touch_x1);

             State1.x = tp_dev1.xfac*touch_x1+tp_dev1.xoff;//�����ת��Ϊ��Ļ����
            State1.y = tp_dev1.yfac*touch_y1+tp_dev1.yoff;  
            State1.Pressed = 1;
            State1.Layer = 1;
            GUI_TOUCH_StoreStateEx(&State1);
        }
        else
        {
            State1.Pressed = 0;
            State1.Layer = 1;    
            GUI_TOUCH_StoreStateEx(&State1);
        }
        #endif  /* STemWin */
        RT_THREAD_DELAY_MS(10);
    }    
}
#endif  /* TOUCH_SCREEN */

/*******************************************************************************
* ������     : emwin_demo_thread_entry
* ����       : emwin_demo�߳�
* ����         : - parameter: �߳���ڲ���
* ���         : None
* ����ֵ    : None
*******************************************************************************/
#ifdef  STemWin
static void emwin_thread_entry(void* parameter)
{    
    WM_SetCreateFlags(WM_CF_MEMDEV);
    GUI_Init();              //STemWin��ʼ��
    
//    GUI_Exec();
    gui_init();
    
    while(1)
    {
        GUI_SelectLayer(1);
        GUI_Delay(100);
    }    
}
#endif  /* STemWin */

/*******************************************************************************
* ������     : emwin_demo_thread_entry
* ����       : emwin_demo�߳�
* ����         : - parameter: �߳���ڲ���
* ���         : None
* ����ֵ    : None
*******************************************************************************/
#ifdef  EmWin_Demo
static void emwin_demo_thread_entry(void* parameter)
{    
    RT_THREAD_DELAY_MS(10);
    while(1)
    {
        #ifdef  EmWin_Demo
        #include "GUIDEMO.h"
        GUIDEMO_Main();    
        #endif  /* EmWin_Demo */
        GUI_Delay(100);
    }    
}
#endif  /* EmWin_Demo */

rt_device_t p_device_finger_in_usart;

rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    static uint8_t rx_buf[128];
    uint32_t rx_length;
    
    rx_length = rt_device_read(p_device_finger_in_usart, 0, rx_buf, size);
    
    for (int i = 0; i < rx_length; i++)
    {
        rt_kprintf("%02X ", rx_buf[i]);
    }
    rt_kprintf("\r\n");
}

void finger ()
{
    uint8_t finger_get[] = {0xF5, 0x23, 0x00, 0x00, 0x00, 0x00, 0x23, 0xF5};
    rt_err_t err;
    rt_serial_t *p_device_serial;
    
    p_device_finger_in_usart = rt_device_find("uart1");
    if (p_device_finger_in_usart != RT_NULL)
    {

        p_device_serial = (rt_serial_t *)p_device_finger_in_usart;
        p_device_serial->config.baud_rate = 115200;
        
        /* ���ûص����������豸*/
        err = rt_device_set_rx_indicate(p_device_finger_in_usart, uart_input);
        rt_kprintf("rt_device_set_rx_indicate %d\r\n", err);
        
        err = rt_device_open(p_device_finger_in_usart, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
        rt_kprintf("rt_device_open %d\r\n", err);
        
        err = rt_device_write(p_device_finger_in_usart, 0, finger_get, sizeof(finger_get));
        rt_kprintf("rt_device_write %d\r\n", err);
    }

}
    
/*******************************************************************************
* ������     : user_init_thread_entry
* ����       : �û���ʼ������
* ����         : - parameter: �߳���ڲ���
* ���         : None
* ����ֵ        : None
*******************************************************************************/
void user_init_thread_entry(void* parameter)
{
    rt_err_t result;
    rt_thread_t p_thread;
    
    /* �����ٽ���*/
    void rt_enter_critical(void); 
        
    /* ����CRCʱ�� */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);
    
    /* ��ʼ�������������� */
    rt_hw_beep_door_init();
    
    /* ��ʼ���豸���� */
    device_config_init(&g_device_config);
    
    /* �����豸��Ϣ */
    rt_memset(&g_device_config, 0, sizeof(device_config_t));
    g_device_config.device_addr = 3;
    strcpy((char *)g_device_config.this_device_name, "EE Bang");
    g_device_config.server_ip[0] = 192;
    g_device_config.server_ip[1] = 168;
    g_device_config.server_ip[2] = 43;
    g_device_config.server_ip[3] = 79;
    g_device_config.server_port = 2756;
    strcpy((char *)g_device_config.router_ssid, "peace");
    strcpy((char *)g_device_config.router_passwd, "zzzzzzzzx");
    g_device_config.router_auth = AUTH_WPA2_PSK;
    strcpy((char *)g_device_config.mesh_ssid, "EE Bang MESH");
    strcpy((char *)g_device_config.mesh_passwd, "zzzzzzzzy");

    device_config_get_set(&g_device_config,SET_DEVICE);
    device_config_print(&g_device_config);

//    finger();
    
    /* ��ʼ���������� */
    if (card_array_init(g_card_id_user_id_list) == -1)
    {
        rt_kprintf("init card array failed\r\n");
    }
    
    /* ��ʼ���������ݿ� */
    att_database_init(&g_att_header);
    
    /* ��ʼ���¼����� */
    rt_event_init(&esp8266_event, "esp8266_event", RT_IPC_FLAG_FIFO);
    rt_event_init(&g_user_check_event, "g_user_check_event", RT_IPC_FLAG_FIFO);
    
    /* ��ʼ��SPI���� */
    rt_hw_stm32_spi_bus_init();
    
#ifdef  TFT
    
    /* ��ʼ��TFTҺ�� */
    TFTLCD_Init();
    POINT_COLOR = WHITE;
    BACK_COLOR = BLACK;
    LCD_ShowString(LCD1,0,0,320,16,16,(uint8_t *)"WiFi Access Control System");
    set_lcd_led(0, 0);
    set_lcd_led(1, 0);
#endif  /* TFT */

#ifdef  TOUCH_SCREEN

    /* ��ʼ�������� */
    TP_Init();
#endif  /* TOUCH_SCREEN */

    /* ��ʼ��ESP8266 */
    init_esp8266();

#ifdef  STemWin

    /* ��ʼ��emwin�߳� */
    result = rt_thread_init(&emwin_thread,
                            "emwin",
                            emwin_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&emwin_stack[0],
                            sizeof(emwin_stack),
                            18,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&emwin_thread);
    }
#endif  /* STemWin */
        
#ifdef  EmWin_Demo
    
    /* ��ʼ��emwin demo�߳� */
    result = rt_thread_init(&emwin_demo_thread,
                            "emwin_demo",
                            emwin_demo_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&emwin_demo_stack[0],
                            sizeof(emwin_demo_stack),
                            18,
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
                            17,
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
    p_thread = rt_thread_create("wifi",
                                wifi_thread_entry, 
                                RT_NULL,
                                64 * 1024, 
                                5, 
                                5);
    if (p_thread != RT_NULL)
        rt_thread_startup(p_thread);

//    result = rt_thread_init(&wifi_thread,
//                            "wifi",
//                            wifi_thread_entry,
//                            RT_NULL,
//                            (rt_uint8_t*)&wifi_stack[0],
//                            sizeof(wifi_stack),
//                            5,
//                            5);
//    if (result == RT_EOK)
//    {
//        rt_thread_startup(&wifi_thread);
//    }
    
    /* ����esp8266��Ϣ��ȡ�߳� */
    gp_esp8266_info_get_tid = rt_thread_create("esp8266_info_get",
                                               esp8266_info_get_entry, 
                                               RT_NULL,
                                               512, 
                                               16, 
                                               5);
    if (gp_esp8266_info_get_tid != RT_NULL) {
        rt_thread_startup(gp_esp8266_info_get_tid);
    }
    
    /* ��ʼ��rtc�߳� */
    result = rt_thread_init(&rtc_thread,
                            "rtc",
                            rtc_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&rtc_stack[0],
                            sizeof(rtc_stack),
                            26,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&rtc_thread);
    }
        
    /* �˳��ٽ���*/
    void rt_exit_critical(void); 
    
    while(1) {
        rt_thread_delay(1000);
    }
}

__asm void SystemReset(void)
{
    MOV R0, #1           //; 
    MSR FAULTMASK, R0    //; ���FAULTMASK ��ֹһ���жϲ���
    LDR R0, =0xE000ED0C  //;
    LDR R1, =0x05FA0004  //; 
    STR R1, [R0]         //; ϵͳ�����λ   

deadloop
    B deadloop        //; ��ѭ��ʹ�������в�������Ĵ���
}
FINSH_FUNCTION_EXPORT(SystemReset, SystemReset)
