#ifndef _WIFI_THREAD_H_
#define _WIFI_THREAD_H_

#include <rtthread.h>
#include <sys.h>

__packed struct wifi_pack
{
	u32  crc;				/* crcУ�� */
	u8   cmd;				/* ���� */
	u16  lenth;			    /* ���ݳ��� */
	u8  *data;				/* ���� */
};

extern rt_uint8_t wifi_stack[ 1024 ];	//�߳�ջ
extern struct rt_thread wifi_thread; 	//�߳̿��ƿ�
extern rt_thread_t gp_esp8266_info_get_tid; /**< \brief ָ���߳̿��ƿ��ָ�� */

extern void wifi_thread_entry(void* parameter);

extern struct wifi_pack wifi_pack_recv;
extern u8 is_recv_wifi_pack;

extern uint8_t station_addr[6];
extern uint8_t softap_addr[6];

s8 wifi_send(u8 cmd, u16 data_lenth, u8 *data);

/*
 * \brief ��ȡesp8266�����Ϣ
 *
 * \param[in] p_parameter �߳���ڲ���
 *
 * \return ��
 */
void esp8266_info_get_entry (void *p_parameter);

#endif
