/**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: att_database.c
**
** ��   ��   ��: �Ž���
**
** �ļ���������: 2016 �� 11 �� 03 ��
**
** ��        ��: ������Ϣ���ݿ�

** ��־:
2016.11.03  �������ļ�
*********************************************************************************************************/

#include <rtthread.h>
#include "att_database.h"
/* �ļ�ϵͳ���ͷ�ļ� */
#include <dfs.h>
#include <dfs_posix.h>

#include "stm32_crc.h"

/* ��ȡ�ṹ���Աƫ�ƺ궨�� */
#define OFFSET(Type, member) ( (u32)&(((struct Type*)0)->member) )
#define MEMBER_SIZE(Type, member) sizeof(((struct Type*)0)->member)

/* ��ʱ���� */
#define rt_thread_delayMs(x) rt_thread_delay(rt_tick_from_millisecond(x))
/* ���õ�����Ϣ��ӡ���� */
#ifdef printAttDebugInfo
#define att_printf(fmt,args...) rt_kprintf(fmt, ##args)
#else
#define att_printf(fmt,args...)
#endif

/* ������Ϣ���ݿ�·�� */
const char *attendance_database_path = "/save_io.bin";
/* ������Ϣ���ݿ�header�ṹ�� */
struct att_header att_header_t;

/*******************************************************************************
* ������ 	: init_att_database
* ����   	: ��ʼ���������ݿ�
* ����     	: None
* ���     	: None
* ����ֵ    : -1: ʧ�� 0: �ɹ�
*******************************************************************************/
s8 init_att_database(void)
{
	int fd = -1;
	u32 crc = 0;
	
	/* ����Ա��Ϣ���ݿ� */
	fd = open(attendance_database_path, O_RDWR|O_CREAT, 0);
	if (fd < 0)
	{
		att_printf("open %s failed\r\n", attendance_database_path);
		close(fd);
		return -1;
	}
	else
	{
		/* �ƶ��ļ�ָ�뵽ָ��λ�� */
		if (lseek(fd, 0, SEEK_SET) == -1)
		{
			att_printf("lseek %s failed\r\n", attendance_database_path);
			close(fd);
			return -1;
		}
		else
		{
			/* ��ȡ���ڼ�¼header��Ϣ */
			read(fd, &att_header_t, sizeof(struct att_header));
			
			/* ����CRC */
			crc = CalcBlockCRC((u8 *)(&att_header_t), sizeof(struct att_header) - 4);
			if (crc != att_header_t.crc)
			{
				att_printf("att header crc validators fail, init att database\r\n");
				/* ��ʼ�����ڼ�¼���ݿ� */
				att_header_t.total = 0;
				att_header_t.not_upload = 0;
				/* ����CRC */
				att_header_t.crc = CalcBlockCRC((u8 *)(&att_header_t), sizeof(struct att_header) - 4);
				
				/* �ƶ��ļ�ָ�뵽ָ��λ�� */
				if (lseek(fd, 0, SEEK_SET) == -1)
				{
					att_printf("lseek %s failed\r\n", attendance_database_path);
					close(fd);
					return -1;
				}
				/* д�뿼�ڼ�¼������ */
				if (write(fd, &att_header_t, sizeof(struct att_header)) != sizeof(struct att_header))
				{
					att_printf("write %s failed\r\n", attendance_database_path);
					close(fd);
					return -1;
				}
				else
				{
					close(fd);
					att_printf("init att database success\r\n");			
				}		
			}
		}
	}
	
	return 0;
}

/*******************************************************************************
* ������ 	: get_set_record_count
* ����   	: ��ȡ/�������ݿ��п��ڼ�¼������
* ����     	: - record_count: ���ڼ�¼������ - cmd: 0: ��ȡ 1: ����
* ���     	: None
* ����ֵ    : -1: ʧ�� 0: �ɹ�
*******************************************************************************/
s8 get_set_record_count(u32 *record_count, u8 cmd)
{
	int fd;
	
	/* �������Ϸ��� */
	if (record_count == 0)
	{
		att_printf("record_count addr is 0\r\n");
		return -1;
	}
	/* ����Ա��Ϣ���ݿ� */
	fd = open(attendance_database_path, O_RDWR|O_CREAT, 0);
	if (fd < 0)
	{
		att_printf("open %s failed\r\n", attendance_database_path);
		close(fd);
		return -1;
	}
	else
	{
		/* �ƶ��ļ�ָ�뵽ָ��λ�� */
		if (lseek(fd, 0, SEEK_SET) == -1)
		{
			att_printf("lseek %s failed\r\n", attendance_database_path);
			close(fd);
			return -1;
		}
		else
		{
			if (cmd == 0)
			{
				/* ��ȡ���ڼ�¼������ */
				if (read(fd, record_count, MEMBER_SIZE(att_header, total)) != MEMBER_SIZE(att_header, total))
				{
					att_printf("read %s failed\r\n", attendance_database_path);
					close(fd);
					return -1;
				}
				else
				{
					close(fd);
					att_printf("get record count %d success\r\n", *record_count);			
				}		
			}
			else if (cmd == 1)
			{
				/* д�뿼�ڼ�¼������ */
				if (write(fd, record_count, MEMBER_SIZE(att_header, total)) != MEMBER_SIZE(att_header, total))
				{
					att_printf("write %s failed\r\n", attendance_database_path);
					close(fd);
					return -1;
				}
				else
				{
					close(fd);
					att_printf("set record count %d success\r\n", *record_count);			
				}		
			}
			else
			{
				close(fd);
				att_printf("get set record count cmd unknown\r\n");
				return -1;
			}
		}
	}
	
	return 0;
}
/*******************************************************************************
* ������ 	: get_set_not_upload
* ����   	: ��ȡ/�������ݿ��п��ڼ�¼δ�ϴ�����
* ����     	: - record_count: ���ڼ�¼δ�ϴ����� - cmd: 0: ��ȡ 1: ����
* ���     	: None
* ����ֵ    : -1: ʧ�� 0: �ɹ�
*******************************************************************************/
s8 get_set_not_upload(u32 *not_upload, u8 cmd)
{
	int fd;
	
	/* �������Ϸ��� */
	if (not_upload == 0)
	{
		att_printf("not_upload addr is 0\r\n");
		return -1;
	}
	/* ����Ա��Ϣ���ݿ� */
	fd = open(attendance_database_path, O_RDWR|O_CREAT, 0);
	if (fd < 0)
	{
		att_printf("open %s failed\r\n", attendance_database_path);
		close(fd);
		return -1;
	}
	else
	{
		/* �ƶ��ļ�ָ�뵽ָ��λ�� */
		if (lseek(fd, OFFSET(att_header, not_upload), SEEK_SET) == -1)
		{
			att_printf("lseek %s failed\r\n", attendance_database_path);
			close(fd);
			return -1;
		}
		else
		{
			if (cmd == 0)
			{
				/* ��ȡ���ڼ�¼������ */
				if (read(fd, not_upload, MEMBER_SIZE(att_header, not_upload)) != MEMBER_SIZE(att_header, not_upload))
				{
					att_printf("read %s failed\r\n", attendance_database_path);
					close(fd);
					return -1;
				}
				else
				{
					close(fd);
					att_printf("get not upload record count %d success\r\n", *not_upload);			
				}		
			}
			else if (cmd == 1)
			{
				/* д�뿼�ڼ�¼������ */
				if (write(fd, not_upload, MEMBER_SIZE(att_header, not_upload)) != MEMBER_SIZE(att_header, not_upload))
				{
					att_printf("write %s failed\r\n", attendance_database_path);
					close(fd);
					return -1;
				}
				else
				{
					close(fd);
					att_printf("set upload record record count %d success\r\n", *not_upload);			
				}		
			}
			else
			{
				close(fd);
				att_printf("get set upload record record count cmd unknown\r\n");
				return -1;
			}
		}
	}
	
	return 0;
}

/*******************************************************************************
* ������ 	: add_delone_att_record
* ����   	: ���/ɾ��һ�����ڼ�¼
* ����     	: - one_att_info: ���ڼ�¼�ṹ�� - cmd: 0: ��ȡ 1: ���
* ���     	: None
* ����ֵ    : -1: ʧ�� 0: �ɹ�
*******************************************************************************/
s8 add_one_att_record(struct att_info *one_att_info, u8 cmd)
{
	int fd;
	
	/* �������Ϸ��� */
	if (one_att_info == 0)
	{
		att_printf("one_att_info addr is 0\r\n");
		return -1;
	}

	/* ����Ա��Ϣ���ݿ� */
	fd = open(attendance_database_path, O_RDWR|O_CREAT, 0);
	if (fd < 0)
	{
		att_printf("open %s failed\r\n", attendance_database_path);
		close(fd);
		return -1;
	}
	else
	{
		/* �ƶ��ļ�ָ�뵽ָ��λ�� */
		if (lseek(fd, (sizeof(struct att_info) * (one_att_info->record_id)) + sizeof(struct att_header), SEEK_SET) == -1)
		{
			att_printf("lseek %s failed\r\n", attendance_database_path);
			close(fd);
			return -1;
		}
		else
		{
			switch(cmd)
			{
				/* ��ȡ���ڼ�¼ */
				case 0:
				{
					/* ��ȡ */
					if (read(fd, one_att_info, sizeof(struct att_info)) != sizeof(struct att_info))
					{
						att_printf("read %s failed\r\n", attendance_database_path);
						close(fd);
						return -1;
					}
					else
					{
						close(fd);
						att_printf("get att record %d success\r\n", one_att_info->record_id);			
					}
				}	break;
				/* ��ӿ��ڼ�¼ */
				case 1:
				{
					/* д�� */
					if (write(fd, one_att_info, sizeof(struct att_info)) != sizeof(struct att_info))
					{
						att_printf("write %s failed\r\n", attendance_database_path);
						close(fd);
						return -1;
					}
					else
					{
						close(fd);
						att_printf("add att record %d success\r\n", one_att_info->user_id);			
					}
				}	break;
				default :
				{
					close(fd);
					att_printf("add delone att record cmd unknown\r\n");					
					return -1;
				}
			}
		}
	}
	
	return 0;
}
