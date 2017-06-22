/**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: rfid_thread.c
**
** ��   ��   ��: �Ž���
**
** �ļ���������: 2016 �� 09 �� 08 ��
**
** ��        ��: rfid��ɨ���߳�

** ��־:
2016.09.08  �������ļ�
*********************************************************************************************************/
#include <rtthread.h>
#include "rfid_thread.h"
#include "finsh.h"
#include "global.h"
#include "rc522.h"

#ifdef  RC522
rt_uint8_t rc522_stack[ 1024 ];
struct rt_thread rc522_thread;
#endif  /* RC522 */

/* ��ȡ���Ŀ��� */
uint32_t g_read_card_id = 0;

/*******************************************************************************
* ������     : rc522_thread_entry
* ����       : rc522�߳�,��⵽ˢ��֮�󷢳�ˢ���¼�
* ����         : - parameter: �߳���ڲ���
* ���         : None
* ����ֵ    : None
*******************************************************************************/
void rc522_thread_entry(void* parameter)
{
    uint32_t old_in_card_id, old_out_card_id;    //�����ϴζ�ȡ�Ŀ���
    uint32_t in_timer = 0, out_timer = 0;    //��ʱʱ�����������ֹͬһ�ſ���ȡ���
    uint8_t in_timer_en = 0, out_timer_en = 0;    //��ʱʱ�������ʹ��λ 1: ʹ��
    uint8_t is_in_over_time = 0, is_out_over_time = 0;    //�Ƿ񳬹���ֵʱ�� 0: �Ѿ�����
    
    /* ��ʼ��RC522�豸 */
    InitRC522();
    while(1)
    {
        /* �л�Ϊ����RFID�豸������ */
        rt_spi_rc522_device = rt_spi_rc522_in_device;
        if(ReadID(&g_read_card_id) == 0)
        {
            /* �Ƚϵ�ǰ��ȡ�Ŀ��ź��ϴζ�ȡ�Ŀ��ţ���ͬ�����Ϸ���ˢ���¼� */
            if(old_in_card_id != g_read_card_id)
            {
                is_in_over_time = 0;
                old_in_card_id = g_read_card_id;
            }
            /* �ж��Ƿ񾭹���ֵʱ�䣬ͬһ�ſ�ֻ�о�����ֵʱ������ٴ�ˢ�� */
            if(is_in_over_time == 0)
            {
                is_in_over_time = 1;
                in_timer_en = 1;
                rt_kprintf("in card event\r\n");
                /* ���Ͷ����¼� */
                rt_event_send(&g_user_check_event, USER_CHECK_CARD_IN);    
            }
        }
        /* �л�Ϊ����RFID�豸������ */
        rt_spi_rc522_device = rt_spi_rc522_out_device;
        if(ReadID(&g_read_card_id) == 0)
        {
            /* �Ƚϵ�ǰ��ȡ�Ŀ��ź��ϴζ�ȡ�Ŀ��ţ���ͬ�����Ϸ���ˢ���¼� */
            if(old_out_card_id != g_read_card_id)
            {
                is_out_over_time = 0;
                old_out_card_id = g_read_card_id;
            }
            /* �ж��Ƿ񾭹���ֵʱ�䣬ͬһ�ſ�ֻ�о�����ֵʱ������ٴ�ˢ�� */
            if(is_out_over_time == 0)
            {
                is_out_over_time = 1;
                out_timer_en = 1;
                rt_kprintf("out card event\r\n");
                /* ���Ͷ����¼� */
                rt_event_send(&g_user_check_event, USER_CHECK_CARD_OUT);    
            }
        }
        /* ��ʱʱ������� */
        if(in_timer_en)
        {
            in_timer++;
            /* �ﵽ��ֵʱ�䣬������Ӧ��־λ */
            if(in_timer >= 50) 
            {
                is_in_over_time = 0;
                in_timer = 0;
                in_timer_en = 0;
            }
        }
        /* ��ʱʱ������� */        
        if(out_timer_en)
        {
            out_timer++;
            /* �ﵽ��ֵʱ�䣬������Ӧ��־λ */
            if(out_timer >= 50) 
            {
                is_out_over_time = 0;
                out_timer = 0;
                out_timer_en = 0;
            }
        }
        RT_THREAD_DELAY_MS(5);
    }    
}

/*******************************************************************************
* ������     : send_get_card_event
* ����       : ����ˢ���¼�
* ����         : - state: ״̬ 0: ���� 1: ���� - card_id_send: ����
* ���         : None
* ����ֵ    : None
*******************************************************************************/
void send_get_card_event(uint8_t state, uint32_t card_id_send)
{
    switch(state)
    {
        case 0:
        {
            /* ���Ͷ����¼� */
            g_read_card_id = card_id_send;
            rt_event_send(&g_user_check_event, USER_CHECK_CARD_OUT);    
        }    break;
        case 1:
        {
            /* ���Ͷ����¼� */
            g_read_card_id = card_id_send;
            rt_event_send(&g_user_check_event, USER_CHECK_CARD_IN);    
        }    break;
        default: rt_kprintf("״̬���� state is %d , 0: ���� 1: ����",state);
    }
}
FINSH_FUNCTION_EXPORT_ALIAS(send_get_card_event, send_card, send get card event)
