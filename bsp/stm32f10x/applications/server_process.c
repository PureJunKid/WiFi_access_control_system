/*******************************************************************************
*                        WiFi Access Control System
*                       ----------------------------
*                                  EE BANG
*
* Contact information:
* web site:    http://www.cqutlab.cn/
* e-mail:      799548861@qq.com
*******************************************************************************/

/**
 * \file
 * \brief ������������͵�����
 *
 * \internal
 * \par Modification history
 * - 1.00 17-04-23  zhangjinke, first implementation.
 * \endinternal
 */ 

#include "server_process.h"

#include <string.h>
#include "ds1307.h"
#include "wifi_thread.h"
#include "esp8266_cmd.h"
#include "stm32_crc.h"
#include "p_database.h"

/**
 * \brief ������������͵�����
 *
 * \param[in] cmd    : ����
 * \param[in] p_data : �����׵�ַ
 * \param[in] lenth  : ���ݳ���
 *
 * \return ��
 */
void data_process(uint8_t cmd, uint8_t *p_data, uint16_t lenth)
{
	uint32_t crc = 0;
	static struct user_info one_user_info;
	
	static uint8_t buf[128];
	
	switch (cmd) {
	
	case CMD_SERVER_USER_ADD:       /* ����û� */
		
	    /* ���Ŀ���ַ */
		memset(buf, 0, 6);
	
	    /* ��ӱ�����ַ */
		memcpy(buf + 6, station_addr, 0);
	
	    /* crcУ�� */
		crc = block_crc_calc(p_data - 1, 1 + sizeof(struct user_info) - 4);
		if (*((uint32_t *)&p_data[1 + sizeof(struct user_info) - 4]) != crc) {
			
			/* crcУ��ʧ�ܣ�����NACK */
			buf[12] = cmd;
			buf[13] = CMD_SERVER_NACK;
			wifi_send(CMD_SEND_MESH_DATA, 6 + 8, buf);
		}
		
	if (0 != add_del_get_one_user (&one_user_info, GET_ONE_USER))
		{
		
		}

		break;
	
	case CMD_SERVER_TIMESYNC:       /* ͬ��ʱ�� */
		
		/* ���Ŀ���ַ */
		memset(buf, 0, 6);
	
	    /* ��ӱ�����ַ */
		memcpy(buf + 6, station_addr, 0);
	
	    /* crcУ�� */
		crc = block_crc_calc(p_data - 1, 9);
		if (*((uint32_t *)&p_data[8]) != crc) {
			
			/* crcУ��ʧ�ܣ�����NACK */
			buf[12] = cmd;
			buf[13] = CMD_SERVER_NACK;
			wifi_send(CMD_SEND_MESH_DATA, 6 + 8, buf);
		}
	
		TimeValue.year = (p_data[0] | (p_data[1] << 8)) - 2000;
		TimeValue.month = p_data[2];
		TimeValue.date = p_data[3];
		TimeValue.hour = p_data[4];
		TimeValue.minute = p_data[5];
		TimeValue.second = p_data[6];
		TimeValue.week = p_data[7];
		DS1307_Time_Init(&TimeValue);

		buf[12] = cmd;
		buf[13] = CMD_SERVER_ACK;
		
		/* ʱ��ͬ���ɹ�������ACK */
	    wifi_send(CMD_SEND_MESH_DATA, 6 + 8, buf);
		break;
	
	default:
		break;
	
	}
}

/* end of file */
