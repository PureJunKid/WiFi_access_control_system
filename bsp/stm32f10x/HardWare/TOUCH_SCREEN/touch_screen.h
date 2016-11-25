#ifndef _TOUCH_SCREEN_H_
#define _TOUCH_SCREEN_H_
#include "sys.h"
#include "ILI93xx.h"

#define TP_PRES_DOWN 0x80  		//����������	  
#define TP_CATH_PRES 0x40  		//�а��������� 
#define CT_MAX_TOUCH  5    		//������֧�ֵĵ���,�̶�Ϊ5��

//������������
typedef struct 
{
	u8 (*init)(void);			//��ʼ��������������
	u8 (*scan)(u8);				//ɨ�败����.0,��Ļɨ��;1,��������;	 
	void (*adjust)(void);		//������У׼ 
	u16 x[CT_MAX_TOUCH]; 		//��ǰ����
	u16 y[CT_MAX_TOUCH];		//�����������5������,����������x[0],y[0]����:�˴�ɨ��ʱ,����������,��
								//x[4],y[4]�洢��һ�ΰ���ʱ������. 
	u8  sta;					//�ʵ�״̬ 
								//b7:����1/�ɿ�0; 
	                            //b6:0,û�а�������;1,�а�������. 
								//b5:����
								//b4~b0:���ݴ��������µĵ���(0,��ʾδ����,1��ʾ����)
/////////////////////������У׼����(����������ҪУ׼)//////////////////////								
	float xfac;					
	float yfac;
	short xoff;
	short yoff;	   
//�����Ĳ���,��������������������ȫ�ߵ�ʱ��Ҫ�õ�.
//b0:0,����(�ʺ�����ΪX����,����ΪY�����TP)
//   1,����(�ʺ�����ΪY����,����ΪX�����TP) 
//b1~6:����.
//b7:0,������
//   1,������ 
	u8 touchtype;
}_m_tp_dev;

extern _m_tp_dev tp_dev;	 	//������������touch.c���涨��

//����/������оƬ��������	   
#define PEN  		PBin(6)  	//PB6 INT
   
//����������
extern void TP_Write_Byte(u8 num);						//�����оƬд��һ������
extern u16 TP_Read_AD(u8 CMD);							//��ȡADת��ֵ
extern u16 TP_Read_XOY(u8 xy);							//���˲��������ȡ(X/Y)
extern u8 TP_Read_XY(u16 *x,u16 *y);					//˫�����ȡ(X+Y)
extern u8 TP_Read_XY2(u16 *x,u16 *y);					//����ǿ�˲���˫���������ȡ
extern void TP_Drow_Touch_Point(LCD_TypeDef *TFTLCD, u16 x,u16 y,u16 color);//��һ������У׼��
extern void TP_Draw_Big_Point(LCD_TypeDef *TFTLCD, u16 x,u16 y,u16 color);	//��һ�����
extern void TP_Save_Adjdata(void);						//����У׼����
extern u8 TP_Get_Adjdata(void);						//��ȡУ׼����
extern void TP_Adjust(LCD_TypeDef *TFTLCD);							//������У׼
extern void TP_Adj_Info_Show(LCD_TypeDef *TFTLCD, u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u16 fac);//��ʾУ׼��Ϣ
//������/������ ���ú���
extern u8 TP_Scan(u8 tp);								//ɨ��
extern u8 TP_Init(LCD_TypeDef *TFTLCD);								//��ʼ��
 
#endif

















