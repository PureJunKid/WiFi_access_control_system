#ifndef _RFID_THREAD_H_
#define _RFID_THREAD_H_

#include <rtthread.h>
#include <sys.h>

#define card_in_check 		(1 << 0)	//����ˢ���¼�
#define card_out_check 		(1 << 1)	//����ˢ���¼�
#define finger_in_check 	(1 << 2)	//����ˢָ���¼�
#define finger_out_check 	(1 << 3)	//����ˢָ���¼�

extern rt_uint8_t rc522_stack[ 1024 ];	//�߳�ջ
extern struct rt_thread rc522_thread;	//�߳̿��ƿ�

/* ˢ����ˢָ���¼����ƿ� */
extern struct rt_event user_check_event;
/* ��ȡ���Ŀ��� */
extern u32 read_card_id;

extern void rc522_thread_entry(void* parameter);

#endif
