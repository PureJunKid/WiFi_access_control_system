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
 * \brief esp8266ָ��
 *
 * \internal
 * \par Modification history
 * - 1.00 17-01-31 zhangjinke, first implementation.
 * \endinternal
 */ 
#ifndef __ESP8266_CMD_H
#define __ESP8266_CMD_H

#include "esp8266.h"

typedef enum auth_mode {
    AUTH_OPEN = 0,
    AUTH_WEP,
    AUTH_WPA_PSK,
    AUTH_WPA2_PSK,
    AUTH_WPA_WPA2_PSK,
    AUTH_MAX
} auth_mode_t;

#define CMD_RETURN_RECV                   (0)    /* ֱ�ӷ��ؽ��յ������� */
#define CMD_GET_SDK_VERSION               (1)    /* ��ȡsdk�汾�� */
#define CMD_GET_FLASH_SIZE_MAP            (2)    /* ��ѯFlash size�Լ�Flash map */
#define CMD_WIFI_GET_IP_INFO              (3)    /* ��ѯIP��ַ */
#define CMD_WIFI_GET_MACADDR              (4)    /* ��ѯmac��ַ */
#define CMD_GET_DEVICE_LIST               (5)    /* ��ȡmesh�豸�б� */
#define CMD_SEND_MESH_DATA                (6)    /* ��mesh�����з������� */
#define CMD_SERVER_ADDR_SET               (7)    /* ���÷�����IP��˿� */
#define CMD_MESH_GROUP_ID_SET             (8)    /* ����MESH��ID */
#define CMD_ROUTER_SET                    (9)    /* ����·������Ϣ */
#define CMD_MESH_WIFI_SET                 (10)   /* ����MESH������Ϣ */
#define CMD_MESH_INIT                     (11)   /* ��ʼ��MESH */

#define CMD_SEND_DATA_TO_MCU              (254)  /* �������ݵ�mcu */



/**
 * \brief �ȴ�esp8266Ӧ��
 *
 * \param[in] cmd ����
 *
 * \retval  0 ���յ�Ӧ��
 * \retval -1 ��ʱ
 * \retval -2 ��������յ�cmd��ƥ��
 * \retval -3 ���յ�����
 */
int8_t wait_ack (uint8_t cmd);

/**
 * \brief ����hspiͨ��
 *
 * \param ��
 *
 * \retval  0 ���յ�Ӧ��
 * \retval -1 ��ʱ
 * \retval -2 ��������յ�cmd��ƥ��
 * \retval -3 ���յ�����
 */
int8_t test_hspi (void);

/**
 * \brief ��ȡsdk�汾��
 *
 * \param[out] p_ver   sdk�汾��
 * \param[out] p_lenth sdk�汾���ַ�������
 *
 * \retval  0 ���յ�Ӧ��
 * \retval -1 ��ʱ
 * \retval -2 ��������յ�cmd��ƥ��
 * \retval -3 ���յ�����
 */
int8_t sdk_version_get (uint8_t *p_ver, uint16_t *p_lenth);

/**
 * \brief ��ѯFlash size�Լ�Flash map
 *
 * \param[out] p_flash_size_map ��ȡ����flash��Ϣ
 *
 * \retval  0 ���յ�Ӧ��
 * \retval -1 ��ʱ
 * \retval -2 ��������յ�cmd��ƥ��
 * \retval -3 ���յ�����
 */
int8_t get_flash_size_map (uint8_t *p_flash_size_map);

/**
 * \brief ��ѯIP��ַ
 *
 * \param[out] p_station_ip station ip��ַ
 * \param[out] p_ap_ip      ap ip��ַ
 *
 * \retval  0 ���յ�Ӧ��
 * \retval -1 ��ʱ
 * \retval -2 ��������յ�cmd��ƥ��
 * \retval -3 ���յ�����
 */
int8_t wifi_ip_get (ip_info_t *p_station_ip, ip_info_t *p_ap_ip);

/**
 * \brief ��ѯmac��ַ
 *
 * \param[out] p_station_ip station ip��ַ
 * \param[out] p_ap_ip      ap ip��ַ
 *
 * \retval  0 ���յ�Ӧ��
 * \retval -1 ��ʱ
 * \retval -2 ��������յ�cmd��ƥ��
 * \retval -3 ���յ�����
 */
int8_t wifi_mac_addr_get (uint8_t *p_station_mac, uint8_t *p_ap_mac);

/**
 * \brief ��ȡmesh�豸�б�
 *
 * \param[out] p_node_list mesh�豸�б�
 *
 * \retval  0 ���յ�Ӧ��
 * \retval -1 ��ʱ
 * \retval -2 ��������յ�cmd��ƥ��
 * \retval -3 ���յ�����
 */
int8_t device_list_get (struct mesh_device_list_type *p_node_list);

/**
 * \brief ���÷�����IP��˿�
 *
 * \param[in] p_ip ip��ַ
 * \param[in] port �˿�
 *
 * \retval  0 ���յ�Ӧ��
 * \retval -1 ��ʱ
 * \retval -2 ��������յ�cmd��ƥ��
 * \retval -3 ���յ�����
 */
int8_t server_addr_set (uint8_t *p_ip, uint16_t port);

/**
 * \brief ����MESH��ID
 *
 * \param[in] p_id MESH��ID
 *
 * \retval  0 ���յ�Ӧ��
 * \retval -1 ��ʱ
 * \retval -2 ��������յ�cmd��ƥ��
 * \retval -3 ���յ�����
 */
int8_t mesh_group_id_set (uint8_t *p_id);

/**
 * \brief ����·������Ϣ
 *
 * \param[in] p_ssid   ·��������
 * \param[in] p_passwd ·��������
 * \param[in] auth     ���ܷ�ʽ
 * \param[in] p_mac    ·����mac��ַ(ֻ����������wifiʱ����Ҫ����)
 *
 * \retval  0 ���յ�Ӧ��
 * \retval -1 ��ʱ
 * \retval -2 ��������յ�cmd��ƥ��
 * \retval -3 ���յ�����
 */
int8_t router_set (uint8_t *p_ssid, uint8_t *p_passwd, uint8_t auth, uint8_t *p_mac);

/**
 * \brief ����MESH������Ϣ
 *
 * \param[in] p_ssid   ·��������
 * \param[in] p_passwd ·��������
 *
 * \retval  0 ���յ�Ӧ��
 * \retval -1 ��ʱ
 * \retval -2 ��������յ�cmd��ƥ��
 * \retval -3 ���յ�����
 */
int8_t mesh_wifi_set (uint8_t *p_ssid, uint8_t *p_passwd);

/**
 * \brief ��ʼ��MESH
 *
 * \param ��
 *
 * \retval  0 ���յ�Ӧ��
 * \retval -1 ��ʱ
 * \retval -2 ��������յ�cmd��ƥ��
 * \retval -3 ���յ�����
 */
int8_t mesh_init (void);

#endif /* __ESP8266_CMD_H */

/* end of file */
