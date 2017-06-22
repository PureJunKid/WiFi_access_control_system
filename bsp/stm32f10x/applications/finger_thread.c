/*******************************************************************************
*                        WiFi Access Control System
*                       ----------------------------
*                                  EE Bang
*
* Contact information:
* web site:    http://www.cqutlab.cn/
* e-mail:      799548861@qq.com
*******************************************************************************/

/**
 * \file
 * \brief ָ�ƴ����߳�
 *
 * \internal
 * \par Modification history
 * - 1.00 17-06-09  zhangjinke, first implementation.
 * \endinternal
 */ 

#include "rfid_thread.h"

#include <rtthread.h>
#include <finsh.h>
#include "global.h"

#ifdef  FINGER_PRINT
rt_uint8_t finger_print_stack[1024];
struct rt_thread finger_print_thread;
#endif  /* FINGER_PRINT */

/** \brief ָ��ģ��ʶ�𵽵��û��� */
uint32_t g_finger_user_id = 0;

/**
 * \brief ָ�ƴ����߳�,��⵽ָ��֮�󷢳�ָ���¼�
 *
 * \param[in] p_parameter �߳���ڲ���
 *
 * \return ��
 */
void finger_print_thread_entry (void *p_parameter)
{
    
    while(1) {
        RT_THREAD_DELAY_MS(5);
    }    
}

/**
 * \brief ����ָ���¼�
 *
 * \param[in] state        ״̬ 0: ���� 1: ����
 * \param[in] card_id_send ����
 *
 * \return ��
 */
void finger_print_event_send (uint8_t state, uint32_t user_id)
{
    g_finger_user_id = user_id;
    
    switch(state) {
        
        case 0:
            
            /* ���Ͷ����¼� */
            rt_event_send(&g_user_check_event, USER_CHECK_FINGER_OUT);    
            break;
        
        case 1:
            
            /* ���Ͷ����¼� */
            rt_event_send(&g_user_check_event, USER_CHECK_FINGER_IN);    
            break;
        
        default: 
            rt_kprintf("״̬���� state is %d , 0: ���� 1: ����",state);
    }
}
FINSH_FUNCTION_EXPORT_ALIAS(finger_print_event_send, send_finger, send finger event)
