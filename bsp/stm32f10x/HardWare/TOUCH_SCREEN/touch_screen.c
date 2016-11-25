#include "touch_screen.h" 
#include "ILI93xx.h"
#include "stdlib.h"
#include "math.h"
#include "spi_bus.h"
#include <drivers/spi.h>
#include <rtthread.h>
#include <dfs_posix.h>

#define rt_thread_delayMs(x) rt_thread_delay(rt_tick_from_millisecond(x))

struct rt_spi_device *rt_spi_tp_device;
struct rt_spi_message rt_spi_tp_message;
const char *file_name = "/tp_adj.bin";

_m_tp_dev tp_dev=
{
	TP_Init,
	TP_Scan,
	TP_Adjust,
	0,
	0, 
	0,
	0,
	0,
	0,	  	 		
	0,
	0,	  	 		
};					
//Ĭ��Ϊtouchtype=0������.
u8 CMD_RDX=0XD0;
u8 CMD_RDY=0X90;

u8 tp_readBuf[1],tp_writeBuf[1];
//SPI������ 
//�Ӵ�����IC��ȡadcֵ
//CMD:ָ��
//����ֵ:����������	   
u16 TP_Read_AD(u8 CMD)	  
{ 	 
	u16 Num=0;
	tp_writeBuf[0] = CMD;
	rt_spi_tp_message.send_buf = tp_writeBuf;
	rt_spi_tp_message.recv_buf = tp_readBuf;	//���ö�д����
	rt_spi_tp_message.length = 1;			//���ö�д����
	rt_spi_tp_message.cs_take = 1;			//��ʼͨ��ʱ����CS
	rt_spi_tp_message.cs_release = 0;		//����ͨ��ʱ������CS
	rt_spi_tp_message.next = RT_NULL;
	rt_spi_transfer_message(rt_spi_tp_device, &rt_spi_tp_message);//����һ�����ݴ���
	
	tp_writeBuf[0] = 0x00;
	rt_spi_tp_message.cs_take = 0;			//��ʼͨ��ʱ������CS
	rt_spi_transfer_message(rt_spi_tp_device, &rt_spi_tp_message);//����һ�����ݴ���
	Num = tp_readBuf[0];
	Num <<= 8;
	
	rt_spi_tp_message.cs_release = 1;		//����ͨ��ʱ����CS
	rt_spi_transfer_message(rt_spi_tp_device, &rt_spi_tp_message);//����һ�����ݴ���
	Num += tp_readBuf[0];
	
	Num >>= 3;//ȡ��Чλ
	return(Num);   
}
//��ȡһ������ֵ(x����y)
//������ȡREAD_TIMES������,����Щ������������,
//Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ 
//xy:ָ�CMD_RDX/CMD_RDY��
//����ֵ:����������
#define READ_TIMES 5 	//��ȡ����
#define LOST_VAL 1	  	//����ֵ
u16 TP_Read_XOY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//����
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 
//��ȡx,y����
//��Сֵ��������100.
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
u8 TP_Read_XY(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;			 	 		  
	xtemp=TP_Read_XOY(CMD_RDX);
	ytemp=TP_Read_XOY(CMD_RDY);	  												   
	//if(xtemp<100||ytemp<100)return 0;//����ʧ��
	*x=xtemp;
	*y=ytemp;
	return 1;//�����ɹ�
}
//����2�ζ�ȡ������IC,�������ε�ƫ��ܳ���
//ERR_RANGE,��������,����Ϊ������ȷ,�����������.	   
//�ú����ܴ�����׼ȷ��
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
#define ERR_RANGE 50 //��Χ 
u8 TP_Read_XY2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)return(0);
    flag=TP_Read_XY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//ǰ�����β�����+-50��
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
}  
//////////////////////////////////////////////////////////////////////////////////		  
//��LCD�����йصĺ���  
//��һ��������
//����У׼�õ�
//x,y:����
//color:��ɫ
void TP_Drow_Touch_Point(LCD_TypeDef *TFTLCD, u16 x,u16 y,u16 color)
{
	POINT_COLOR=color;
	LCD_DrawLine(TFTLCD, x-12,y,x+13,y);//����
	LCD_DrawLine(TFTLCD, x,y-12,x,y+13);//����
	LCD_DrawPoint(TFTLCD, x+1,y+1);
	LCD_DrawPoint(TFTLCD, x-1,y+1);
	LCD_DrawPoint(TFTLCD, x+1,y-1);
	LCD_DrawPoint(TFTLCD, x-1,y-1);
	LCD_Draw_Circle(TFTLCD, x,y,6);//������Ȧ
}	  
//��һ�����(2*2�ĵ�)		   
//x,y:����
//color:��ɫ
void TP_Draw_Big_Point(LCD_TypeDef *TFTLCD, u16 x,u16 y,u16 color)
{	    
	POINT_COLOR=color;
	LCD_DrawPoint(TFTLCD, x,y);//���ĵ� 
	LCD_DrawPoint(TFTLCD, x+1,y);
	LCD_DrawPoint(TFTLCD, x,y+1);
	LCD_DrawPoint(TFTLCD, x+1,y+1);	 	  	
}						  
//////////////////////////////////////////////////////////////////////////////////		  
//��������ɨ��
//tp:0,��Ļ����;1,��������(У׼�����ⳡ����)
//����ֵ:��ǰ����״̬.
//0,�����޴���;1,�����д���
u8 TP_Scan(u8 tp)
{			   
	if(PEN==0)//�а�������
	{
		if(tp)TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);//��ȡ��������
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))//��ȡ��Ļ����
		{
	 		tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;//�����ת��Ϊ��Ļ����
			tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff;  
	 	} 
		if((tp_dev.sta&TP_PRES_DOWN)==0)//֮ǰû�б�����
		{		 
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//��������  
			tp_dev.x[4]=tp_dev.x[0];//��¼��һ�ΰ���ʱ������
			tp_dev.y[4]=tp_dev.y[0];  	   			 
		}			   
	}else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//֮ǰ�Ǳ����µ�
		{
			tp_dev.sta&=~(1<<7);//��ǰ����ɿ�	
		}else//֮ǰ��û�б�����
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;//���ص�ǰ�Ĵ���״̬
}	  
//////////////////////////////////////////////////////////////////////////	 
//������EEPROM����ĵ�ַ�����ַ,ռ��14���ֽ�(RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+13)
#define SAVE_ADDR_BASE 40
//����У׼����										    
void TP_Save_Adjdata(void)
{
	int fd, size;
	char buffer[14];
	
	rt_memcpy(buffer, (u8*)&tp_dev.xfac, 13);//��Ҫд������ݴ��뻺��
	buffer[13] = 0x0A;//�����д0X0A���У׼����
	fd = open(file_name, O_RDWR|O_CREAT, 0);
	if (fd >= 0)
	{
		size = write(fd, buffer, 14);//ǿ�Ʊ���&tp_dev.xfac��ַ��ʼ��14���ֽ����ݣ������浽tp_dev.touchtype
		if(size == 14) { rt_kprintf("save %s is success\r\n",file_name); }
		else { rt_kprintf("save %s is failed\r\n",file_name); }
	}
	else
	{
		rt_kprintf("open %s is failed\r\n",file_name);
	}
	close(fd);//�ر��ļ�
}
//�õ�������EEPROM�����У׼ֵ
//����ֵ��1���ɹ���ȡ����
//        0����ȡʧ�ܣ�Ҫ����У׼
u8 TP_Get_Adjdata(void)
{					  
	u8 temp;
	int fd, size;
	char buffer[14];
	
	fd = open(file_name, O_RDWR|O_CREAT, 0);
	if (fd >= 0)
	{
		size = read(fd, buffer, 14);
		if(size == 14)
		{
			temp = buffer[13];//��ȡ�����,���Ƿ�У׼����
			if(temp==0X0A)//�������Ѿ�У׼����			   
			{
				rt_memcpy((u8*)&tp_dev.xfac, buffer, 15);//��ȡ֮ǰ�����У׼���� 
				if(tp_dev.touchtype)//X,Y��������Ļ�෴
				{
					CMD_RDX=0X90;
					CMD_RDY=0XD0;	 
				}else				   //X,Y��������Ļ��ͬ
				{
					CMD_RDX=0XD0;
					CMD_RDY=0X90;
				}
				close(fd);//�ر��ļ�
				return 1;	 
			}
		}
	}
	else
	{
		rt_kprintf("open %s is failed\r\n",file_name);
	}
	close(fd);//�ر��ļ�
	return 0;
}	 
//��ʾ�ַ���
u8* const TP_REMIND_MSG_TBL="Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";
 					  
//��ʾУ׼���(��������)
void TP_Adj_Info_Show(LCD_TypeDef *TFTLCD, u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u16 fac)
{
	_lcd_dev *lcddev;
	
	if (TFTLCD == LCD0)
	{
		lcddev = &lcddev0;
	}
	else
	{
		lcddev = &lcddev1;
	}

	POINT_COLOR=RED;
	LCD_ShowString(TFTLCD, 40,160,lcddev->width,lcddev->height,16,"x1:");
 	LCD_ShowString(TFTLCD, 40+80,160,lcddev->width,lcddev->height,16,"y1:");
 	LCD_ShowString(TFTLCD, 40,180,lcddev->width,lcddev->height,16,"x2:");
 	LCD_ShowString(TFTLCD, 40+80,180,lcddev->width,lcddev->height,16,"y2:");
	LCD_ShowString(TFTLCD, 40,200,lcddev->width,lcddev->height,16,"x3:");
 	LCD_ShowString(TFTLCD, 40+80,200,lcddev->width,lcddev->height,16,"y3:");
	LCD_ShowString(TFTLCD, 40,220,lcddev->width,lcddev->height,16,"x4:");
 	LCD_ShowString(TFTLCD, 40+80,220,lcddev->width,lcddev->height,16,"y4:");  
 	LCD_ShowString(TFTLCD, 40,240,lcddev->width,lcddev->height,16,"fac is:");     
	LCD_ShowNum(TFTLCD, 40+24,160,x0,4,16);		//��ʾ��ֵ
	LCD_ShowNum(TFTLCD, 40+24+80,160,y0,4,16);	//��ʾ��ֵ
	LCD_ShowNum(TFTLCD, 40+24,180,x1,4,16);		//��ʾ��ֵ
	LCD_ShowNum(TFTLCD, 40+24+80,180,y1,4,16);	//��ʾ��ֵ
	LCD_ShowNum(TFTLCD, 40+24,200,x2,4,16);		//��ʾ��ֵ
	LCD_ShowNum(TFTLCD, 40+24+80,200,y2,4,16);	//��ʾ��ֵ
	LCD_ShowNum(TFTLCD, 40+24,220,x3,4,16);		//��ʾ��ֵ
	LCD_ShowNum(TFTLCD, 40+24+80,220,y3,4,16);	//��ʾ��ֵ
 	LCD_ShowNum(TFTLCD, 40+56,240,fac,3,16); 	//��ʾ��ֵ,����ֵ������95~105��Χ֮��.
}
		 
//������У׼����
//�õ��ĸ�У׼����
void TP_Adjust(LCD_TypeDef *TFTLCD)
{								 
	u16 pos_temp[4][2];//���껺��ֵ
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	double fac; 	
	u16 outtime=0;
	_lcd_dev *lcddev;
	
	if (TFTLCD == LCD0)
	{
		lcddev = &lcddev0;
	}
	else
	{
		lcddev = &lcddev1;
	}
 	cnt=0;				
	POINT_COLOR=BLUE;
	BACK_COLOR =WHITE;
	LCD_Clear(TFTLCD, WHITE);//����   
	POINT_COLOR=RED;//��ɫ 
	LCD_Clear(TFTLCD, WHITE);//���� 	   
	POINT_COLOR=BLACK;
	LCD_ShowString(TFTLCD, 40,40,160,100,16,(u8*)TP_REMIND_MSG_TBL);//��ʾ��ʾ��Ϣ
	TP_Drow_Touch_Point(TFTLCD, 20,20,RED);//����1 
	tp_dev.sta=0;//���������ź� 
	tp_dev.xfac=0;//xfac��������Ƿ�У׼��,����У׼֮ǰ�������!�������	 
	while(1)//�������10����û�а���,���Զ��˳�
	{
		tp_dev.scan(1);//ɨ����������
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)//����������һ��(��ʱ�����ɿ���.)
		{	
			outtime=0;		
			tp_dev.sta&=~(1<<6);//��ǰ����Ѿ����������.
						   			   
			pos_temp[cnt][0]=tp_dev.x[0];
			pos_temp[cnt][1]=tp_dev.y[0];
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TP_Drow_Touch_Point(TFTLCD, 20,20,WHITE);				//�����1 
					TP_Drow_Touch_Point(TFTLCD, lcddev->width-20,20,RED);	//����2
					break;
				case 2:
 					TP_Drow_Touch_Point(TFTLCD, lcddev->width-20,20,WHITE);	//�����2
					TP_Drow_Touch_Point(TFTLCD, 20,lcddev->height-20,RED);	//����3
					break;
				case 3:
 					TP_Drow_Touch_Point(TFTLCD, 20,lcddev->height-20,WHITE);			//�����3
 					TP_Drow_Touch_Point(TFTLCD, lcddev->width-20,lcddev->height-20,RED);	//����4
					break;
				case 4:	 //ȫ���ĸ����Ѿ��õ�
	    		    //�Ա����
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,2�ľ���
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�3,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)//���ϸ�
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(TFTLCD, lcddev->width-20,lcddev->height-20,WHITE);	//�����4
   	 					TP_Drow_Touch_Point(TFTLCD, 20,20,RED);								//����1
 						TP_Adj_Info_Show(TFTLCD, pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
 						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,3�ľ���
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(TFTLCD, lcddev->width-20,lcddev->height-20,WHITE);	//�����4
   	 					TP_Drow_Touch_Point(TFTLCD, 20,20,RED);								//����1
 						TP_Adj_Info_Show(TFTLCD, pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
						continue;
					}//��ȷ��
								   
					//�Խ������
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,4�ľ���
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,3�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(TFTLCD, lcddev->width-20,lcddev->height-20,WHITE);	//�����4
   	 					TP_Drow_Touch_Point(TFTLCD, 20,20,RED);								//����1
 						TP_Adj_Info_Show(TFTLCD, pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
						continue;
					}//��ȷ��
					//������
					tp_dev.xfac=(float)(lcddev->width-40)/(pos_temp[1][0]-pos_temp[0][0]);//�õ�xfac		 
					tp_dev.xoff=(lcddev->width-tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//�õ�xoff
						  
					tp_dev.yfac=(float)(lcddev->height-40)/(pos_temp[2][1]-pos_temp[0][1]);//�õ�yfac
					tp_dev.yoff=(lcddev->height-tp_dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//�õ�yoff  
					if(abs(tp_dev.xfac)>2||abs(tp_dev.yfac)>2)//������Ԥ����෴��.
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(TFTLCD, lcddev->width-20,lcddev->height-20,WHITE);	//�����4
   	 					TP_Drow_Touch_Point(TFTLCD, 20,20,RED);								//����1
						LCD_ShowString(TFTLCD, 40,26,lcddev->width,lcddev->height,16,"TP Need readjust!");
						tp_dev.touchtype=!tp_dev.touchtype;//�޸Ĵ�������.
						if(tp_dev.touchtype)//X,Y��������Ļ�෴
						{
							CMD_RDX=0X90;
							CMD_RDY=0XD0;	 
						}else				   //X,Y��������Ļ��ͬ
						{
							CMD_RDX=0XD0;
							CMD_RDY=0X90;	 
						}			    
						continue;
					}		
					POINT_COLOR=BLUE;
					LCD_Clear(TFTLCD, WHITE);//����
					LCD_ShowString(TFTLCD, 35,110,lcddev->width,lcddev->height,16,"Touch Screen Adjust OK!");//У�����
					rt_thread_delayMs(1000);
					TP_Save_Adjdata();  
 					LCD_Clear(TFTLCD, WHITE);//����   
					return;//У�����				 
			}
		}
		rt_thread_delayMs(10);
		outtime++;
		if(outtime>1000)
		{
			TP_Get_Adjdata();
			break;
	 	} 
 	}
}

void tp_attach_device()
{
	static struct rt_spi_device spi_device;
	static struct stm32_spi_cs  spi_cs;
	GPIO_InitTypeDef  GPIO_InitStructure;
	
 	//ʹ��PB,PG�˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);
	//����TP_CS����
    spi_cs.GPIOx = GPIOE;
    spi_cs.GPIO_Pin = GPIO_Pin_2;

	GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;		//tp_cs PB12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO���ٶ�Ϊ50MHz
	GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);		//�����趨������ʼ��GPIOB.12
	GPIO_SetBits(spi_cs.GPIOx,spi_cs.GPIO_Pin);			//PB.12 �����
	//�����ж���������
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;//B6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4

	rt_spi_bus_attach_device(&spi_device, "SPI_TP", "SPI3", (void*)&spi_cs);
}

//��������ʼ��  		    
//����ֵ:0,û�н���У׼
//       1,���й�У׼
u8 TP_Init(LCD_TypeDef *TFTLCD)
{
	/* ��SPI3_BUS�ϸ��Ŵ������豸 */
	tp_attach_device();
	
	rt_spi_tp_device = (struct rt_spi_device *) rt_device_find("SPI_TP");
	if (rt_spi_tp_device == RT_NULL)
	{
		return 0;
	}

	/* config spi */
	{
		struct rt_spi_configuration cfg;
		cfg.data_width = 8;
		cfg.mode = RT_SPI_MODE_0 | RT_SPI_MSB;
		cfg.max_hz = 2 * 1000 * 1000;
		rt_spi_configure(rt_spi_tp_device, &cfg);
	}
	
	TP_Read_XY(&tp_dev.x[0],&tp_dev.y[0]);//��һ�ζ�ȡ��ʼ��	 
	if(TP_Get_Adjdata())return 0;//�Ѿ�У׼
	else			  		//δУ׼?
	{ 										    
		LCD_Clear(TFTLCD, WHITE);	//����
		TP_Adjust(TFTLCD);  		//��ĻУ׼  
	}			
	TP_Get_Adjdata();	
	return 1; 									 
}

