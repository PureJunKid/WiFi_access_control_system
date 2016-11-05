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

#include "finsh.h"

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
* ������ 	: get_set_record_header
* ����   	: ��ȡ/���ÿ������ݿ�header
* ����     	: - att_header_t: �������ݿ�header - cmd: 0: ��ȡ 1: ����
* ���     	: None
* ����ֵ    : -1: ʧ�� 0: �ɹ�
*******************************************************************************/
s8 get_set_record_header(struct att_header *att_header_t, u8 cmd)
{
	int fd;
	
	/* �������Ϸ��� */
	if (att_header_t == 0)
	{
		att_printf("att_header_t addr is 0\r\n");
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
				if (read(fd, att_header_t, sizeof(struct att_header)) != sizeof(struct att_header))
				{
					att_printf("read %s failed\r\n", attendance_database_path);
					close(fd);
					return -1;
				}
				else
				{
					close(fd);
					if (att_header_t->crc == CalcBlockCRC((u8 *)att_header_t, sizeof(struct att_header) - 4))
					{
						att_printf("get record header total is %d, not_upload is %d success\r\n",att_header_t->total, att_header_t->not_upload);
					}
					else
					{
						att_printf("crc validators fail total is %d, not_upload is %d\r\n",att_header_t->total, att_header_t->not_upload);
						return -1;
					}
				}		
			}
			else if (cmd == 1)
			{
				/* ����CRC */
				att_header_t->crc = CalcBlockCRC((u8 *)att_header_t, sizeof(struct att_header) - 4);
				/* д�뿼�ڼ�¼������ */
				if (write(fd, att_header_t, sizeof(struct att_header)) != sizeof(struct att_header))
				{
					att_printf("write %s failed\r\n", attendance_database_path);
					close(fd);
					return -1;
				}
				else
				{
					close(fd);
					att_printf("set record header %d, %d success\r\n",att_header_t->total, att_header_t->not_upload);			
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
* ������ 	: get_set_att_record
* ����   	: ��ȡ/����һ�����ڼ�¼
* ����     	: - one_att_info: ���ڼ�¼�ṹ�� - cmd: 0: ��ȡ 1: ����
* ���     	: None
* ����ֵ    : -1: ʧ�� 0: �ɹ�
*******************************************************************************/
s8 get_set_att_record(struct att_info *one_att_info, u8 cmd)
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
		if (lseek(fd, (sizeof(struct att_info) * (one_att_info->record_id - 1)) + sizeof(struct att_header), SEEK_SET) == -1)
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
						if (one_att_info->crc == CalcBlockCRC((u8 *)one_att_info, sizeof(struct att_info) - 4))
						{
							att_printf("get att record %d success\r\n", one_att_info->record_id);			
						}
						else
						{
							att_printf("crc validators fail,record_id is %d\r\n",one_att_info->record_id);
							return -1;
						}
					}
				}	break;
				/* ��ӿ��ڼ�¼ */
				case 1:
				{
					/* ����CRC */
					one_att_info->crc = CalcBlockCRC((u8 *)one_att_info, sizeof(struct att_info) - 4);
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
						att_printf("set att record %d success\r\n", one_att_info->user_id);			
					}
				}	break;
				default :
				{
					close(fd);
					att_printf("get set att record cmd unknown\r\n");					
					return -1;
				}
			}
		}
	}
	
	return 0;
}/*******************************************************************************
* ������ 	: print_record
* ����   	: ͨ�����ڴ�ӡָ���������ڼ�¼
* ����     	: - count: ��Ҫ��ӡ������
* ���     	: None
* ����ֵ    : None
*******************************************************************************/
void print_record(u32 count)
{
	struct att_info one_att_info;
	u32 i;
	
	if (count > att_header_t.total)
	{
		count = att_header_t.total;
	}
	rt_kprintf("total is %d, print %d\r\n",att_header_t.total, count);
	for (i = 0; i < count; i++)
	{
		one_att_info.record_id = i + 1;
		if (get_set_att_record(&one_att_info, GET_RECORD) == 0)
		{
			rt_kprintf("is_upload: %d is_delete: %d record_id: %5d user_id: %5d student_id: %11s name: %4s device_addr: %3d state: %d ",
				one_att_info.is_upload, one_att_info.is_delete, one_att_info.record_id, one_att_info.user_id, 
				one_att_info.student_id, one_att_info.name, one_att_info.device_addr, one_att_info.state);
			
			rt_kprintf(" 20%02d.%02d.%02d  %02d.%02d.%02d\r\n",
				one_att_info.year, one_att_info.month, one_att_info.day, one_att_info.hour, 
				one_att_info.minutes, one_att_info.second);
			
		}
		else
		{
			rt_kprintf("get_set_att_record failed\r\n");
		}
	}
}
FINSH_FUNCTION_EXPORT(print_record, print record)

