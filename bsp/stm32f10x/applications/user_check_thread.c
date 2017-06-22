/**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: user_check_thread.c
**
** ��   ��   ��: �Ž���
**
** �ļ���������: 2016 �� 09 �� 08 ��
**
** ��        ��: ���յ�ˢ����ˢָ���¼�֮�����Ȩ�޼�⣬ͨ������

** ��־:
2016.09.08  �������ļ�
*********************************************************************************************************/
#include <rtthread.h>
#include <time.h>
#include "user_check_thread.h"
#include "wifi_thread.h"
#include "rfid_thread.h"
#include "p_database.h"
#include "device_config.h"
#include <dfs.h>
#include <dfs_posix.h>
#include "beep_door.h"
#include "att_database.h"
#include "rtc_thread.h"
#include "global.h"
#include "stm32_crc.h"

/** \brief ˢ����ˢָ���¼����ƿ� */
struct rt_event g_user_check_event;

rt_uint8_t user_check_stack[ 1024 ];    //�߳�ջ
struct rt_thread user_check_thread;     //�߳̿��ƿ�


/*******************************************************************************
* ������     : add_one_att_record
* ����       : ���һ�����ڼ�¼
* ����         : - user_info_temp: ��Ա��Ϣ�ṹ�� -recved_event: �¼�(ˢ����ˣָ��)
* ���         : None
* ����ֵ    : None
*******************************************************************************/
int8_t add_one_att_record(user_info_t *user_info_temp, uint32_t recved_event)
{
    att_info_t att_info_temp;
    att_header_t att_header_temp;

    /* ���ÿ�����Ϣ */
    rt_memset(&att_info_temp, 0 ,sizeof(att_info_t));
    att_info_temp.user_id = user_info_temp->user_id;
    rt_memcpy(&att_info_temp.student_id, &user_info_temp->student_id, MEMBER_SIZE(att_info, student_id));
    rt_memcpy(&att_info_temp.name, &user_info_temp->name, MEMBER_SIZE(att_info, name));
    att_info_temp.device_addr = g_device_config.device_addr;
    rt_memcpy(att_info_temp.mac_addr, station_addr, sizeof(station_addr));
    if ((recved_event & USER_CHECK_CARD_IN) || (recved_event & USER_CHECK_FINGER_IN))
    {
        att_info_temp.state = 0; /* ����״̬Ϊ���� */
    }
    else
    {
        att_info_temp.state = 1; /* ����״̬Ϊ���� */
    }
    att_info_temp.year    = TimeValue.year + 2000;
    att_info_temp.month   = TimeValue.month;
    att_info_temp.day     = TimeValue.date;
    att_info_temp.hour    = TimeValue.hour;
    att_info_temp.minutes = TimeValue.minute;
    att_info_temp.second  = TimeValue.second;
    
    /* ���濼����Ϣ */
    if (att_record_add(&att_info_temp) != 0){
        rt_kprintf("att_record_get_set failed\r\n");
    }
    
    return 0;
}

/*******************************************************************************
* ������     : user_check_thread_entry
* ����       : ���յ�ˢ����ˢָ���¼�֮�����Ȩ�޼�⣬ͨ������
* ����         : - parameter: �߳���ڲ���
* ���         : None
* ����ֵ    : None
*******************************************************************************/
void user_check_thread_entry(void* parameter)
{
    rt_err_t status = RT_EOK;
    rt_uint32_t recved_event = 0;
//    uint16_t user_num = 0, max_user_num = 0;
    int32_t search_result = 0;
    uint32_t i = 0;
    uint32_t crc = 0;
    char limit_time[20], now_time[20];
    card_id_user_id_link_t card_id_user_id_key;
    card_id_user_id_link_t *p_card_id_user_id_recv;
    static user_info_t user_info_struct_get; /* ����һ����Ա��Ϣ�ṹ�� */
    
//    if(get_set_user_num(&user_num, GET_USER) == -1)
//    {
//        rt_kprintf("get user num failed\r\n");
//    }
//    
//    if(get_set_user_num_max(&max_user_num, GET_USER) == -1)
//    {
//        rt_kprintf("get user num max failed\r\n");
//    }
    while(1)
    {
        /* ��ջ�ȡ������Ա��Ϣ */
        rt_memset(&user_info_struct_get, 0, sizeof(user_info_t));
        
        /* �ȴ�ˢ����ˢָ���¼� */
        status = rt_event_recv(&g_user_check_event,                       //�¼�����ľ��
                               USER_CHECK_CARD_IN |                       //�����̸߳���Ȥ���¼�
                               USER_CHECK_CARD_OUT |
                               USER_CHECK_FINGER_IN |
                               USER_CHECK_FINGER_OUT,            
                               RT_EVENT_FLAG_CLEAR|RT_EVENT_FLAG_OR,    //�߼�������¼�
                               RT_WAITING_FOREVER,                      //������ʱ
                               &recved_event                            //ָ���յ����¼�
                              );
        
        /* ���ݽ��յ����¼�ִ�в�ͬ�Ĵ��� */
        if(status == RT_EOK) {
            
            if ((recved_event & USER_CHECK_CARD_IN) || (recved_event & USER_CHECK_CARD_OUT)) { /* ˢ���¼� */
                
                card_id_user_id_key.card_id = g_read_card_id;
                
                /* �������Ŷ�Ӧ���û��� */
                p_card_id_user_id_recv = bsearch(&card_id_user_id_key,
                                                  g_card_id_user_id_list, 
                                                  MAX_USER_NUM, 
                                                  sizeof(card_id_user_id_link_t), 
                                                  card_compar);

                if (p_card_id_user_id_recv == RT_NULL) {
                    if(recved_event & USER_CHECK_CARD_IN) { 
                        rt_kprintf("in card id %X is not exist\r\n", g_read_card_id); 
                    } else { 
                        rt_kprintf("out card id %X is not exist\r\n", g_read_card_id); 
                    }
                    continue;
                } else {    /* ��������Ӧ���û��� */
                    user_info_struct_get.user_id = p_card_id_user_id_recv->user_id;
                    
                    /* ��ȡ��ϸ�û���Ϣ */
                    if (add_del_get_one_user(&user_info_struct_get, GET_ONE_USER) != 0) {
                        rt_kprintf("��ȡ��ϸ�û���Ϣʧ�� user id:%d\r\n", g_card_id_user_id_list[search_result].user_id);
                    }
                    
                    /* crcУ�� */
                    crc = block_crc_calc((uint8_t *)&user_info_struct_get, sizeof(user_info_t) - 4 - 16);
                    if (crc != user_info_struct_get.crc) {
                        rt_kprintf("�û���ϢcrcУ�����\r\n");
                        continue;
                    }
                    
                    /* �ж�card id�Ƿ���ȷ */
                    if (user_info_struct_get.card_id != p_card_id_user_id_recv->card_id) { 
                        continue; 
                    }
                }
            } else if ((recved_event & USER_CHECK_FINGER_IN) || (recved_event & USER_CHECK_FINGER_OUT)) { /* ָ���¼� */
            
            } else {
                rt_kprintf("unknown user check event 0x%08X\r\n", recved_event);
                continue;
            }
            
            /* �ж��Ƿ��ǳ�������Ա */
            if ((user_info_struct_get.authority[15]&(1 << 7)) == (1 << 7) ) { //���λΪ1��Ϊ��������Ա��ֱ�ӿ���
                rt_kprintf("\r\n��������Ա����ӭ���� uID:%d\r\n", user_info_struct_get.user_id);
                
                /* ���� */
                open_door();
                
                /* ��¼������Ϣ */
                add_one_att_record(&user_info_struct_get, recved_event);
                continue;
            }
            
            /* �ж��û��Ƿ񼤻� */
            if (user_info_struct_get.effective != 1)
            {
                rt_kprintf("\r\nδ���\r\n");
                continue;
            }
            
            /* �ж��Ƿ�������ʱ���û� */
            if (user_info_struct_get.is_time_limit == 1)
            {
                sprintf(now_time, 
                        "%04d-%02d-%02d %02d:%02d:%02d", 
                        TimeValue.year + 2000,
                        TimeValue.month,
                        TimeValue.date,
                        TimeValue.hour,
                        TimeValue.minute,
                        TimeValue.second);
                sprintf(limit_time, 
                        "%04d-%02d-%02d %02d:%02d:%02d", 
                        user_info_struct_get.year,
                        user_info_struct_get.month,
                        user_info_struct_get.day,
                        user_info_struct_get.hour,
                        user_info_struct_get.minutes,
                        user_info_struct_get.second);
                
                /* ������ڣ���ʾ */
                if (strcmp(now_time, limit_time) >= 0) {
                    rt_kprintf("\r\n���ڣ�\r\n");
                    continue;                    
                }
            }
            
            /* ȷ��Ȩ�� */
            if (user_info_struct_get.authority[15] & (1 << 6)) {
            } else if ((user_info_struct_get.authority[g_device_config.device_addr/8]&(1 << (g_device_config.device_addr%8))) != (1 << (g_device_config.device_addr%8)))
            {
                rt_kprintf("\r\nȨ�޲��㣡\r\n");
                continue;
            }
            
            /* ������״̬ */
            switch(recved_event) {
                
                /* ����ˢ�� */
                case USER_CHECK_CARD_IN:
                    if ((user_info_struct_get.state[g_device_config.device_addr / 8] & (1 << (g_device_config.device_addr % 8))) != (1 << (g_device_config.device_addr%8)) )
                    {    //0:���� 1:����    ���������
                        rt_kprintf("\r\n����δˢ����\r\n");
                        continue;
                    }
                    else
                    {
                        /* ��״̬����Ϊ���� */
                        user_info_struct_get.state[g_device_config.device_addr/8] &=~ (1 << (g_device_config.device_addr%8));
                        get_set_state(user_info_struct_get.state,user_info_struct_get.user_id,SET_USER);
                        /* ���� */
                        open_door();
                    }
                    break;
                    
                /* ����ˢ�� */
                case USER_CHECK_CARD_OUT:
                    if ((user_info_struct_get.state[g_device_config.device_addr/8]&(1 << (g_device_config.device_addr%8))) == (1 << (g_device_config.device_addr%8)) )
                    {    //0:���� 1:����    ���������
                        rt_kprintf("\r\n����δˢ����\r\n");
                        continue;
                    }
                    else
                    {
                        /* ��״̬����Ϊ���� */
                        user_info_struct_get.state[g_device_config.device_addr/8] |= (1 << (g_device_config.device_addr%8));
                        get_set_state(user_info_struct_get.state,user_info_struct_get.user_id,SET_USER);
                        /* ���� */
                        open_door();
                    }
                    break;
                    
                /* ����ˢָ�� */
                case USER_CHECK_FINGER_IN:
                    break;
                
                /* ����ˢָ�� */
                case USER_CHECK_FINGER_OUT:
                    break;
                
                default : 
                    ; /* VOID */
            }
            
            /* ��¼������Ϣ */
            add_one_att_record(&user_info_struct_get, recved_event);
            
            if (user_info_struct_get.authority[15] & (1 << 6)) {
                rt_kprintf("\r\n����Ա����ӭ���� uID:%d", user_info_struct_get.user_id);
            }
            
            /* ��ӡ�����Ϣ */
            rt_kprintf("\r\nuser id is %d\r\n", user_info_struct_get.user_id);
            rt_kprintf("card id is %X\r\n", user_info_struct_get.card_id);
            rt_kprintf("effective is %d\r\n", user_info_struct_get.effective);
            rt_kprintf("name is %s\r\n", user_info_struct_get.name);
            rt_kprintf("student id is %s\r\n", user_info_struct_get.student_id);
            
            /* ��ӡȨ�� */
            rt_kprintf("authority is \r\n"); 
            for (i = 0; i<sizeof(user_info_struct_get.authority); i++)
            {
                rt_kprintf("%02X ", user_info_struct_get.authority[i]);
            }
            rt_kprintf("\r\n"); 
        }
    }    
}

