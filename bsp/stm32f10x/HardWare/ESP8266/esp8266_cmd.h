#ifndef _ESP8266_CMD_H_
#define _ESP8266_CMD_H_

#include "sys.h"

#define CMD_RETURN_RECV                   (0)	 /* ֱ�ӷ��ؽ��յ������� */
#define CMD_GET_SDK_VERSION               (1)    /* ��ȡsdk�汾�� */
#define CMD_GET_FLASH_SIZE_MAP            (2)    /* ��ѯFlash size�Լ�Flash map */
#define CMD_WIFI_GET_IP_INFO              (3)    /* ��ѯIP��ַ */
#define CMD_WIFI_GET_MACADDR              (4)    /* ��ѯmac��ַ */
#define CMD_GET_DEVICE_LIST               (5)    /* ��ȡmesh�豸�б� */
#define CMD_SEND_MESH_DATA                (6)    /* ��mesh�����з������� */

#define CMD_SEND_DATA_TO_MCU              (254)  /* �������ݵ�mcu */


s8 wifi_get_macaddr(u8 station_mac[], u8 ap_mac[]);

#endif
