#ifndef _WIFI_THREAD_H_
#define _WIFI_THREAD_H_

#include <rtthread.h>
#include <sys.h>

extern rt_uint8_t wifi_stack[ 1024 ];	//�߳�ջ
extern struct rt_thread wifi_thread; 	//�߳̿��ƿ�
extern void wifi_thread_entry(void* parameter);

#endif
