#ifndef _RFID_THREAD_H_
#define _RFID_THREAD_H_

#include <rtthread.h>
#include <stdint.h>

#define USER_CHECK_CARD_IN         (1 << 0)    //����ˢ���¼�
#define USER_CHECK_CARD_OUT         (1 << 1)   //����ˢ���¼�

extern rt_uint8_t rc522_stack[ 1024 ];    //�߳�ջ
extern struct rt_thread rc522_thread;     //�߳̿��ƿ�

/* ��ȡ���Ŀ��� */
extern uint32_t g_read_card_id;

extern void rc522_thread_entry(void* parameter);

#endif
