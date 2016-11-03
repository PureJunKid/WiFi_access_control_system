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
#include "user_check_thread.h"
#include "rfid_thread.h"
#include "p_database.h"
#include "device_config.h"
/* �ļ�ϵͳ���ͷ�ļ� */
#include <dfs.h>
#include <dfs_posix.h>
/* ���������������ͷ�ļ� */
#include "beep_door.h"

/* ��ʱ���� */
#define rt_thread_delayMs(x) rt_thread_delay(rt_tick_from_millisecond(x))

rt_uint8_t user_check_stack[ 1024 ];	//�߳�ջ
struct rt_thread user_check_thread; 	//�߳̿��ƿ�

/* ����һ����Ա��Ϣ�ṹ�� */
struct user_info user_info_struct_get;
u16 user_num;

/*******************************************************************************
* ������ 	: user_check_thread_entry
* ����   	: ���յ�ˢ����ˢָ���¼�֮�����Ȩ�޼�⣬ͨ������
* ����     	: - parameter: �߳���ڲ���
* ���     	: None
* ����ֵ    : None
*******************************************************************************/
void user_check_thread_entry(void* parameter)
{
	rt_err_t status = RT_EOK;
	rt_uint32_t recved_event = 0;
	u16 user_num = 0, max_user_num = 0;
	s32 search_result = 0;
	u32 i;
		
	if(get_set_user_num(&user_num,GET_USER) == -1)
	{
		rt_kprintf("get user num failed\r\n");
	}
	
	if(get_set_user_num_max(&max_user_num,GET_USER) == -1)
	{
		rt_kprintf("get user num max failed\r\n");
	}
	while(1)
	{
		/* �ȴ�ˢ����ˢָ���¼� */
		status = rt_event_recv(	&user_check_event,						//�¼�����ľ��
								card_in_check|card_out_check,			//�����̸߳���Ȥ���¼�
								RT_EVENT_FLAG_CLEAR|RT_EVENT_FLAG_OR,	//�߼�������¼�
								RT_WAITING_FOREVER,						//������ʱ
								&recved_event							//ָ���յ����¼�
							  );
		/* ���ݽ��յ����¼�ִ�в�ͬ�Ĵ��� */
		if(status == RT_EOK)
		{
			/* ˢ���¼� */
			if ((recved_event&card_in_check) || (recved_event&card_out_check))
			{
				/* �������Ŷ�Ӧ���û��� */
				search_result = bin_search(card_id_array, user_num, read_card_id);
				if(search_result == -1)
				{
					if(recved_event&card_in_check) { rt_kprintf("in card id %X is not exist\r\n", read_card_id); }
					else { rt_kprintf("out card id %X is not exist\r\n", read_card_id); }
				}
				else	/* ��������Ӧ���û��� */
				{
					/* ��ȡ��ϸ�û���Ϣ */
					user_info_struct_get.user_id = card_id_array[search_result].user_id;
					add_del_get_one_user(&user_info_struct_get, GET_ONE_USER);
					/* �ж�card id�Ƿ���ȷ */
					if (user_info_struct_get.card_id != card_id_array[search_result].card_id) { continue; }
					/* �ж��Ƿ��ǳ�������Ա */
					if ((user_info_struct_get.authority[15]&(1 << 7)) == (1 << 7) )
					{//���λΪ1��Ϊ��������Ա��ֱ�ӿ���
						rt_kprintf("\r\n��������Ա%s����ӭ���٣�\r\n", user_info_struct_get.name);
						/* ���� */
						open_door();
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
						/* ������ڣ���ʾ */
						if(0)
						{
							rt_kprintf("\r\n���ڣ�\r\n");
							continue;					
						}
					}
					/* ȷ��Ȩ�� */
					if ((user_info_struct_get.authority[device_addr/8]&(1 << (device_addr%8))) != (1 << (device_addr%8)) )
					{
						rt_kprintf("\r\nȨ�޲��㣡\r\n");
						continue;
					}
					/* ������״̬ */
					switch(recved_event)
					{
						/* ����ˢ�� */
						case card_in_check:
						{
							if ((user_info_struct_get.state[device_addr/8]&(1 << (device_addr%8))) != (1 << (device_addr%8)) )
							{	//0:���� 1:����    ���������
								rt_kprintf("\r\n����δˢ����\r\n");
								continue;
							}
							else
							{
								/* ��״̬����Ϊ���� */
								user_info_struct_get.state[device_addr/8] &=~ (1 << (device_addr%8));
								get_set_state(user_info_struct_get.state,user_info_struct_get.user_id,SET_USER);
								/* ���� */
								open_door();
							}
						}	break;
						/* ����ˢ�� */
						case card_out_check:
						{
							if ((user_info_struct_get.state[device_addr/8]&(1 << (device_addr%8))) == (1 << (device_addr%8)) )
							{	//0:���� 1:����    ���������
								rt_kprintf("\r\n����δˢ����\r\n");
								continue;
							}
							else
							{
								/* ��״̬����Ϊ���� */
								user_info_struct_get.state[device_addr/8] |= (1 << (device_addr%8));
								get_set_state(user_info_struct_get.state,user_info_struct_get.user_id,SET_USER);
								/* ���� */
								open_door();
							}
						}
						default : break;
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
	}	
}

