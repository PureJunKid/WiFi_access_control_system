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

#ifndef __SERVER_PROCESS_H
#define __SERVER_PROCESS_H

#include <stdint.h>

#define CMD_SERVER_USER_ADD     (0x01) /**< \brief ����û� */
#define CMD_SERVER_USER_DEL     (0x02) /**< \brief ɾ���û� */
#define CMD_SERVER_CONNECT      (0x03) /**< \brief ���� */
#define CMD_SERVER_TIMESYNC     (0x04) /**< \brief ͬ��ʱ�� */
#define CMD_SERVER_ATT_GET      (0x05) /**< \brief ��ȡָ������������Ϣ ÿ�����25�� */
#define CMD_SERVER_DOOR_OPEN    (0x06) /**< \brief ���� */
#define CMD_SERVER_CONFIG_SET   (0x07) /**< \brief �´�������Ϣ */

#define CMD_SERVER_FILE_START   (0xF0) /**< \brief ��ʼ�ļ����� */
#define CMD_SERVER_FILE_DATA    (0xF1) /**< \brief �ļ����� */
#define CMD_SERVER_FILE_END     (0xF2) /**< \brief �ļ�������� */

#define CMD_SERVER_ACK          (0x55) /**< \brief ACK */
#define CMD_SERVER_NACK         (0xAA) /**< \brief NACK */


/**
 * \brief ������������͵�����
 *
 * \param[in] cmd    : ����
 * \param[in] p_data : �����׵�ַ
 * \param[in] lenth  : ���ݳ���
 *
 * \return ��
 */
void data_process(uint8_t cmd, uint8_t *p_data, uint16_t lenth);

#endif /* __SERVER_PROCESS_H */

/* end of file */
