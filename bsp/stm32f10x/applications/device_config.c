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

/* �豸��ַ */
u8 device_addr = 0;
/* �豸���� */
char this_device_name[64];

/*******************************************************************************
* ������ 	: get_set_device_addr
* ����   	: ��ȡ/�����豸��ַ
* ����     	: - device_addr: �豸��ַ - cmd: 0: ��ȡ�豸��ַ 1: �����豸��ַ
* ���     	: None
* ����ֵ    : -1: ʧ�� 0: �ɹ�
*******************************************************************************/
s8 get_set_device_addr(u8 *device_addr, u8 cmd)
{
	int fd;
	/* ���豸�����ļ� */
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
				/* ��ȡ�豸��ַ */
				if (read(fd, device_addr, 1) != 1)
				{
					cfg_printf("read %s failed\r\n", device_config_path);
					close(fd);
					return -1;
				}
				else
				{
					close(fd);
					cfg_printf("get device addr %d success\r\n", *device_addr);			
				}		
			}
			else if (cmd == 1)
			{
				/* д���豸��ַ */
				if (write(fd, device_addr, 1) != 1)
				{
					cfg_printf("write %s failed\r\n", device_config_path);
					close(fd);
					return -1;
				}
				else
				{
					close(fd);
					cfg_printf("set device addr %d success\r\n", *device_addr);			
				}		
			}
			else
			{
				close(fd);
				cfg_printf("set device addr cmd unknown\r\n");
				return -1;
			}
		}
	}
	
	close(fd);
	return 0;
}


/*******************************************************************************
* ������ 	: get_set_device_name
* ����   	: ��ȡ/�����豸����
* ����     	: - device_name: �豸���� - cmd: 0: ��ȡ�豸���� 1: �����豸����
* ���     	: None
* ����ֵ    : -1: ʧ�� 0: �ɹ�
*******************************************************************************/
s8 get_set_device_name(char *device_name, u8 cmd)
{
	int fd;
	/* ���豸�����ļ� */
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
		if (lseek(fd, 1, SEEK_SET) == -1)
		{
			cfg_printf("lseek %s failed\r\n", device_config_path);
			close(fd);
			return -1;
		}
		else
		{
			if (cmd == 0)
			{
				/* ��ȡ�豸���� */
				if (read(fd, device_name, 64) != 64)
				{
					cfg_printf("read %s failed\r\n", device_config_path);
					close(fd);
					return -1;
				}
				else
				{
					close(fd);
					cfg_printf("get device name %s success\r\n", device_name);			
				}		
			}
			else if (cmd == 1)
			{
				/* д���豸���� */
				if (write(fd, device_name, 64) != 64)
				{
					cfg_printf("write %s failed\r\n", device_config_path);
					close(fd);
					return -1;
				}
				else
				{
					close(fd);
					cfg_printf("set device name %s success\r\n", device_name);			
				}		
			}
			else
			{
				close(fd);
				cfg_printf("set device name cmd unknown\r\n");
				return -1;
			}
		}
	}
	
	return 0;
}

