/**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: p_database.h
**
** ��   ��   ��: �Ž���
**
** �ļ���������: 2016 �� 09 �� 08 ��
**
** ��        ��: �û���Ϣ���ݿ�

** ��־:
2016.09.08  �������ļ�
*********************************************************************************************************/

#ifndef _P_DATABASE_H_
#define _P_DATABASE_H_

#include <rtthread.h>
#include <sys.h>

/* ��ӡ������Ϣ */
#define printDebugInfo
/* ����û����� */
#define MAX_USER_NUM		1000
/* header��С */
#define HEADER_SIZE			8

#define GET_USER			0	//��ȡ�û�
#define SET_USER			1	//�����û�

#define ADD_ONE_USER		0	//����û�
#define DEL_ONE_USER		1	//ɾ���û�
#define GET_ONE_USER		2	//��ȡ�û���Ϣ

/* ��ȡ�ṹ���Աƫ�ƺ궨�� */
#define OFFSET(Type, member) ( (u32)&(((struct Type*)0)->member) )
#define MEMBER_SIZE(Type, member) sizeof(((struct Type*)0)->member)

__packed struct user_info //��1050�ֽ�
{
	u16 user_id;		// 0-1			//�û���
	u32 card_id;		// 2-5			//����
	u8 effective;		// 6			//����״̬ 0:ʧЧ 1:��Ч
	u8 student_id[16];	// 7-22			//ѧ�� 11������(ǰ��0)
	u8 name[8];			// 23-30		//���� �ұ߲�0x00
	u8 authority[16];	// 31-46		//Ȩ��
	u8 state[16];		// 47-62		//����״̬
	u8 is_time_limit;	// 63			//�Ƿ���ʱ������ 0:������ 1:������
	u16 year;			// 64-65		//��(��Ч�ڣ�����֮���û�ʧЧ)
	u8 month;			// 66			//��
	u8 day;				// 67			//��
	u8 hour;			// 68			//ʱ
	u8 minutes;			// 69			//��
	u8 second;			// 70			//��
	u16 finger_index[5];// 71-80		//ָ������ֵ�ֱ���ָ��ģ���е��û���
	u8 finger[5][193];	// 81-1045		//ָ������ֵ
	u32 crc;			// 1046-1049	//����Ա֮ǰ���г�Ա���ֽڼ����CRCֵ
};

__packed struct card_id_struct //��6�ֽ�
{
	u16 user_id;		//�û���
	u32 card_id;		//����
};

extern const char *user_info_database_path;

/* ֻ�����û��š����ŵĽṹ������,�ɿ�������֮��ʹ��2�ַ����� */
extern struct card_id_struct card_id_array[MAX_USER_NUM];

int bin_search(struct card_id_struct sSource[], int array_size, int key);

extern s8 get_set_user_num(u16 *user_num, u8 cmd);//��ȡ/�������ݿ��е���Ա��Ϣ
extern s8 get_set_user_num_max(u16 *user_num_max, u8 cmd);//��ȡ/�������ݿ��е�����û���
extern s8 add_del_get_one_user(struct user_info *one_user_info, u8 cmd);//���/ɾ��/��ȡ���ݿ��е�һ���û�(��Ҫ����one_user_info->user_id����)
s8 get_set_state(u8 *state, u16 user_id, u8 cmd);
extern s8 init_card_array(struct card_id_struct card_array[]);//��ʼ�����Žṹ����

#endif
