/**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: p_database.c
**
** ��   ��   ��: �Ž���
**
** �ļ���������: 2016 �� 09 �� 08 ��
**
** ��        ��: �û���Ϣ���ݿ�

** ��־:
2016.09.08  �������ļ�
*********************************************************************************************************/
#include <rtthread.h>
#include "p_database.h"
/* �ļ�ϵͳ���ͷ�ļ� */
#include <dfs.h>
#include <dfs_posix.h>
/* ��ʱ���� */
#define rt_thread_delayMs(x) rt_thread_delay(rt_tick_from_millisecond(x))
/* ���õ�����Ϣ��ӡ���� */
#ifdef printDebugInfo
#define p_printf(fmt,args...) rt_kprintf(fmt, ##args)
#else
#define p_printf(fmt,args...)
#endif

/* �û���Ϣ���ݿ�·�� */
const char *user_info_database_path = "/member.bin";

/* ֻ�����û��š����ŵĽṹ������,�ɿ�������֮��ʹ��2�ַ����� */
struct card_id_struct card_id_array[MAX_USER_NUM];

/*******************************************************************************
* ������ 	: bin_search
* ����   	: �Է�����
* ����     	: - sSource: ��С��������õ����� - array_size: �����С - key:Ŀ����ֵ
* ���     	: None
* ����ֵ    : -1: δ������ ����: ��������������±�
*******************************************************************************/
int bin_search(struct card_id_struct sSource[], int array_size, int key)  
{     
    int low = 0, high = array_size - 1, mid;  
      
    while (low <= high)  
    {         
        mid = (low + high) / 2;//��ȡ�м��λ��  
          
        if (sSource[mid].card_id == key)              
            return mid; //�ҵ��򷵻���Ӧ��λ��  
        if (sSource[mid].card_id > key)            
            high = mid - 1; //�����key�������͵�λ�ò���  
        else  
            low = mid + 1;  //�����keyС�������ߵ�λ�ò���  
    }     
    return -1;    
}

/*******************************************************************************
* ������ 	: quik_sort
* ����   	: ���������㷨
* ����     	: - card_id_array: ��Ҫ��������� - low: �����±߽� - high:�����ϱ߽�
* ���     	: None
* ����ֵ    : None
*******************************************************************************/
void quik_sort(struct card_id_struct card_id_array[],int low,int high)
{
	int i = low;
	int j = high;  
	struct card_id_struct temp = card_id_array[i]; 

	if (low < high)
	{
		while(i < j) 
		{
			while((card_id_array[j].card_id >= temp.card_id) && (i < j))
			{ 
				j--; 
			}
			card_id_array[i] = card_id_array[j];
			while((card_id_array[i].card_id <= temp.card_id) && (i < j))
			{
				i++; 
			}  
			card_id_array[j]= card_id_array[i];
		}
		card_id_array[i] = temp;
		quik_sort(card_id_array,low,i-1);
		quik_sort(card_id_array,j+1,high);
	}
	else
	{
		return;
	}
}

/*******************************************************************************
* ������ 	: get_set_user_num
* ����   	: ��ȡ/�������ݿ��е��û���Ϣ����
* ����     	: - user_num: ��Ա���� - cmd: 0: ��ȡ��Ա���� 1: ������Ա����
* ���     	: None
* ����ֵ    : -1: ʧ�� 0: �ɹ�
*******************************************************************************/
s8 get_set_user_num(u16 *user_num, u8 cmd)
{
	int fd;
	/* ����Ա��Ϣ���ݿ� */
	fd = open(user_info_database_path, O_RDWR|O_CREAT, 0);
	if (fd < 0)
	{
		p_printf("open %s failed\r\n", user_info_database_path);
		close(fd);
		return -1;
	}
	else
	{
		/* �ƶ��ļ�ָ�뵽ָ��λ�� */
		if (lseek(fd, 0, SEEK_SET) == -1)
		{
			p_printf("lseek %s failed\r\n", user_info_database_path);
			close(fd);
			return -1;
		}
		else
		{
			if (cmd == 0)
			{
				/* ��ȡ��Ա���� */
				if (read(fd, user_num, 2) != 2)
				{
					p_printf("read %s failed\r\n", user_info_database_path);
					close(fd);
					return -1;
				}
				else
				{
					close(fd);
					p_printf("get user num %d success\r\n", *user_num);			
				}		
			}
			else if (cmd == 1)
			{
				/* д����Ա���� */
				if (write(fd, user_num, 2) != 2)
				{
					p_printf("write %s failed\r\n", user_info_database_path);
					close(fd);
					return -1;
				}
				else
				{
					close(fd);
					p_printf("set user num %d success\r\n", *user_num);			
				}		
			}
			else
			{
				close(fd);
				p_printf("set user num cmd unknown\r\n");
				return -1;
			}
		}
	}
	
	return 0;
}

/*******************************************************************************
* ������ 	: get_set_user_num
* ����   	: ��ȡ/�������ݿ��е�����û���
* ����     	: - user_num: ��Ա���� - cmd: 0: ��ȡ����û��� 1: ��������û���
* ���     	: None
* ����ֵ    : -1: ʧ�� 0: �ɹ�
*******************************************************************************/
s8 get_set_user_num_max(u16 *user_num_max, u8 cmd)
{
	int fd;
	/* ����Ա��Ϣ���ݿ� */
	fd = open(user_info_database_path, O_RDWR|O_CREAT, 0);
	if (fd < 0)
	{
		p_printf("open %s failed\r\n", user_info_database_path);
		close(fd);
		return -1;
	}
	else
	{
		/* �ƶ��ļ�ָ�뵽ָ��λ�� */
		if (lseek(fd, 2, SEEK_SET) == -1)
		{
			p_printf("lseek %s failed\r\n", user_info_database_path);
			close(fd);
			return -1;
		}
		else
		{
			if (cmd == 0)
			{
				/* ��ȡ����û��� */
				if (read(fd, user_num_max, 2) != 2)
				{
					p_printf("read %s failed\r\n", user_info_database_path);
					close(fd);
					return -1;
				}
				else
				{
					close(fd);
					p_printf("get user num max %d success\r\n", *user_num_max);			
				}		
			}
			else if (cmd == 1)
			{
				/* д������û��� */
				if (write(fd, user_num_max, 2) != 2)
				{
					p_printf("write %s failed\r\n", user_info_database_path);
					close(fd);
					return -1;
				}
				else
				{
					close(fd);
					p_printf("set user num max %d success\r\n", *user_num_max);			
				}		
			}
			else
			{
				close(fd);
				p_printf("set user num max cmd unknown\r\n");
				return -1;
			}
		}
	}
	
	return 0;
}

/*******************************************************************************
* ������ 	: add_del_get_one_user
* ����   	: ���/ɾ��/��ȡ���ݿ��е�һ���û�(��Ҫ����one_user_info->user_id����)
* ����     	: - one_user_info: ��Ա��Ϣ�ṹ�� - cmd: 0: ��� 1: ɾ�� 2: ��ȡ
* ���     	: None
* ����ֵ    : -1: ʧ�� 0: �ɹ�
*******************************************************************************/
s8 add_del_get_one_user(struct user_info *one_user_info, u8 cmd)
{
	int fd;

	/* ����Ա��Ϣ���ݿ� */
	fd = open(user_info_database_path, O_RDWR|O_CREAT, 0);
	if (fd < 0)
	{
		p_printf("open %s failed\r\n", user_info_database_path);
		close(fd);
		return -1;
	}
	else
	{
		/* �ƶ��ļ�ָ�뵽ָ��λ�� */
		if (lseek(fd, (sizeof(struct user_info)*(one_user_info->user_id-1)) + HEADER_SIZE, SEEK_SET) == -1)
		{
			p_printf("lseek %s failed\r\n", user_info_database_path);
			close(fd);
			return -1;
		}
		else
		{
			switch(cmd)
			{
				/* ����û� */
				case 0:
				{
					/* д���û���Ϣ */
					if (write(fd, one_user_info, sizeof(struct user_info)) != sizeof(struct user_info))
					{
						p_printf("write %s failed\r\n", user_info_database_path);
						close(fd);
						return -1;
					}
					else
					{
						close(fd);
						p_printf("add user %d success\r\n", one_user_info->user_id);			
					}
				}	break;
				/* ɾ���û� */
				case 1:
				{
					/* д���û���Ϣ */
					rt_memset(one_user_info, 0,sizeof(struct user_info));//���û���Ϣ����
					if (write(fd, one_user_info, sizeof(struct user_info)) != sizeof(struct user_info))
					{
						p_printf("write %s failed\r\n", user_info_database_path);
						close(fd);
						return -1;
					}
					else
					{
						close(fd);
						p_printf("add user %d success\r\n", one_user_info->user_id);			
					}
				}	break;
				/* ��ȡ�û���Ϣ */
				case 2:
				{
					/* ��ȡ�û���Ϣ */
					if (read(fd, one_user_info, sizeof(struct user_info)) != sizeof(struct user_info))
					{
						p_printf("read %s failed\r\n", user_info_database_path);
						close(fd);
						return -1;
					}
					else
					{
						close(fd);
						p_printf("get user %d success\r\n", one_user_info->user_id);			
					}
				}	break;
				default :
				{
					close(fd);
					p_printf("add_del_get one user cmd unknown\r\n");					
					return -1;
				}
			}
		}
	}
	
	return 0;
}

/*******************************************************************************
* ������ 	: get_set_state
* ����   	: ��ȡ/����ָ���û��Ľ���״̬
* ����     	: - state: ״̬ - user_id: �û��� - cmd: 0: ��ȡ״̬ 1: ����״̬
* ���     	: None
* ����ֵ    : -1: ʧ�� 0: �ɹ�
*******************************************************************************/
s8 get_set_state(u8 *state, u16 user_id, u8 cmd)
{
	int fd;

	/* ����Ա��Ϣ���ݿ� */
	fd = open(user_info_database_path, O_RDWR|O_CREAT, 0);
	if (fd < 0)
	{
		p_printf("open %s failed\r\n", user_info_database_path);
		close(fd);
		return -1;
	}	
	else
	{
		/* �ƶ��ļ�ָ�뵽ָ��λ�� */
		if (lseek(fd, (sizeof(struct user_info)*(user_id-1)) + OFFSET(user_info,state) + HEADER_SIZE, SEEK_SET) == -1)
		{
			p_printf("lseek %s failed\r\n", user_info_database_path);
			close(fd);
			return -1;
		}
		else
		{
			switch(cmd)
			{
				/* ��ȡ����״̬ */
				case 0:
				{
					/* ��ȡ����״̬ */
					if (read(fd, state, MEMBER_SIZE(user_info,state)) != MEMBER_SIZE(user_info,state))
					{
						p_printf("read %s failed\r\n", user_info_database_path);
						close(fd);
						return -1;
					}
					else
					{
						close(fd);
						p_printf("get state %d success\r\n", user_id);			
					}
				}	break;
				/* ���ý���״̬ */
				case 1:
				{
					/* д�����״̬ */
					if (write(fd, state, MEMBER_SIZE(user_info,state)) != MEMBER_SIZE(user_info,state))
					{
						p_printf("write %s failed\r\n", user_info_database_path);
						close(fd);
						return -1;
					}
					else
					{
						close(fd);
						p_printf("set state %d success\r\n", user_id);			
					}
				}	break;
				default :
				{
					close(fd);
					p_printf("get set state cmd unknown\r\n");					
					return -1;
				}
			}
		}
	}
	
	return 0;
}




/*******************************************************************************
* ������ 	: init_card_array
* ����   	: ��ʼ�����Žṹ����
* ����     	: - card_array: ���Žṹ����
* ���     	: None
* ����ֵ    : -1: ʧ�� 0: �ɹ�
*******************************************************************************/
s8 init_card_array(struct card_id_struct card_array[])
{
	int fd;
	int i;
	u16 user_num,max_user_num;
	u16 effective_num = 0;
	
	if(get_set_user_num(&user_num,GET_USER) == -1)
	{
		p_printf("get user num failed\r\n");
		return -1;
	}
	
	if(get_set_user_num_max(&max_user_num,GET_USER) == -1)
	{
		p_printf("get user num max failed\r\n");
		return -1;
	}
	/* ����Ա��Ϣ���ݿ� */
	fd = open(user_info_database_path, O_RDWR|O_CREAT, 0);
	if (fd < 0)
	{
		p_printf("open %s failed\r\n", user_info_database_path);
		close(fd);
		return -1;
	}
	else
	{
		for (i = 1; i<=max_user_num; i++)
		{
			/* �ƶ��ļ�ָ�뵽ָ��λ�� */
			if (lseek(fd, (sizeof(struct user_info)*(i-1)) + HEADER_SIZE, SEEK_SET) == -1)
			{
				p_printf("lseek %s failed\r\n", user_info_database_path);
				close(fd);
				return -1;
			}
			else
			{
				/* ��ȡ�û���Ϣ */
				if (read(fd, (void *)&card_array[effective_num], sizeof(struct card_id_struct)) != sizeof(struct card_id_struct))
				{
					p_printf("read %s failed\r\n", user_info_database_path);
					close(fd);
					return -1;
				}
				else
				{
					if (card_array[effective_num].user_id != 0)
					{
						effective_num++;
					}
					p_printf("get user %d success\r\n", i);			
				}
			}	
		}
		/* �ж��û������Ƿ���ȷ */
		if (effective_num != user_num)
		{
			p_printf("init card array failed! user_num is %d  effective_num is %d\r\n", user_num, effective_num);
		}
		else
		{
			/* ʹ�ÿ��������㷨���������� */
			quik_sort(card_array, 0, user_num - 1);
		}
	}
	
	close(fd);
	return 0;
}
