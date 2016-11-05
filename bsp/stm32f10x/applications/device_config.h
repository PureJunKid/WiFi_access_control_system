/**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: device_config.h
**
** ��   ��   ��: �Ž���
**
** �ļ���������: 2016 �� 09 �� 19 ��
**
** ��        ��: �豸�����ļ����

** ��־:
2016.09.19  �������ļ�
*********************************************************************************************************/

#ifndef _DEVICE_CONFIG_
#define _DEVICE_CONFIG_

#include <rtthread.h>
#include <sys.h>

/* ��ӡ������Ϣ */
#define printCfgDebugInfo

#define GET_DEVICE		0			//��ȡ�豸xxx
#define SET_DEVICE		1			//�����豸xxx

__packed struct device_config
{
	u8  device_addr;				//������ַ
	s8  this_device_name[64];			//��������
	u32 crc;						//crcУ��
};

extern struct device_config device_config_t;

extern s8 init_device_config(void);                                      /* ��ʼ���豸���� */
extern s8 get_set_device_config(struct device_config *device_config_t, u8 cmd); /* ��ȡ/�����豸���� */


#endif
