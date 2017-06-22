#ifndef _WIFI_THREAD_H_
#define _WIFI_THREAD_H_

#include <rtthread.h>
#include <stdint.h>

__packed struct wifi_pack
{
    uint32_t  crc;   /**< \brief crcУ�� */
    uint8_t   cmd;   /**< \brief ���� */
    uint16_t  lenth; /**< \brief ���ݳ��� */
    uint8_t  *data;  /**< \brief ���� */
};

extern rt_uint8_t wifi_stack[ 1024 ];    //�߳�ջ
extern struct rt_thread wifi_thread;     //�߳̿��ƿ�
extern rt_thread_t gp_esp8266_info_get_tid; /**< \brief ָ���߳̿��ƿ��ָ�� */

extern void wifi_thread_entry(void* parameter);

extern struct wifi_pack wifi_pack_recv;
extern uint8_t is_recv_wifi_pack;

extern uint8_t station_addr[6];
extern uint8_t softap_addr[6];

int8_t wifi_send(uint8_t cmd, uint16_t data_lenth, uint8_t *data);

/*
 * \brief ��ȡesp8266�����Ϣ
 *
 * \param[in] p_parameter �߳���ڲ���
 *
 * \return ��
 */
void esp8266_info_get_entry (void *p_parameter);

#endif
