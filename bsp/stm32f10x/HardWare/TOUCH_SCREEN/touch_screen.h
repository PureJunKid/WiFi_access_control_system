#ifndef _TOUCH_SCREEN_H_
#define _TOUCH_SCREEN_H_
#include "sys.h"
#include "ILI93xx.h"

//�봥�����йض��壬����ʵ�������д
#define TOUCH_AD_TOP		160  	//���´������Ķ�����д�� Y ��ģ������ֵ��
#define TOUCH_AD_BOTTOM		3990 	//���´������ĵײ���д�� Y ��ģ������ֵ��
#define TOUCH_AD_LEFT 		160		//���´���������࣬д�� X ��ģ������ֵ��
#define TOUCH_AD_RIGHT		3990	//���´��������Ҳ࣬д�� X ��ģ������ֵ��

#define TP_PRES_DOWN 0x80  		//����������	  
#define TP_CATH_PRES 0x40  		//�а��������� 
#define CT_MAX_TOUCH  5    		//������֧�ֵĵ���,�̶�Ϊ5��

//������������
typedef struct 
{
	u8 (*init)(void);			//��ʼ��������������
	u8 (*scan)(LCD_TypeDef *TFTLCD, u8);				//ɨ�败����.0,��Ļɨ��;1,��������;	 
	void (*adjust)(LCD_TypeDef *TFTLCD);		//������У׼ 
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

extern _m_tp_dev tp_dev0;	 	//������������touch.c���涨��
extern _m_tp_dev tp_dev1;	 	//������������touch.c���涨��

//������оƬ��������	   
#define tp_irq0  		PBin(7)  	/* ���ⴥ���� */
#define tp_irq1  		PBin(6)  	/* ���ڴ����� */
   
//����������
extern void TP_Write_Byte(LCD_TypeDef *TFTLCD, u8 num);						//�����оƬд��һ������
extern u16 TP_Read_AD(LCD_TypeDef *TFTLCD, u8 CMD);							//��ȡADת��ֵ
extern u16 TP_Read_XOY(LCD_TypeDef *TFTLCD, u8 xy);							//���˲��������ȡ(X/Y)
extern u8 TP_Read_XY(LCD_TypeDef *TFTLCD, u16 *x,u16 *y);					//˫�����ȡ(X+Y)
extern u8 TP_Read_XY2(LCD_TypeDef *TFTLCD, u16 *x,u16 *y);					//����ǿ�˲���˫���������ȡ
extern void TP_Drow_Touch_Point(LCD_TypeDef *TFTLCD, u16 x,u16 y,u16 color);//��һ������У׼��
extern void TP_Draw_Big_Point(LCD_TypeDef *TFTLCD, u16 x,u16 y,u16 color);	//��һ�����
extern void TP_Save_Adjdata(LCD_TypeDef *TFTLCD);						//����У׼����
extern u8 TP_Get_Adjdata(LCD_TypeDef *TFTLCD);						//��ȡУ׼����
extern void TP_Adjust(LCD_TypeDef *TFTLCD);							//������У׼
extern void TP_Adj_Info_Show(LCD_TypeDef *TFTLCD, u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u16 fac);//��ʾУ׼��Ϣ
//������/������ ���ú���
extern u8 TP_Scan(LCD_TypeDef *TFTLCD, u8 tp);								//ɨ��
extern u8 TP_Init(void);								//��ʼ��
 
#endif

