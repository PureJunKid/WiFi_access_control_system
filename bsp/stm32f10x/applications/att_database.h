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
 * \brief ������Ϣ���ݿ�
 *
 * \internal
 * \par Modification history
 * - 1.00 16-11-03  zhangjinke, first implementation.
 * \endinternal
 */ 

#ifndef __ATT_DATABASE_H
#define __ATT_DATABASE_H

#include <stdint.h>

#if 0
#define PRINT_ATT_DEBUG_INFO    /**< \brief ��ӡ������Ϣ */
#endif

#define GET_RECORD            0 /**< \brief ��ȡ��¼ */
#define SET_RECORD            1 /**< \brief ���ü�¼ */

/** \brief ��Ա��Ϣ�ṹ�� */
__packed typedef struct att_info
{
    uint16_t user_id;            /**< \brief �û��� */
    uint8_t  student_id[16];     /**< \brief ѧ�� */
    uint8_t  name[16];           /**< \brief ���� */
    uint8_t  device_addr;        /**< \brief �豸��ַ */
    uint8_t  mac_addr[6];        /**< \brief mac��ַ */
    uint8_t  state;              /**< \brief ����״̬ 0:���� 1:���� */
    uint16_t year;               /**< \brief ��(��Ч�ڣ�����֮���û�ʧЧ) */
    uint8_t  month;              /**< \brief �� */
    uint8_t  day;                /**< \brief �� */
    uint8_t  hour;               /**< \brief ʱ */
    uint8_t  minutes;            /**< \brief �� */
    uint8_t  second;             /**< \brief �� */
    uint32_t crc;                /**< \brief ����Ա֮ǰ���г�Ա���ֽڼ����CRCֵ */
} att_info_t;

/** \brief ��Ա��Ϣ���ݿ�ͷ */
__packed typedef struct att_header
{
    uint32_t total;              /**< \brief ������ */
    uint32_t crc;                /**< \brief crcУ�� */
} att_header_t;

extern att_header_t g_att_header;                               /* ������Ϣ���ݿ�header�ṹ�� */

/**
 * \brief ��ʼ���������ݿ�
 *
 * \param[in] p_att_header ������Ϣͷ
 *
 * \retval  0 �ɹ�
 * \retval -1 ʧ��
 */
int8_t att_database_init (att_header_t *p_att_header);

/**
 * \brief ��ȡ/���ÿ������ݿ�header
 *
 * \param[in,out] p_att_header ������Ϣͷ
 * \param[in]     cmd          0: ��ȡ 1: ����
 *
 * \retval  0 �ɹ�
 * \retval -1 ʧ��
 */
int8_t record_header_get_set (att_header_t *p_att_header, uint8_t cmd);

/**
 * \brief ���һ�����ڼ�¼
 *
 * \param[in] p_one_att_info ���ڼ�¼�ṹ��
 *
 * \retval  0 �ɹ�
 * \retval -1 ʧ��
 */
int8_t att_record_add (att_info_t *p_one_att_info);

/**
 * \brief ɾ��ָ���������ڼ�¼
 *
 * \param[in] num ��ɾ��������
 *
 * \retval  0 �ɹ�
 * \retval -1 ʧ��
 */
int8_t att_record_del (uint16_t num);

/**
 * \brief ��ȡ���ڼ�¼(�����һ����¼��ʼ��ȡ)
 *
 * \param[out] att_info    ���ڼ�¼
 * \param[in]  lenth       ����
 * \param[out] p_att_count ��ȡ���Ŀ��ڼ�¼����
 *
 * \retval  0 �ɹ�
 * \retval -1 ʧ��
 */
int8_t att_record_get (att_info_t att_info[], uint8_t lenth, uint16_t *p_att_count);

/**
 * \brief ��տ��ڼ�¼
 *
 * \param[in] p_att_header ������Ϣͷ
 *
 * \retval  0 �ɹ�
 * \retval -1 ʧ��
 */
int8_t att_database_wipe (att_header_t *p_att_header);

#endif /* __ATT_DATABASE_H */

/* end of file */
