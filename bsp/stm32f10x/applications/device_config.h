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

#define GET_DEVICE		0	//��ȡ�豸xxx
#define SET_DEVICE		1	//�����豸xxx


extern u8 device_addr;
extern char this_device_name[64];

extern s8 get_set_device_addr(u8 *device_addr, u8 cmd);	//��ȡ/�����豸��ַ
extern s8 get_set_device_name(char *device_name, u8 cmd);	//��ȡ/�����豸����


#endif
