#ifndef _WIFI_THREAD_H_
#define _WIFI_THREAD_H_

#include <rtthread.h>
#include <sys.h>

__packed struct wifi_pack
{
	u32  crc;				/* crcУ�� */
	u8   dst[6];			/* Ŀ���ַ */
	u8   src[6];			/* Դ��ַ */
	u8   cmd;				/* ���� */
	u16  lenth;			    /* ���� */
	u8  *data;				/* ���� */
};

extern rt_uint8_t wifi_stack[ 1024 ];	//�߳�ջ
extern struct rt_thread wifi_thread; 	//�߳̿��ƿ�
extern void wifi_thread_entry(void* parameter);

#endif
