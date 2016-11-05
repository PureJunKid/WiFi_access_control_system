/**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: att_database.h
**
** ��   ��   ��: �Ž���
**
** �ļ���������: 2016 �� 11 �� 03 ��
**
** ��        ��: ������Ϣ���ݿ�

** ��־:
2016.11.03  �������ļ�
*********************************************************************************************************/

#ifndef _ATT_DATABASE_H_
#define _ATT_DATABASE_H_

#include <rtthread.h>
#include <sys.h>

/* ��ӡ������Ϣ */
//#define printAttDebugInfo

#define GET_RECORD			0	//��ȡ��¼
#define SET_RECORD			1	//���ü�¼

__packed struct att_info
{
	u8  is_upload;			//�Ƿ��ϴ� 0:δ�ϴ� 1:���ϴ�
	u8  is_delete;			//�Ƿ�ɾ�� 0:δɾ�� 1:��ɾ��
	u32 record_id;			//���ڼ�¼ID��
	u16 user_id;			//�û���
	u8  student_id[16];		//ѧ�� 11������(ǰ��0)
	u8  name[8];			//���� �ұ߲�0x00
	u8  device_addr;		//�豸��ַ
	u8  state;				//����״̬ 0:���� 1:����
	u16 year;				//��(��Ч�ڣ�����֮���û�ʧЧ)
	u8  month;				//��
	u8  day;				//��
	u8  hour;				//ʱ
	u8  minutes;			//��
	u8  second;				//��
	u32 crc;				//����Ա֮ǰ���г�Ա���ֽڼ����CRCֵ
};

__packed struct att_header
{
	u32 total;				//������
	u32 not_upload;			//δ�ϴ�����
	u32 crc;				//crcУ��
};

extern struct att_header att_header_t;                               /* ������Ϣ���ݿ�header�ṹ�� */

extern s8 init_att_database(void);                                        /* ��ʼ���������ݿ� */
extern s8 get_set_record_header(struct att_header *att_header_t, u8 cmd); /* ��ȡ/���ÿ������ݿ�header */
extern s8 get_set_att_record(struct att_info *one_att_info, u8 cmd);      /* ��ȡ/����һ�����ڼ�¼ */
extern s8 get_set_delete(u32 record_id, u8 *is_delete, u8 cmd);           /* ��ȡ/���ÿ��ڼ�¼�Ƿ�ɾ�� */
extern s8 get_set_upload(u32 record_id, u8 *is_upload, u8 cmd);           /* ��ȡ/���ÿ��ڼ�¼�Ƿ��ϴ� */
extern s8 wipe_att_database(void);                                        /* ��տ��ڼ�¼ */

#endif
