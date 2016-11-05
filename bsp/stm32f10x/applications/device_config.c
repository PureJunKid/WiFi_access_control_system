/**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: device_config.c
**
** ��   ��   ��: �Ž���
**
** �ļ���������: 2016 �� 09 �� 19 ��
**
** ��        ��: �豸�����ļ����

** ��־:
2016.09.19  �������ļ�
*********************************************************************************************************/
#include <rtthread.h>
#include "device_config.h"
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
#ifdef printCfgDebugInfo
#define cfg_printf(fmt,args...) rt_kprintf(fmt, ##args)
#else
#define cfg_printf(fmt,args...)
#endif

/* �豸�����ļ�·�� */
const char *device_config_path = "/config.bin";

struct device_config device_config_t;
/*******************************************************************************
* ������ 	: init_device_config
* ����   	: ��ʼ���豸����
* ����     	: None
* ���     	: None
* ����ֵ    : -1: ʧ�� 0: �ɹ�
*******************************************************************************/
s8 init_device_config(void)
{
	int fd = -1;
	u32 crc = 0;
	
	/* ���豸���� */
	fd = open(device_config_path, O_RDWR|O_CREAT, 0);
	if (fd < 0)
	{
		cfg_printf("open %s failed\r\n", device_config_path);
		close(fd);
		return -1;
	}
	else
	{
		/* �ƶ��ļ�ָ�뵽ָ��λ�� */
		if (lseek(fd, 0, SEEK_SET) == -1)
		{
			cfg_printf("lseek %s failed\r\n", device_config_path);
			close(fd);
			return -1;
		}
		else
		{
			/* ��ȡ�豸������Ϣ */
			read(fd, &device_config_t, sizeof(struct device_config));
			
			/* ����CRC */
			crc = CalcBlockCRC((u8 *)(&device_config_t), sizeof(struct device_config) - 4);
			if (crc != device_config_t.crc)
			{
				cfg_printf("device config crc validators fail, init device config\r\n");
				/* ��ʼ���豸������Ϣ */
				device_config_t.device_addr = 0;
				rt_memset(device_config_t.this_device_name, 0, MEMBER_SIZE(device_config, this_device_name));
				/* ����CRC */
				device_config_t.crc = CalcBlockCRC((u8 *)(&device_config_t), sizeof(struct device_config) - 4);
				
				/* �ƶ��ļ�ָ�뵽ָ��λ�� */
				if (lseek(fd, 0, SEEK_SET) == -1)
				{
					cfg_printf("lseek %s failed\r\n", device_config_path);
					close(fd);
					return -1;
				}
				/* д���豸���� */
				if (write(fd, &device_config_t, sizeof(struct device_config)) != sizeof(struct device_config))
				{
					cfg_printf("write %s failed\r\n", device_config_path);
					close(fd);
					return -1;
				}
				else
				{
					close(fd);
					cfg_printf("init device config success\r\n");			
				}		
			}
			else
			{
				close(fd);
			}
		}
	}
	
	return 0;
}

/*******************************************************************************
* ������ 	: get_set_device_config
* ����   	: ��ȡ/�����豸����
* ����     	: - device_config: �豸���ýṹ�� - cmd: 0: ��ȡ�豸���� 1: �����豸����
* ���     	: None
* ����ֵ    : -1: ʧ�� 0: �ɹ�
*******************************************************************************/
s8 get_set_device_config(struct device_config *device_config_t, u8 cmd)
{
	int fd;
	
	/* �������Ϸ��� */
	if (device_config_t == 0)
	{
		cfg_printf("device_config_t addr is 0\r\n");
		return -1;
	}
	/* ����Ա��Ϣ���ݿ� */
	fd = open(device_config_path, O_RDWR|O_CREAT, 0);
	if (fd < 0)
	{
		cfg_printf("open %s failed\r\n", device_config_path);
		close(fd);
		return -1;
	}
	else
	{
		/* �ƶ��ļ�ָ�뵽ָ��λ�� */
		if (lseek(fd, 0, SEEK_SET) == -1)
		{
			cfg_printf("lseek %s failed\r\n", device_config_path);
			close(fd);
			return -1;
		}
		else
		{
			if (cmd == 0)
			{
				/* ��ȡ���ڼ�¼������ */
				if (read(fd, device_config_t, sizeof(struct device_config)) != sizeof(struct device_config))
				{
					cfg_printf("read %s failed\r\n", device_config_path);
					close(fd);
					return -1;
				}
				else
				{
					close(fd);
					if (device_config_t->crc == CalcBlockCRC((u8 *)device_config_t, sizeof(struct device_config) - 4))
					{
						cfg_printf("get device config addr is %d, name is %s success\r\n",device_config_t->device_addr, device_config_t->this_device_name);
					}
					else
					{
						cfg_printf("crc validators fail addr is %d, name is %s success\r\n",device_config_t->device_addr, device_config_t->this_device_name);
						return -1;
					}
				}		
			}
			else if (cmd == 1)
			{
				/* ����CRC */
				device_config_t->crc = CalcBlockCRC((u8 *)device_config_t, sizeof(struct device_config) - 4);
				/* д�뿼�ڼ�¼������ */
				if (write(fd, device_config_t, sizeof(struct device_config)) != sizeof(struct device_config))
				{
					cfg_printf("write %s failed\r\n", device_config_path);
					close(fd);
					return -1;
				}
				else
				{
					close(fd);
					cfg_printf("set device config addr is %d, name is %s success\r\n",device_config_t->device_addr, device_config_t->this_device_name);
				}		
			}
			else
			{
				close(fd);
				cfg_printf("get set device config cmd unknown\r\n");
				return -1;
			}
		}
	}
	
	return 0;
}
