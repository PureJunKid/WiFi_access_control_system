#ifndef _ESP8266_CMD_H_
#define _ESP8266_CMD_H_

#include "sys.h"

#define cmd_return_recv                   (0)	 /* ֱ�ӷ��ؽ��յ������� */
#define cmd_get_sdk_version               (1)    /* ��ȡsdk�汾�� */
#define cmd_get_flash_size_map            (2)    /* ��ѯFlash size�Լ�Flash map */
#define cmd_wifi_get_ip_info              (3)    /* ��ѯIP��ַ */
#define cmd_wifi_get_macaddr              (4)    /* ��ѯmac��ַ */
#define cmd_get_device_list               (5)    /* ��ȡmesh�豸�б� */

#define cmd_send_data_to_mcu              (254)  /* �������ݵ�mcu */

#endif
