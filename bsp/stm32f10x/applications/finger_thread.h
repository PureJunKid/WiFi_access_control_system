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

#ifndef __FINGER_THREAD_H
#define __FINGER_THREAD_H

#include <rtthread.h>

extern rt_uint8_t rc522_stack[ 1024 ];    //�߳�ջ
extern struct rt_thread rc522_thread;    //�߳̿��ƿ�

/* ˢ����ˢָ���¼����ƿ� */
extern struct rt_event user_check_event;
/* ��ȡ���Ŀ��� */
extern uint32_t read_card_id;

extern void rc522_thread_entry(void* parameter);

#endif
