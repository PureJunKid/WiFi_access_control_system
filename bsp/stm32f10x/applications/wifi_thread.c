/**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: wifi_thread.c
**
** ��   ��   ��: �Ž���
**
** �ļ���������: 2016 �� 09 �� 11 ��
**
** ��        ��: ͨ������߳�

** ��־:
2016.09.11  �������ļ�
*********************************************************************************************************/
#include <rtthread.h>
#include "finsh.h"
#include "wifi_thread.h"
#include "esp8266.h"
#include "stm32_crc.h"
#include "esp8266_cmd.h"

/* ��ȡ�ṹ���Աƫ�ƺ궨�� */
#define OFFSET(Type, member) ( (u32)&(((struct Type*)0)->member) )
#define MEMBER_SIZE(Type, member) sizeof(((struct Type*)0)->member)

#define rt_thread_delayMs(x) rt_thread_delay(rt_tick_from_millisecond(x))

rt_uint8_t wifi_stack[ 1024 ];	//�߳�ջ
struct rt_thread wifi_thread; 	//�߳̿��ƿ�
struct wifi_pack wifi_pack_recv;
u8 is_recv_wifi_pack = 0;

/*******************************************************************************
* ������ 	: wifi_thread_entry
* ����   	: ͨ������߳�
* ����     	: - parameter: �߳���ڲ���
* ���     	: None
* ����ֵ    : None
*******************************************************************************/
void wifi_thread_entry(void* parameter)
{
	rt_err_t status = RT_EOK;     /* ���պ�������ֵ */
	rt_uint32_t recved_event = 0; /* �յ����¼� */
	static u32 par_lenth = sizeof(struct wifi_pack) - MEMBER_SIZE(wifi_pack, data); /* ���еĲ�����С */
	static u32 crc_lenth = MEMBER_SIZE(wifi_pack, crc); /* CRC��С */
	u32 i = 0;
	
	/* ��ʼ��ESP8266 */
	init_esp8266();
	while(1)
	{
		/* �ȴ�ˢ����ˢָ���¼� */
		status = rt_event_recv(	&esp8266_event,                       /* �¼�����ľ�� */
								hspi_rx,                              /* �����̸߳���Ȥ���¼� */
								RT_EVENT_FLAG_CLEAR|RT_EVENT_FLAG_OR, /* �߼�������¼� */
								RT_WAITING_FOREVER,                   /* ������ʱ */
								&recved_event                         /* ָ���յ����¼� */
							  );
		/* ���ݽ��յ����¼�ִ�в�ͬ�Ĵ��� */
		if(status == RT_EOK)
		{
			/* hspi�����¼� */
			if (recved_event & hspi_rx)
			{
				/* hspi������� */
				if (esp8266_spi_read() == 0)
				{
					rt_memcpy(&wifi_pack_recv, recv_pack, par_lenth);
					/* У������� */
					if (wifi_pack_recv.lenth + par_lenth != recv_lenth)
					{
						is_recv_pack = 0; /* ���µȴ��������� */
						rt_kprintf("lenth verify failed\r\n");
						continue;
					}
					/* CRCУ�� */
					if (wifi_pack_recv.crc != CalcBlockCRC(recv_pack + crc_lenth, par_lenth - crc_lenth + wifi_pack_recv.lenth))
					{
						is_recv_pack = 0; /* ���µȴ��������� */
						rt_kprintf("crc verify failed\r\n");
						continue;
					}
					
					wifi_pack_recv.data = recv_pack + par_lenth;
					is_recv_wifi_pack = 1;
					
//					rt_kprintf("cmd: %d, lenth: %d, crc: %08X\r\n", wifi_pack_recv.cmd, wifi_pack_recv.lenth, wifi_pack_recv.crc);
//					
//					for (i = 0; i < wifi_pack_recv.lenth; i++)
//					{
//						rt_kprintf("%02X ", *(wifi_pack_recv.data + i));				
//					}
//					rt_kprintf("\r\n");	
					
					if (wifi_pack_recv.cmd == cmd_send_data_to_mcu)
					{
						struct mesh_header_format *header = NULL;
						header = (struct mesh_header_format *)wifi_pack_recv.data;
						if (header->oe == 0)
						{
							rt_kprintf("len: %d, data_len: %d\r\n", header->len, header->len - ESP_MESH_HEAD_SIZE);
//							for (i = 0; i<header->len - ESP_MESH_HEAD_SIZE; i++)
//							{
//								rt_kprintf("%02X ", header->user_data[i]);
//							}
//							rt_kprintf("\r\n");
						}
					}
					
					is_recv_pack = 0; /* ���µȴ��������� */
				}
			}
		}
	}	
}

s8 wifi_send(u8 cmd, u16 data_lenth, u8 *data)
{
	struct wifi_pack wifi_pack_send;
	u8 *send_pack = NULL;
	u32 par_lenth = sizeof(struct wifi_pack) - MEMBER_SIZE(wifi_pack, data); /* ���еĲ�����С */
	u32 crc_lenth = MEMBER_SIZE(wifi_pack, crc); /* CRC��С */
	
	/* �����ڴ� */
	send_pack = (u8 *)rt_malloc(data_lenth + par_lenth);
    if (!send_pack) 
    { 
        rt_kprintf("send_pack memory failed\r\n");

        return -RT_ENOMEM;
    }

	wifi_pack_send.cmd = cmd;                /* ���� */
	wifi_pack_send.lenth = data_lenth;       /* ���ݳ��� */
	/* ����CRC֮����������������������� */
	rt_memcpy(send_pack + crc_lenth, (u8 *)&wifi_pack_send + crc_lenth, par_lenth - crc_lenth);
	/* �����ݿ����������� */
	rt_memcpy(send_pack + par_lenth, data, data_lenth);
	/* ����CRC */
	wifi_pack_send.crc = CalcBlockCRC(send_pack + crc_lenth, par_lenth - crc_lenth + data_lenth);
	/* ��CRC������������ */
	rt_memcpy(send_pack, &wifi_pack_send, crc_lenth);
	/* �������� */
	if (esp8266_spi_write(send_pack, par_lenth + data_lenth) != 0)
	{
		rt_free(send_pack);
		return -1;
	}
	
	rt_free(send_pack);
	return 0;
}

s8 sendTest(u8 cmd, u32 lenth)
{
	u8 buf[1024*5];
	int i;
	
	for (i = 0; i < lenth; i++)
	{
		buf[i] = (u8)i;
	}
	
	return wifi_send(cmd, lenth, buf);
}
FINSH_FUNCTION_EXPORT(sendTest, sendTest)
