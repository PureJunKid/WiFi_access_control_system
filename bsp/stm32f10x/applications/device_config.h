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
 * \brief �豸�����ļ����
 * 
 * \internal
 * \par Modification history
 * - 1.00 16-09-19 zhangjinke, first implementation
 * \endinternal
 */
#ifndef __DEVICE_CONFIG
#define __DEVICE_CONFIG

#include <rtthread.h>
#include "global.h"

#define PRINT_CFG_DEBUG_INFO           /**< \brief ��ӡ������Ϣ */

#define GET_DEVICE        0            /**< \brief ��ȡ�豸xxx */
#define SET_DEVICE        1            /**< \brief �����豸xxx */

/** \brief ϵͳ���ýṹ */
__packed typedef struct device_config
{
    uint8_t  device_addr;                /**< \brief ������ַ */
    uint8_t  this_device_name[64];       /**< \brief �������� */
    uint8_t  server_ip[4];               /**< \brief ������IP��ַ */
    uint16_t server_port;                /**< \brief �������˿� */
    uint8_t  router_ssid[32 + 1];        /**< \brief ·�������� */
    uint8_t  router_passwd[256 + 1];     /**< \brief ·�������� */
    uint8_t  router_auth;                /**< \brief ·�������ܷ�ʽ */
    uint8_t  router_bssid[6];            /**< \brief ·����MAC��ַ */
    uint8_t  mesh_ssid[32 + 1];          /**< \brief mesh�������� */
    uint8_t  mesh_passwd[256 + 1];       /**< \brief mesh�������� */
    uint8_t  mesh_group_id[6];           /**< \brief mesh��ID */
    uint32_t crc;                        /**< \brief crcУ�� */
} device_config_t;

/** \brief ϵͳ������Ϣ */
extern device_config_t g_device_config;

/**
 * \brief ��ʼ���豸����
 *
 * \param[in] p_device_config �豸���ýṹ��
 *
 * \retval  0 �ɹ�
 * \retval -1 ʧ��
 */
int8_t device_config_init (device_config_t *p_device_config);

/**
 * \brief ��ȡ/�����豸����
 *
 * \param[in,out] device_config �豸���ýṹ��
 * \param[in]     cmd 0: ��ȡ�豸���� 1: �����豸����
 *
 * \retval  0 �ɹ�
 * \retval -1 ʧ��
 */
int8_t device_config_get_set (device_config_t *p_device_config, uint8_t cmd);

/**
 * \brief ��ӡ�豸������Ϣ
 *
 * \param[in] p_device_config �豸���ýṹ��
 *
 * \return ��
 */
void device_config_print (device_config_t *p_device_config);

#endif /* __DEVICE_CONFIG */

/* end of file */
