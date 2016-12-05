#include "ILI93xx.h"
#include "stdlib.h"
#include "font.h" 
#include "usart.h"	 
#include "pwm.h"
#include <rtthread.h>

#define rt_thread_delayMs(x) rt_thread_delay(rt_tick_from_millisecond(x))

void DelayUs(u32 dwTime)
{
    u32 dwCurCounter=0;                                //��ǰʱ�����ֵ
    u32 dwPreTickVal=SysTick->VAL;                     //��һ��SYSTICK����ֵ
    u32 dwCurTickVal;                                  //��һ��SYSTICK����ֵ
    dwTime=dwTime*(72000000/1000000);    //����ʱʱ�䣬������ʱ�����
    for(;;){
        dwCurTickVal=SysTick->VAL;
        if(dwCurTickVal<dwPreTickVal){
            dwCurCounter=dwCurCounter+dwPreTickVal-dwCurTickVal;
        }
        else{
            dwCurCounter=dwCurCounter+dwPreTickVal+SysTick->LOAD-dwCurTickVal;
        }
        dwPreTickVal=dwCurTickVal;
        if(dwCurCounter>=dwTime){
            return;
        }
    }
}

//LCD�Ļ�����ɫ�ͱ���ɫ	   
u16 POINT_COLOR=0x0000;	//������ɫ
u16 BACK_COLOR=0xFFFF;  //����ɫ 
  
//����LCD��Ҫ����
//Ĭ��Ϊ����
_lcd_dev lcddev0;
_lcd_dev lcddev1;
	 
//д�Ĵ�������
//regval:�Ĵ���ֵ
void LCD_WR_REG(LCD_TypeDef *TFTLCD, u16 regval)
{
	TFTLCD->LCD_REG=regval;//д��Ҫд�ļĴ������	 
}
//дLCD����
//data:Ҫд���ֵ
void LCD_WR_DATA(LCD_TypeDef *TFTLCD, u16 data)
{
	TFTLCD->LCD_RAM=data;		 
}
//��LCD����
//����ֵ:������ֵ
u16 LCD_RD_DATA(LCD_TypeDef *TFTLCD)
{
	vu16 ram;			//��ֹ���Ż�
	ram=TFTLCD->LCD_RAM;	
	return ram;	 
}					   
//д�Ĵ���
//LCD_Reg:�Ĵ�����ַ
//LCD_RegValue:Ҫд�������
void LCD_WriteReg(LCD_TypeDef *TFTLCD, u16 LCD_Reg,u16 LCD_RegValue)
{
	TFTLCD->LCD_REG = LCD_Reg;		//д��Ҫд�ļĴ������	 
	TFTLCD->LCD_RAM = LCD_RegValue;//д������	    		 
}	   
//���Ĵ���
//LCD_Reg:�Ĵ�����ַ
//����ֵ:����������
u16 LCD_ReadReg(LCD_TypeDef *TFTLCD, u16 LCD_Reg)
{
	LCD_WR_REG(TFTLCD, LCD_Reg);		//д��Ҫ���ļĴ������
	//DelayUs(5);		  
	return LCD_RD_DATA(TFTLCD);		//���ض�����ֵ
}
//��ʼдGRAM
void LCD_WriteRAM_Prepare(LCD_TypeDef *TFTLCD)
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
 	TFTLCD->LCD_REG=lcddev->wramcmd;	  
}	 
//LCDдGRAM
//RGB_Code:��ɫֵ
void LCD_WriteRAM(LCD_TypeDef *TFTLCD, u16 RGB_Code)
{							    
	TFTLCD->LCD_RAM = RGB_Code;//дʮ��λGRAM
}
//��ILI93xx����������ΪGBR��ʽ��������д���ʱ��ΪRGB��ʽ��
//ͨ���ú���ת��
//c:GBR��ʽ����ɫֵ
//����ֵ��RGB��ʽ����ɫֵ
u16 LCD_BGR2RGB(u16 c)
{
	u16  r,g,b,rgb;   
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);
} 
//��mdk -O1ʱ���Ż�ʱ��Ҫ����
//��ʱi
void opt_delay(u8 i)
{
	while(i--);
}

//��ȡ��ĳ�����ɫֵ	 
//x,y:����
//����ֵ:�˵����ɫ
u16 LCD_ReadPoint(LCD_TypeDef *TFTLCD, u16 x,u16 y)
{
 	u16 r=0;
	_lcd_dev *lcddev;
	
	if (TFTLCD == LCD0)
	{
		lcddev = &lcddev0;
	}
	else
	{
		lcddev = &lcddev1;
	}
	
	if(x>=lcddev->width||y>=lcddev->height)return 0;	//�����˷�Χ,ֱ�ӷ���		   
	LCD_SetCursor(TFTLCD, x,y);	    
	LCD_WR_REG(TFTLCD, 0X22);      		 			//����IC���Ͷ�GRAMָ��
 	r=LCD_RD_DATA(TFTLCD);								//dummy Read	   
	opt_delay(2);	  
 	r=LCD_RD_DATA(TFTLCD);  		  						//ʵ��������ɫ
	
	return LCD_BGR2RGB(r);						//����IC
}			 

//���ù��λ��
//Xpos:������
//Ypos:������
void LCD_SetCursor(LCD_TypeDef *TFTLCD, u16 Xpos, u16 Ypos)
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

	if(lcddev->dir==1)Xpos=lcddev->width-1-Xpos;//������ʵ���ǵ�תx,y����
	LCD_WriteReg(TFTLCD, lcddev->setxcmd, Xpos);
	LCD_WriteReg(TFTLCD, lcddev->setycmd, Ypos);
}

//����LCD���Զ�ɨ�跽��
//ע��:�����������ܻ��ܵ��˺������õ�Ӱ��(������9341/6804����������),
//����,һ������ΪL2R_U2D����,�������Ϊ����ɨ�跽ʽ,���ܵ�����ʾ������.
//dir:0~7,����8������(���嶨���lcd.h)
//9320/9325/9328/4531/4535/1505/b505/5408/9341/5310/5510/1963��IC�Ѿ�ʵ�ʲ���	   	   
void LCD_Scan_Dir(LCD_TypeDef *TFTLCD, u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	
	switch(dir)
	{
		case L2R_U2D://������,���ϵ���
			regval|=(1<<5)|(1<<4)|(0<<3); 
			break;
		case L2R_D2U://������,���µ���
			regval|=(0<<5)|(1<<4)|(0<<3); 
			break;
		case R2L_U2D://���ҵ���,���ϵ���
			regval|=(1<<5)|(0<<4)|(0<<3);
			break;
		case R2L_D2U://���ҵ���,���µ���
			regval|=(0<<5)|(0<<4)|(0<<3); 
			break;	 
		case U2D_L2R://���ϵ���,������
			regval|=(1<<5)|(1<<4)|(1<<3); 
			break;
		case U2D_R2L://���ϵ���,���ҵ���
			regval|=(1<<5)|(0<<4)|(1<<3); 
			break;
		case D2U_L2R://���µ���,������
			regval|=(0<<5)|(1<<4)|(1<<3); 
			break;
		case D2U_R2L://���µ���,���ҵ���
			regval|=(0<<5)|(0<<4)|(1<<3); 
			break;	 
	} 
	dirreg=0X03;
	regval|=1<<12; 
	LCD_WriteReg(TFTLCD, dirreg,regval);
}

//����
//x,y:����
//POINT_COLOR:�˵����ɫ
void LCD_DrawPoint(LCD_TypeDef *TFTLCD, u16 x,u16 y)
{
	LCD_SetCursor(TFTLCD, x,y);		//���ù��λ�� 
	LCD_WriteRAM_Prepare(TFTLCD);	//��ʼд��GRAM
	TFTLCD->LCD_RAM=POINT_COLOR; 
}

//���ٻ���
//x,y:����
//color:��ɫ
void LCD_Fast_DrawPoint(LCD_TypeDef *TFTLCD, u16 x,u16 y,u16 color)
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
	
	if(lcddev->dir==1)x=lcddev->width-1-x;//������ʵ���ǵ�תx,y����
	LCD_WriteReg(TFTLCD, lcddev->setxcmd,x);
	LCD_WriteReg(TFTLCD, lcddev->setycmd,y);
	
	TFTLCD->LCD_REG=lcddev->wramcmd; 
	TFTLCD->LCD_RAM=color; 
}

//����LCD��ʾ����
//dir:0,������1,����
void LCD_Display_Dir(LCD_TypeDef *TFTLCD, u8 dir)
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

	if(dir==0)			//����
	{
		lcddev->dir=0;	//����
		lcddev->width=240;
		lcddev->height=320;
		lcddev->wramcmd=0X22;
		lcddev->setxcmd=0X20;
		lcddev->setycmd=0X21;  
	}else 				//����
	{	  				
		lcddev->dir=1;	//����
		lcddev->width=320;
		lcddev->height=240;
		lcddev->wramcmd=0X22;
		lcddev->setxcmd=0X21;
		lcddev->setycmd=0X20;  
	} 
	LCD_Scan_Dir(TFTLCD, DFT_SCAN_DIR);	//Ĭ��ɨ�跽��
}	 

//��ʼ��lcd
//�ó�ʼ���������Գ�ʼ������ILI93XXҺ��,�������������ǻ���ILI9320��!!!
//�������ͺŵ�����оƬ��û�в���! 
void TFTLCD_Init(void)
{
	_lcd_dev *lcddev = 0;
	LCD_TypeDef *TFTLCD = 0;	
 	GPIO_InitTypeDef GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;

	rt_hw_lcd_led_init(14400-1,1-1);	 //1��Ƶ��PWMƵ��=72000000/14400=5Khz
	set_lcd_led(1, 0);	//�ر�����Һ������

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);		 //ʹ��FSMCʱ��
	//ʹ��PORTC,D,E,F,G�Լ�AFIO���ù���ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG|RCC_APB2Periph_AFIO,ENABLE);

 	//PORTD�����������  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15;  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //�����������   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	  
	//PORTE�����������  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //�����������   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOE, &GPIO_InitStructure); 
	  
	//PORTF�����������  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //�����������   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOF, &GPIO_InitStructure); 
	  
   	//PORTG�����������	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;    /* NE2, NE3 */ 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //�����������   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOG, &GPIO_InitStructure); 
 
	readWriteTiming.FSMC_AddressSetupTime = 0x01;	 //��ַ����ʱ�䣨ADDSET��Ϊ2��HCLK 1/36M=27ns
    readWriteTiming.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�	
    readWriteTiming.FSMC_DataSetupTime = 0x0f;		 // ���ݱ���ʱ��Ϊ16��HCLK,��ΪҺ������IC�Ķ����ݵ�ʱ���ٶȲ���̫�죬�����1289���IC��
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA 
    
	writeTiming.FSMC_AddressSetupTime = 0x00;	 //��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK  
    writeTiming.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ�䣨A		
    writeTiming.FSMC_DataSetupTime = 0x03;		 ////���ݱ���ʱ��Ϊ4��HCLK	
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA 

    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; // ���������ݵ�ַ
    FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM   
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//�洢�����ݿ��Ϊ16bit   
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//  �洢��дʹ��
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // ��дʹ�ò�ͬ��ʱ��
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; //��дʱ��
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  //дʱ��

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM2; /* NE2, LCD_CS_IN */
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);              /* ��ʼ��FSMC���� */
   	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM2, ENABLE);              /* ʹ��BANK2 */

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3; /* NE3, LCD_CS_OUT */
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);              /* ��ʼ��FSMC���� */
   	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);              /* ʹ��BANK3 */
	
	rt_thread_delayMs(50); 					// delay 50 ms
	for (u8 i = 0; i < 2; i++)
	{
		if (i == 0)
		{
			TFTLCD = LCD0;
			lcddev = &lcddev0;
		}
		else
		{
			TFTLCD = LCD1;
			lcddev = &lcddev1;
		}
		
		lcddev->id=LCD_ReadReg(TFTLCD, 0x0000);	//��ID��9320/9325/9328/4531/4535��IC��   
		if(lcddev->id==0x9328)//ILI9328   OK  
		{
			LCD_WriteReg(TFTLCD, 0x00EC,0x108F);// internal timeing      
			LCD_WriteReg(TFTLCD, 0x00EF,0x1234);// ADD        
			LCD_WriteReg(TFTLCD, 0x0001,0x0100);     
			LCD_WriteReg(TFTLCD, 0x0002,0x0700);//��Դ����                    
			LCD_WriteReg(TFTLCD, 0x0003,(1<<12)|(3<<4)|(0<<3) );//65K    
			LCD_WriteReg(TFTLCD, 0x0004,0x0000);                                   
			LCD_WriteReg(TFTLCD, 0x0008,0x0202);	           
			LCD_WriteReg(TFTLCD, 0x0009,0x0000);         
			LCD_WriteReg(TFTLCD, 0x000a,0x0000);//display setting         
			LCD_WriteReg(TFTLCD, 0x000c,0x0001);//display setting          
			LCD_WriteReg(TFTLCD, 0x000d,0x0000);//0f3c          
			LCD_WriteReg(TFTLCD, 0x000f,0x0000);
			//��Դ����
			LCD_WriteReg(TFTLCD, 0x0010,0x0000);   
			LCD_WriteReg(TFTLCD, 0x0011,0x0007);
			LCD_WriteReg(TFTLCD, 0x0012,0x0000);                                                                 
			LCD_WriteReg(TFTLCD, 0x0013,0x0000);                 
			LCD_WriteReg(TFTLCD, 0x0007,0x0001);                 
			rt_thread_delayMs(50); 
			LCD_WriteReg(TFTLCD, 0x0010,0x1490);   
			LCD_WriteReg(TFTLCD, 0x0011,0x0227);
			rt_thread_delayMs(50); 
			LCD_WriteReg(TFTLCD, 0x0012,0x008A);                  
			rt_thread_delayMs(50); 
			LCD_WriteReg(TFTLCD, 0x0013,0x1a00);   
			LCD_WriteReg(TFTLCD, 0x0029,0x0006);
			LCD_WriteReg(TFTLCD, 0x002b,0x000d);
			rt_thread_delayMs(50); 
			LCD_WriteReg(TFTLCD,0x0020,0x0000);                                                            
			LCD_WriteReg(TFTLCD,0x0021,0x0000);           
			rt_thread_delayMs(50); 
			//٤��У��
			LCD_WriteReg(TFTLCD, 0x0030,0x0000); 
			LCD_WriteReg(TFTLCD, 0x0031,0x0604);   
			LCD_WriteReg(TFTLCD, 0x0032,0x0305);
			LCD_WriteReg(TFTLCD, 0x0035,0x0000);
			LCD_WriteReg(TFTLCD, 0x0036,0x0C09); 
			LCD_WriteReg(TFTLCD, 0x0037,0x0204);
			LCD_WriteReg(TFTLCD, 0x0038,0x0301);        
			LCD_WriteReg(TFTLCD, 0x0039,0x0707);     
			LCD_WriteReg(TFTLCD, 0x003c,0x0000);
			LCD_WriteReg(TFTLCD, 0x003d,0x0a0a);
			rt_thread_delayMs(50); 
			LCD_WriteReg(TFTLCD, 0x0050,0x0000); //ˮƽGRAM��ʼλ�� 
			LCD_WriteReg(TFTLCD, 0x0051,0x00ef); //ˮƽGRAM��ֹλ��                    
			LCD_WriteReg(TFTLCD, 0x0052,0x0000); //��ֱGRAM��ʼλ��                    
			LCD_WriteReg(TFTLCD, 0x0053,0x013f); //��ֱGRAM��ֹλ��  

			LCD_WriteReg(TFTLCD, 0x0060,0xa700);        
			LCD_WriteReg(TFTLCD, 0x0061,0x0001); 
			LCD_WriteReg(TFTLCD, 0x006a,0x0000);
			LCD_WriteReg(TFTLCD, 0x0080,0x0000);
			LCD_WriteReg(TFTLCD, 0x0081,0x0000);
			LCD_WriteReg(TFTLCD, 0x0082,0x0000);
			LCD_WriteReg(TFTLCD, 0x0083,0x0000);
			LCD_WriteReg(TFTLCD, 0x0084,0x0000);
			LCD_WriteReg(TFTLCD, 0x0085,0x0000);

			LCD_WriteReg(TFTLCD, 0x0090,0x0010);     
			LCD_WriteReg(TFTLCD, 0x0092,0x0600);  
			//������ʾ����     
			LCD_WriteReg(TFTLCD, 0x0007,0x0133); 
		} 
		LCD_Display_Dir(TFTLCD, 1);	//EMWINʵ��Ĭ������Ϊ����
		LCD_Clear(TFTLCD, BLACK);		//��ʾȫ��
	}
	set_lcd_led(1, 250);	//��������Һ������
	set_lcd_led(0, 250);	//��������Һ������
}

//��������
//color:Ҫ���������ɫ
void LCD_Clear(LCD_TypeDef *TFTLCD, u16 color)
{
	u32 index=0;      
	u32 totalpoint;
	_lcd_dev *lcddev;
	
	if (TFTLCD == LCD0)
	{
		lcddev = &lcddev0;
	}
	else
	{
		lcddev = &lcddev1;
	}
	
	totalpoint=lcddev->width;
	totalpoint*=lcddev->height; 			//�õ��ܵ���
 	LCD_SetCursor(TFTLCD, 0x00,0x0000);	//���ù��λ�� 
	LCD_WriteRAM_Prepare(TFTLCD);     		//��ʼд��GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
		TFTLCD->LCD_RAM=color;	
	}
}  
//��ָ����������䵥����ɫ
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
//color:Ҫ������ɫ
void LCD_Fill(LCD_TypeDef *TFTLCD, u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
	
	xlen=ex-sx+1;	 
	for(i=sy;i<=ey;i++)
	{
		LCD_SetCursor(TFTLCD, sx,i);      				//���ù��λ�� 
		LCD_WriteRAM_Prepare(TFTLCD);     			//��ʼд��GRAM	  
		for(j=0;j<xlen;j++)TFTLCD->LCD_RAM=color;	//��ʾ��ɫ 	    
	}
}
//��ָ�����������ָ����ɫ��			 
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
//color:Ҫ������ɫ
void LCD_Color_Fill(LCD_TypeDef *TFTLCD, u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
	u16 height,width;
	u16 i,j;
	width=ex-sx+1; 			//�õ����Ŀ��
	height=ey-sy+1;			//�߶�
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(TFTLCD, sx,sy+i);   	//���ù��λ�� 
		LCD_WriteRAM_Prepare(TFTLCD);     //��ʼд��GRAM
		for(j=0;j<width;j++)TFTLCD->LCD_RAM=color[i*width+j];//д������ 
	}		  
}  
//����
//x1,y1:�������
//x2,y2:�յ�����  
void LCD_DrawLine(LCD_TypeDef *TFTLCD, u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		LCD_DrawPoint(TFTLCD, uRow,uCol);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    
//������	  
//(x1,y1),(x2,y2):���εĶԽ�����
void LCD_DrawRectangle(LCD_TypeDef *TFTLCD, u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(TFTLCD, x1,y1,x2,y1);
	LCD_DrawLine(TFTLCD, x1,y1,x1,y2);
	LCD_DrawLine(TFTLCD, x1,y2,x2,y2);
	LCD_DrawLine(TFTLCD, x2,y1,x2,y2);
}
//��ָ��λ�û�һ��ָ����С��Բ
//(x,y):���ĵ�
//r    :�뾶
void LCD_Draw_Circle(LCD_TypeDef *TFTLCD, u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //�ж��¸���λ�õı�־
	while(a<=b)
	{
		LCD_DrawPoint(TFTLCD, x0+a,y0-b);             //5
 		LCD_DrawPoint(TFTLCD, x0+b,y0-a);             //0           
		LCD_DrawPoint(TFTLCD, x0+b,y0+a);             //4               
		LCD_DrawPoint(TFTLCD, x0+a,y0+b);             //6 
		LCD_DrawPoint(TFTLCD, x0-a,y0+b);             //1       
 		LCD_DrawPoint(TFTLCD, x0-b,y0+a);             
		LCD_DrawPoint(TFTLCD, x0-a,y0-b);             //2             
  		LCD_DrawPoint(TFTLCD, x0-b,y0-a);             //7     	         
		a++;
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 									  
//��ָ��λ����ʾһ���ַ�
//x,y:��ʼ����
//num:Ҫ��ʾ���ַ�:" "--->"~"
//size:�����С 12/16/24
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void LCD_ShowChar(LCD_TypeDef *TFTLCD, u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	_lcd_dev *lcddev;
	
	if (TFTLCD == LCD0)
	{
		lcddev = &lcddev0;
	}
	else
	{
		lcddev = &lcddev1;
	}

 	num=num-' ';//�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//����1206����
		else if(size==16)temp=asc2_1608[num][t];	//����1608����
		else if(size==24)temp=asc2_2412[num][t];	//����2412����
		else return;								//û�е��ֿ�
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(TFTLCD, x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(TFTLCD, x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=lcddev->height)return;		//��������
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev->width)return;	//��������
				break;
			}
		}  	 
	}  	    	   	 	  
}   
//m^n����
//����ֵ:m^n�η�.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			

//��ʾ����,��λΪ0,����ʾ
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//color:��ɫ 
//num:��ֵ(0~4294967295);	 
void LCD_ShowNum(LCD_TypeDef *TFTLCD, u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(TFTLCD, x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(TFTLCD, x+(size/2)*t,y,temp+'0',size,0); 
	}
} 
//��ʾ����,��λΪ0,������ʾ
//x,y:�������
//num:��ֵ(0~999999999);	 
//len:����(��Ҫ��ʾ��λ��)
//size:�����С
//mode:
//[7]:0,�����;1,���0.
//[6:1]:����
//[0]:0,�ǵ�����ʾ;1,������ʾ.
void LCD_ShowxNum(LCD_TypeDef *TFTLCD, u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(TFTLCD, x+(size/2)*t,y,'0',size,mode&0X01);  
				else LCD_ShowChar(TFTLCD, x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(TFTLCD, x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 
//��ʾ�ַ���
//x,y:�������
//width,height:�����С  
//size:�����С
//*p:�ַ�����ʼ��ַ		  
void LCD_ShowString(LCD_TypeDef *TFTLCD, u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//�˳�
        LCD_ShowChar(TFTLCD, x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}
