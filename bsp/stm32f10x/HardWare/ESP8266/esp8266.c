#include "esp8266.h"
#include "sys.h"
#include "spi_bus.h"
#include <drivers/spi.h>
#include <rtthread.h>
#include "finsh.h"

#define delay_ms(ms) rt_thread_delay(rt_tick_from_millisecond(ms))

#define start_bit (1 << 7)
#define end_bit (1 << 6)

struct rt_spi_device *rt_spi_esp8266_device;//esp8266�豸
struct rt_spi_message esp8266_message;	//SPI�豸ͨ������Ϣ�ṹ��

u8 wr_rdy = 1, rd_rdy = 0;

/* esp8266�¼����ƿ� */
struct rt_event esp8266_event;

/*******************************************************************************
* ������ 	: esp8266_spi_transmit
* ����   	: SPI���亯��
* ����     	: - cmd: ����ָ��
*			    MASTER_WRITE_DATA_TO_SLAVE_CMD    2
*			    MASTER_READ_DATA_FROM_SLAVE_CMD   3
*			    MASTER_WRITE_STATUS_TO_SLAVE_CMD  1
*			    MASTER_READ_STATUS_FROM_SLAVE_CMD 4
*             - addr: ��ַ��ͨ��Ϊ0
*             - buf: ������������д�����ݻ򴫳���������
* ���     	: None
* ����ֵ    : None
*******************************************************************************/
void esp8266_spi_transmit(u8 cmd, u8 addr, u8 *buf)
{
	static u8 esp8266_readBuf[34],esp8266_writeBuf[34];	//SPIͨ�Ż���
	int i;
	
	/* ������Χ��� */
	if (((cmd != 2)&&(cmd != 3))||(buf == NULL))
	{
		return;
	}
	esp8266_writeBuf[0] = cmd;
	esp8266_writeBuf[1] = addr;
	/* 0x02 д����  0x03������ */
	if (cmd == 0x02)
	{
		for (i = 0; i < 32; i++) { esp8266_writeBuf[i + 2] = buf[i]; }
	}
	else
	{
		for (i = 0; i < 32; i++) { esp8266_writeBuf[i + 2] = 0; }
	}
		
	rt_memset(esp8266_readBuf,0,sizeof(esp8266_readBuf));
	esp8266_message.send_buf = esp8266_writeBuf;
	esp8266_message.recv_buf = esp8266_readBuf;	//���ö�д����
	esp8266_message.length = 34;				//���ö�д����
	esp8266_message.cs_take = 1;				//��ʼͨ��ʱ����CS
	esp8266_message.cs_release = 1;				//����ͨ��ʱ����CS
	esp8266_message.next = RT_NULL;
	rt_spi_transfer_message(rt_spi_esp8266_device, &esp8266_message);//����һ�����ݴ���
	
	/* ����ȡ��������д��buf�� */
	if (cmd == 0x03)
	{
		for (i = 0; i < 32; i++) { buf[i] = esp8266_readBuf[i + 2]; }
	}
}

/*******************************************************************************
* ������ 	: esp8266_spi_write
* ����   	: SPI���亯��
*             ���ֽڹ̶�Ϊcommand byte�����ڴ���31�ֽ����ݰ��ķְ��������
*             11xx xxxx ��λ����Ϊ���ݷְ���־���װ�1xxx xxxx��ĩ��x1xx xxxx��
*             ����λ���ã���ʾ���м�����ݷְ���xx11 1111 ��6λ���ݣ���ʾ��ǰ���ĳ���
* ����     	: - cmd: ����ָ��
* ���     	: None
* ����ֵ    : -1: ʧ�� 0: �ɹ�
*******************************************************************************/
s8 esp8266_spi_write(u8 *pack, u32 lenth)
{
	int i,j,t = 50000;
	u8 buf[32];
	int num = lenth/31 + 1;		//����
	int last_byte = lenth%31;	//���һ����Ч�ֽ���
	
	if (last_byte == 0)
	{
		num--;
		last_byte = 31;
	}
	/* �������Ϸ��� */
	if (pack == NULL)
	{
		return -1;
	}
	
	for (i = 0; i < num; i++)
	{
		buf[0] = 0;                              	/* ���command byte */
		/* ����command byte */
		if (i != num - 1)        					/* �����Ϊ��ĩ��,���õ�ǰ������ */
		{
			buf[0] = 31; 
		}
		else                              			/* ���õ�ǰ�����Ȳ����ĩ����־λ */
		{
			buf[0] = (end_bit | last_byte);
		}
		if (i == 0) { buf[0] |= start_bit;}     	/* ����װ���־λ */
		
		/* �����ݸ��Ƶ�����buf�� */
		if(i == num - 1) 
		{
			for (j = 0; j < last_byte; j++) 
			{
				buf[j + 1] = pack[i*31 + j]; 
			}
		}
		else
		{
			for (j = 0; j < 31; j++) 
			{
				buf[j + 1] = pack[i*31 + j];
			}
		}
		
		/* �ȴ�����׼���ñ�־λ */
		while((wr_rdy != 1)&&(t != 0))
		{
			t--;
		}
		wr_rdy = 0;
		if (t == 0)
		{
			return -1; /* �ȴ���ʱ�����ش��� */
		}
		/* ����һ��SPI���� */
		esp8266_spi_transmit(0x02, 0, buf);	
	}
	
	return 0;
}

s8 WriteTest(u32 lenth)
{
	u8 buf[1024*5];
	int i;
	
	for (i = 0; i < sizeof(buf); i++)
	{
		buf[i] = (u8)i;
	}
	return esp8266_spi_write(buf,lenth);
}
FINSH_FUNCTION_EXPORT(WriteTest, WiFiWriteTest)

u8 recv_pack[1024*5];
u8 is_recv_pack = 0;
u32 recv_lenth = 0;
/*******************************************************************************
* ������ 	: esp8266_spi_read
* ����   	: SPI���պ���, �ɽ����̵߳���, ��ֹ�����ط�����
* ����     	: None
* ���     	: None
* ����ֵ    : -1: ����δ��� 0: �������
*******************************************************************************/
s8 esp8266_spi_read(void)
{
	static int i = 0;
	static u8 buf[32];
	static u8 isReceive = 0;
	static u32 pack_counter = 0;
	
	/* ����һ��SPI���� */
	esp8266_spi_transmit(0x03, 0, buf);
	
	if (is_recv_pack == 0)
	{
		/* �����װ� */
		if (( buf[0] & start_bit) == start_bit)
		{
			i = 0;
			is_recv_pack = 0;
			isReceive = 1;
			pack_counter = 0;
			recv_lenth = 0;
		}
		/* �������װ�֮��ʼ�������� */
		if (isReceive)
		{
			recv_lenth += buf[0] & 0x3f;                    /* ��¼���յ����ֽ��� */
			if (( buf[0] & end_bit) == end_bit)             /* �ж��Ƿ��յ�ĩ�� */
			{
				for (i = 0; i < (buf[0] & 0x3f); i++)
				{
					recv_pack[pack_counter*31 + i] = buf[i + 1];
				}
				isReceive = 0;
				is_recv_pack = 1;                           /* �յ�ĩ��֮��ֹͣ�������ݲ���λ������ɱ�־ */
			}
			else
			{
				for (i = 0; i < 31; i++)
				{
					recv_pack[pack_counter*31 + i] = buf[i + 1];
				}
				pack_counter++;                             /* ��¼���յ��İ��� */
			}
		}	
		/* ������� */
		if (is_recv_pack == 1)
		{
			return 0;
		}
		i++;
	}
	
	return -1;
}

/*******************************************************************************
* ������ 	: esp8266_cs_cfg
* ����   	: ��λESP8266
* ����     	: None
* ���     	: None
* ����ֵ    : None
*******************************************************************************/
void esp8266_io_cfg(u8 io, u8 mode)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//ʹ�ܶ˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                       /* IO���ٶ�Ϊ50MHz */
	if(mode == ENABLE) { GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; } /* ������� */
	else { GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; }          /* ģ������ */	
	switch (io)
	{
		case EN:
		{
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;                               /* esp8266_en */
			GPIO_Init(GPIOC, &GPIO_InitStructure);                                  /* �����趨������ʼ�� */	
		} break;
		case RST:
		{
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;                               /* esp8266_rst */
			GPIO_Init(GPIOB, &GPIO_InitStructure);                                  /* �����趨������ʼ�� */	
		} break;
		case CS:
		{
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                               /* esp8266_cs */
			GPIO_Init(GPIOA, &GPIO_InitStructure);                                  /* �����趨������ʼ�� */	
		} break;
		case BOOT:
		{
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                               /* esp8266_boot */
			GPIO_Init(GPIOC, &GPIO_InitStructure);                                  /* �����趨������ʼ�� */	
		} break;
		default: break;
	}
}

/*******************************************************************************
* ������ 	: esp8266_boot
* ����   	: ʹESP8266����boot(1,6), ��¼����
* ����     	: None
* ���     	: None
* ����ֵ    : None
*******************************************************************************/
void esp8266_boot(void)
{
	/* �ı����ŵ���Ҫ��״̬ */
	esp8266_io_cfg(BOOT, ENABLE);
	esp8266_io_cfg(EN, ENABLE);
	esp8266_io_cfg(RST, ENABLE);
	esp8266_io_cfg(CS, DISABLE);
	ESP8266_EN = 0;
	ESP8266_RST = 0;
	ESP8266_BOOT = 0;
	delay_ms(5);
	ESP8266_EN = 1;
	ESP8266_RST = 1;
	delay_ms(200);
	ESP8266_BOOT = 1;
	esp8266_io_cfg(CS, ENABLE);
	esp8266_io_cfg(RST, DISABLE);
	esp8266_io_cfg(EN, DISABLE);
	esp8266_io_cfg(BOOT, DISABLE);
}
FINSH_FUNCTION_EXPORT(esp8266_boot, esp8266_reset)

/*******************************************************************************
* ������ 	: esp8266_reset
* ����   	: ��λESP8266
* ����     	: None
* ���     	: None
* ����ֵ    : None
*******************************************************************************/
void esp8266_reset(void)
{
	/* �ı����ŵ���Ҫ��״̬ */
	esp8266_io_cfg(EN, ENABLE);
	esp8266_io_cfg(RST, ENABLE);
	esp8266_io_cfg(CS, DISABLE);
	ESP8266_EN = 0;
	ESP8266_RST = 0;
	delay_ms(5);
	/* ��λ */
	ESP8266_EN = 1;
	ESP8266_RST = 1;
	delay_ms(200);
	/* �ָ�����״̬ */
	esp8266_io_cfg(CS, ENABLE);
	esp8266_io_cfg(RST, DISABLE);
	esp8266_io_cfg(EN, DISABLE);
}
FINSH_FUNCTION_EXPORT(esp8266_reset, esp8266_reset)

/*******************************************************************************
* ������ 	: esp8266_exti_init
* ����   	: spi˫��͸��Э���е��ж��߳�ʼ��
* ����     	: None
* ���     	: None
* ����ֵ    : None
*******************************************************************************/
void esp8266_exti_init(void)
{
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��
	
    //GPIOB.1 �ж����Լ��жϳ�ʼ������ �����ش���
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);

  	EXTI_InitStructure.EXTI_Line = EXTI_Line1;	//GOIO5
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

    //GPIOF.7 �ж����Լ��жϳ�ʼ������ �����ش���
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOF,GPIO_PinSource7);
	
  	EXTI_InitStructure.EXTI_Line = EXTI_Line7;	//GOIO4
  	EXTI_Init(&EXTI_InitStructure);	  	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;				//ʹ��GOIO5���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;			//�����ȼ�3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;				//ʹ��GOIO4���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;			//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);
}

void esp8266_attach_device()
{
	static struct rt_spi_device esp8266_spi_device;
	static struct stm32_spi_cs  esp8266_spi_cs;
	GPIO_InitTypeDef  GPIO_InitStructure;
	
 	//ʹ�ܶ˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//����RC522_IN_CS����
    esp8266_spi_cs.GPIOx = GPIOA;
    esp8266_spi_cs.GPIO_Pin = GPIO_Pin_4;

	GPIO_InitStructure.GPIO_Pin = esp8266_spi_cs.GPIO_Pin;      /* esp8266_cs */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;            /* ������� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;           /* IO���ٶ�Ϊ50MHz */
	GPIO_Init(esp8266_spi_cs.GPIOx, &GPIO_InitStructure);       /* �����趨������ʼ�� */
	GPIO_SetBits(esp8266_spi_cs.GPIOx,esp8266_spi_cs.GPIO_Pin); /* ����� */
	//�����豸��SPI����
	rt_spi_bus_attach_device(&esp8266_spi_device, "ESP8266", "SPI1", (void*)&esp8266_spi_cs);
	/* ��λesp8266 */
	esp8266_reset();
}

s8 init_esp8266(void)
{
	/* ��SPI1_BUS�ϸ���esp8266�豸 */
	esp8266_attach_device();
	/* ����esp8266�豸 */
	rt_spi_esp8266_device = (struct rt_spi_device *) rt_device_find("ESP8266");//�����豸
	if (rt_spi_esp8266_device == RT_NULL)
	{
		return -1;
	}

	/* config spi */
	{
		struct rt_spi_configuration cfg;
		cfg.data_width = 8;
		cfg.mode = RT_SPI_MODE_0 | RT_SPI_MSB;
		cfg.max_hz = 0.1 * 1000 * 1000;
		rt_spi_configure(rt_spi_esp8266_device, &cfg);
	}
	/* ��ʼ���ж��� */
	esp8266_exti_init();
	
	return 0;
}

/* GPIO4�������ж� */
void EXTI9_5_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line7) == SET)
	{
		/* �����������ݴ�����ϣ����Խ�����һ��д�� */
		wr_rdy = 1;
		/* ���LINE7�ϵ��жϱ�־λ */
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
}

/* GPIO5�������ж� */
void EXTI1_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line1) == SET)
	{
		/* �ӻ����·��ͻ��棬�������Զ�ȡ */
		rd_rdy = 1;
		/* ����hspi�����¼� */
		rt_event_send(&esp8266_event, hspi_rx);
		/* ���LINE4�ϵ��жϱ�־λ */
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}
