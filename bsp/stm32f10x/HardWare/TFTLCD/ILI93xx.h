#ifndef __LCD_H
#define __LCD_H		
#include "sys.h"	 
#include "stdlib.h"
  
//LCD��Ҫ������
typedef struct  
{										    
	u16 width;			//LCD ���
	u16 height;			//LCD �߶�
	u16 id;				//LCD ID
	u8  dir;			//���������������ƣ�0��������1��������	
	u16	wramcmd;		//��ʼдgramָ��
	u16 setxcmd;		//����x����ָ��
	u16 setycmd;		//����y����ָ�� 
}_lcd_dev; 	  

//LCD����
extern _lcd_dev lcddev0;	//����LCD��Ҫ����
extern _lcd_dev lcddev1;

//LCD�Ļ�����ɫ�ͱ���ɫ	   
extern u16  POINT_COLOR;//Ĭ�Ϻ�ɫ    
extern u16  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ

//LCD��ַ�ṹ��
typedef struct
{
	vu16 LCD_REG;
	vu16 LCD_RAM;
} LCD_TypeDef;

//ʹ��NOR/SRAM�� Bank1 A0��Ϊ�������������� 
//ע������ʱSTM32�ڲ�������һλ����! 			    
#define LCD0_BASE     ((u32)(0x64000000 | 0x00000000))
#define LCD0          ((LCD_TypeDef *) LCD0_BASE)
#define LCD1_BASE     ((u32)(0x68000000 | 0x00000000))
#define LCD1          ((LCD_TypeDef *) LCD1_BASE)
	 
//ɨ�跽����
#define L2R_U2D  0 //������,���ϵ���
#define L2R_D2U  1 //������,���µ���
#define R2L_U2D  2 //���ҵ���,���ϵ���
#define R2L_D2U  3 //���ҵ���,���µ���

#define U2D_L2R  4 //���ϵ���,������
#define U2D_R2L  5 //���ϵ���,���ҵ���
#define D2U_L2R  6 //���µ���,������
#define D2U_R2L  7 //���µ���,���ҵ���	 

#define DFT_SCAN_DIR  L2R_U2D  //Ĭ�ϵ�ɨ�跽��

//������ɫ
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //��ɫ
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY  			 0X8430 //��ɫ
//GUI��ɫ

#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
//������ɫΪPANEL����ɫ 
 
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
//#define LIGHTGRAY        0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY 			 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)
	    															  
void TFTLCD_Init(void);													   	//��ʼ��
void LCD_Clear(LCD_TypeDef *TFTLCD, u16 Color);	 												//����
void LCD_SetCursor(LCD_TypeDef *TFTLCD, u16 Xpos, u16 Ypos);										//���ù��
void LCD_DrawPoint(LCD_TypeDef *TFTLCD, u16 x,u16 y);											//����
void LCD_Fast_DrawPoint(LCD_TypeDef *TFTLCD, u16 x,u16 y,u16 color);								//���ٻ���
u16  LCD_ReadPoint(LCD_TypeDef *TFTLCD, u16 x,u16 y); 											//���� 
void LCD_Draw_Circle(LCD_TypeDef *TFTLCD, u16 x0,u16 y0,u8 r);						 			//��Բ
void LCD_DrawLine(LCD_TypeDef *TFTLCD, u16 x1, u16 y1, u16 x2, u16 y2);							//����
void LCD_DrawRectangle(LCD_TypeDef *TFTLCD, u16 x1, u16 y1, u16 x2, u16 y2);		   				//������
void LCD_Fill(LCD_TypeDef *TFTLCD, u16 sx,u16 sy,u16 ex,u16 ey,u16 color);		   				//��䵥ɫ
void LCD_Color_Fill(LCD_TypeDef *TFTLCD, u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);				//���ָ����ɫ
void LCD_ShowChar(LCD_TypeDef *TFTLCD, u16 x,u16 y,u8 num,u8 size,u8 mode);						//��ʾһ���ַ�
void LCD_ShowNum(LCD_TypeDef *TFTLCD, u16 x,u16 y,u32 num,u8 len,u8 size);  						//��ʾһ������
void LCD_ShowxNum(LCD_TypeDef *TFTLCD, u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);				//��ʾ ����
void LCD_ShowString(LCD_TypeDef *TFTLCD, u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);		//��ʾһ���ַ���,12/16����

//LCD�ֱ�������
#define SSD_HOR_RESOLUTION		800		//LCDˮƽ�ֱ���
#define SSD_VER_RESOLUTION		480		//LCD��ֱ�ֱ���
//LCD������������
#define SSD_HOR_PULSE_WIDTH		1		//ˮƽ����
#define SSD_HOR_BACK_PORCH		210		//ˮƽǰ��
#define SSD_HOR_FRONT_PORCH		45		//ˮƽ����

#define SSD_VER_PULSE_WIDTH		1		//��ֱ����
#define SSD_VER_BACK_PORCH		34		//��ֱǰ��
#define SSD_VER_FRONT_PORCH		10		//��ֱǰ��
//���¼����������Զ�����
#define SSD_HT	(SSD_HOR_RESOLUTION+SSD_HOR_PULSE_WIDTH+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS	(SSD_HOR_PULSE_WIDTH+SSD_HOR_BACK_PORCH)
#define SSD_VT 	(SSD_VER_PULSE_WIDTH+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH+SSD_VER_RESOLUTION)
#define SSD_VPS (SSD_VER_PULSE_WIDTH+SSD_VER_BACK_PORCH)

#endif  
	 
	 



