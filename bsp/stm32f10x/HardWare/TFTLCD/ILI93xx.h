#ifndef __LCD_H
#define __LCD_H		
#include "stdlib.h"
#include "global.h"

//��Ļ��С
#define XSIZE_PHYS  320 //X��
#define YSIZE_PHYS  240 //Y��
#define VXSIZE_PHYS	320 
#define VYSIZE_PHYS 240
 
//LCD��Ҫ������
typedef struct  
{										    
	uint16_t width;			//LCD ���
	uint16_t height;			//LCD �߶�
	uint16_t id;				//LCD ID
	uint8_t  dir;			//���������������ƣ�0��������1��������	
	uint16_t	wramcmd;		//��ʼдgramָ��
	uint16_t setxcmd;		//����x����ָ��
	uint16_t setycmd;		//����y����ָ�� 
}_lcd_dev; 	  

//LCD����
extern _lcd_dev lcddev0;	//����LCD��Ҫ����
extern _lcd_dev lcddev1;

//LCD�Ļ�����ɫ�ͱ���ɫ	   
extern uint16_t  POINT_COLOR;//Ĭ�Ϻ�ɫ    
extern uint16_t  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ

//LCD��ַ�ṹ��
typedef struct
{
	vu16 LCD_REG;
	vu16 LCD_RAM;
} LCD_TypeDef;

//ʹ��NOR/SRAM�� Bank1 A0��Ϊ�������������� 
//ע������ʱSTM32�ڲ�������һλ����! 			    
#define LCD0_BASE     ((uint32_t)(0x68000000 | 0x00000000)) /* ����Һ�� */
#define LCD0          ((LCD_TypeDef *) LCD0_BASE)
#define LCD1_BASE     ((uint32_t)(0x64000000 | 0x00000000)) /* ����Һ�� */
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
void LCD_Clear(LCD_TypeDef *TFTLCD, uint16_t Color);	 												//����
void LCD_SetCursor(LCD_TypeDef *TFTLCD, uint16_t Xpos, uint16_t Ypos);										//���ù��
void LCD_DrawPoint(LCD_TypeDef *TFTLCD, uint16_t x,uint16_t y);											//����
void LCD_Fast_DrawPoint(LCD_TypeDef *TFTLCD, uint16_t x,uint16_t y,uint16_t color);								//���ٻ���
uint16_t  LCD_ReadPoint(LCD_TypeDef *TFTLCD, uint16_t x,uint16_t y); 											//���� 
void LCD_Draw_Circle(LCD_TypeDef *TFTLCD, uint16_t x0,uint16_t y0,uint8_t r);						 			//��Բ
void LCD_DrawLine(LCD_TypeDef *TFTLCD, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);							//����
void LCD_DrawRectangle(LCD_TypeDef *TFTLCD, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);		   				//������
void LCD_Fill(LCD_TypeDef *TFTLCD, uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);		   				//��䵥ɫ
void LCD_Color_Fill(LCD_TypeDef *TFTLCD, uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color);				//���ָ����ɫ
void LCD_ShowChar(LCD_TypeDef *TFTLCD, uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode);						//��ʾһ���ַ�
void LCD_ShowNum(LCD_TypeDef *TFTLCD, uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size);  						//��ʾһ������
void LCD_ShowxNum(LCD_TypeDef *TFTLCD, uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode);				//��ʾ ����
void LCD_ShowString(LCD_TypeDef *TFTLCD, uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p);		//��ʾһ���ַ���,12/16����

#endif  
	 
	 



