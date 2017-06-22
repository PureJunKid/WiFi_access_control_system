/******************************************************************************
* @ File name --> ds1307.c
* @ Author    --> By@ Sam Chan
* @ Version   --> V1.0
* @ Date      --> 11 - 27 - 2013
* @ Brief     --> ʱ��оƬDS1307����
* @           --> ��������������DS1307��DS1338��DS1338ZоƬ
*
* @ Copyright (C) 20**
* @ All rights reserved
*******************************************************************************
*
*                                  File Update
* @ Version   --> V1.
* @ Author    --> By@
* @ Date      --> 
* @ Revise    --> 
*
******************************************************************************/

#include "ds1307.h"

/******************************************************************************
                                    �������
******************************************************************************/

Time_Typedef TimeValue;  //����ʱ�仺��ָ��
uint8_t Time_Buffer[8];    //ʱ���������ݻ���

/******************************************************************************
* Function Name --> DS1307ĳ�Ĵ���д��һ���ֽ�����
* Description   --> none
* Input         --> REG_ADD��Ҫ�����Ĵ�����ַ
*                   dat��Ҫд�������
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void DS1307_Write_Byte(uint8_t REG_ADD,uint8_t dat)
{
    IIC_Start();
    if(!(IIC_Write_Byte(DS1307_Write))) //����д������Ӧ��λ
    {
        IIC_Write_Byte(REG_ADD);
        IIC_Write_Byte(dat);
    }
    IIC_Stop();
}
/******************************************************************************
* Function Name --> DS1307ĳ�Ĵ�����ȡһ���ֽ�����
* Description   --> none
* Input         --> REG_ADD��Ҫ�����Ĵ�����ַ
* Output        --> none
* Reaturn       --> ��ȡ������ֵ
******************************************************************************/
uint8_t DS1307_Read_Byte(uint8_t REG_ADD)
{
    uint8_t rcv;

    IIC_Start();
    if(!(IIC_Write_Byte(DS1307_Write))) //����д������Ӧ��λ
    {
        IIC_Write_Byte(REG_ADD);    //����Ҫ�����ļĴ�����ַ
        IIC_Start();    //��������
        IIC_Write_Byte(DS1307_Read);    //���Ͷ�ȡ����
        rcv = IIC_Read_Byte();
        IIC_Ack(0x01); //���ͷ�Ӧ���ź�
    }
    IIC_Stop();
    return rcv;
}
/******************************************************************************
* Function Name --> DS1307��ʱ�������Ĵ���������д�����ݻ��߶�ȡ����
* Description   --> ����д��n�ֽڻ���������ȡn�ֽ�����
* Input         --> REG_ADD��Ҫ�����Ĵ�����ʼ��ַ
*                   *WBuff��д�����ݻ���
*                   num��д����������
*                   mode������ģʽ��0��д�����ݲ�����1����ȡ���ݲ���
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void DS1307_Operate_Register(uint8_t REG_ADD,uint8_t *pBuff,uint8_t num,uint8_t mode)
{
    uint8_t i;
    if(mode)    //��ȡ����
    {
        IIC_Start();
        if(!(IIC_Write_Byte(DS1307_Write)))    //����д������Ӧ��λ
        {
            IIC_Write_Byte(REG_ADD);    //��λ��ʼ�Ĵ�����ַ
            IIC_Start();    //��������
            IIC_Write_Byte(DS1307_Read);    //���Ͷ�ȡ����
            for(i = 0;i < num;i++)
            {
                *pBuff = IIC_Read_Byte();    //��ȡ����
                if(i == (num - 1))    IIC_Ack(0x01);    //���ͷ�Ӧ���ź�
                else IIC_Ack(0x00);    //����Ӧ���ź�
                pBuff++;
            }
        }
        IIC_Stop();    
    }
    else    //д������
    {             
        IIC_Start();
        if(!(IIC_Write_Byte(DS1307_Write)))    //����д������Ӧ��λ
        {
            IIC_Write_Byte(REG_ADD);    //��λ��ʼ�Ĵ�����ַ
            for(i = 0;i < num;i++)
            {
                IIC_Write_Byte(*pBuff);    //д������
                pBuff++;
            }
        }
        IIC_Stop();
    }
}
/******************************************************************************
* Function Name --> DS1307��ȡ����д��ʱ����Ϣ
* Description   --> ����д��n�ֽڻ���������ȡn�ֽ�����
* Input         --> *pBuff��д�����ݻ���
*                   mode������ģʽ��0��д�����ݲ�����1����ȡ���ݲ���
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void DS1307_ReadWrite_Time(uint8_t mode)
{
    uint8_t Time_Register[8];    //����ʱ�仺��
    
    if(mode)    //��ȡʱ����Ϣ
    {
        DS1307_Operate_Register(Address_second,Time_Register,7,1);    //�����ַ��0x00����ʼ��ȡʱ����������
        
        /******�����ݸ��Ƶ�ʱ��ṹ���У��������������******/
        TimeValue.second =     (Time_Register[0] & 0x0f) + ((Time_Register[0] & 0x70)>>4)*10;    //������
        TimeValue.minute =     (Time_Register[1] & 0x0f) + ((Time_Register[1] & 0x70)>>4)*10;    //��������
        TimeValue.hour =     (Time_Register[2] & 0x0f) + ((Time_Register[2] & 0x30)>>4)*10;        //Сʱ����
        TimeValue.week =     Time_Register[3] & Shield_weekBit;                                    //��������
        TimeValue.date =     (Time_Register[4] & 0x0f) + ((Time_Register[4] & 0x30)>>4)*10;        //������
        TimeValue.month =     (Time_Register[5] & 0x0f) + ((Time_Register[5] & 0x10)>>4)*10;    //������
        TimeValue.year =     (Time_Register[6] & 0x0f) + ((Time_Register[6] & 0xf0)>>4)*10;        //������
    }
    else
    {
        /******��ʱ��ṹ���и������ݽ���******/
        Time_Register[0] = TimeValue.second | Control_Chip_Run;    //�룬����оƬ
        Time_Register[1] = TimeValue.minute;    //����
        Time_Register[2] = TimeValue.hour | Hour_Mode24;    //Сʱ��24Сʱ��
        Time_Register[3] = TimeValue.week;    //����
        Time_Register[4] = TimeValue.date;    //��        
        Time_Register[5] = TimeValue.month;    //��
        Time_Register[6] = TimeValue.year;    //��
        
        DS1307_Operate_Register(Address_second,Time_Register,7,0);    //�����ַ��0x00����ʼд��ʱ����������
    }
}
/******************************************************************************
* Function Name --> DS1307���Ժû�
* Description   --> ��DS1307оƬ��RAM�����һ����ַд��һ�����ݲ��������ж�
*                   ���ϴ�д���ֵ��ȣ����ǵ�һ���ϵ磬�������ʼ��ʱ��
* Input         --> none
* Output        --> none
* Reaturn       --> 0���豸���������ǵ�һ���ϵ�
*                   1���豸�����������
******************************************************************************/    
uint8_t DS1307_Check(void)
{
    if(DS1307_Read_Byte(RAM_Address55) == test_data)    return 0;    //�豸���������ǵ�һ���ϵ�
    else    return 1;
}
/******************************************************************************
* Function Name --> DS1307���õ�RAMд���ݲ���
* Description   --> none
* Input         --> *pBuff��д���ݴ����
*                   WRadd����д��ʼ��ַ����Χ��RAM_Address0 ~ RAM_Address55֮�䣬���һλ��ַ��������;
*                   num����д�ֽ����ݵ���������Χ��1 ~ 55֮��
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void DS1307_RAM_Write_Data(uint8_t* pBuff,uint8_t WRadd,uint8_t num)
{
    uint8_t i;
    uint8_t ADDremain;   //д����������
    
    /******�ж�д�����ݵ���ʼ��ַ��Χ******/

    if(WRadd >= RAM_Address55)  return; //���һ��RAM��Ԫ������ֱ���˳�

    /******�жϷ������ݵ�����Ŀ******/

    if((WRadd + num) >= (RAM_Address55 - 1))    ADDremain = RAM_Address55 - 1 - WRadd;  //������Χ��д�����µĿռ�
    else    ADDremain = num;    //û�����ռ䣬ֱ��д��

    IIC_Start();
    if(!(IIC_Write_Byte(DS1307_Write))) //����д������Ӧ���ź�
    {
        IIC_Write_Byte(WRadd);  //����д�������׵�ַ
        for(i = 0;i < ADDremain;i++)
        {
            IIC_Write_Byte(pBuff[i]);   //д������
        }
    }
    IIC_Stop();
}
/******************************************************************************
* Function Name --> DS1307���õ�RAM�����ݲ���
* Description   --> none
* Input         --> WRadd����д��ʼ��ַ����Χ��RAM_Address0 ~ RAM_Address55֮�䣬���һλ��ַ��������;
*                   num����д�ֽ����ݵ���������Χ��1 ~ 55֮��
* Output        --> *pBuff�������ݴ����
* Reaturn       --> none
******************************************************************************/
void DS1307_RAM_Read_Data(uint8_t* pBuff,uint8_t WRadd,uint8_t num)
{
    uint8_t i;
    uint8_t ADDremain;

    /******�ж϶�ȡ���ݵ���ʼ��ַ��Χ******/

    if(WRadd >= RAM_Address55)  return; //���һ��RAM��Ԫ������ֱ���˳�

    /******���һ����ַ���������DS1307���ã����Բ������һ����ַ����******/

    if((WRadd + num) >= RAM_Address55)  ADDremain = RAM_Address55 - 1 - WRadd;  //������Χ�ˣ���ȡ��ʼ��ַ�������ڶ�����ַ�ռ������
    else    ADDremain = num;    //û������ַ��Χ��ȫ����ȡ��

    IIC_Start();
    if(!(IIC_Write_Byte(DS1307_Write))) //����д������Ӧ���ź�
    {
        IIC_Write_Byte(WRadd);  //���Ͷ�ȡ���ݿ�ʼ�Ĵ�����ַ
        IIC_Start();
        if(!(IIC_Write_Byte(DS1307_Read)))  //���Ͷ�ȡ������Ӧ���ź�
        {
            for(i = 0;i < ADDremain;i++)
            {
                pBuff[i] = IIC_Read_Byte(); //��ʼ����num������
                if(i == (ADDremain - 1))    IIC_Ack(0x01); //��ȡ�����һ�����ݣ����ͷ�Ӧ���ź�
                else    IIC_Ack(0x00);  //����Ӧ���ź�
            }
        }
    }
    IIC_Stop();
}
/******************************************************************************
* Function Name --> ʱ��������ʼ��
* Description   --> none
* Input         --> *TimeVAL��RTCоƬ�Ĵ���ֵָ��
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void DS1307_Time_Init(Time_Typedef *TimeVAL)
{    
    //ʱ����������
    Time_Buffer[0] = ((((TimeVAL->second%10)&0x0f) | ((TimeVAL->second/10)&0x0f)<<4)) &~(1<<7);    //����RTCоƬ
    Time_Buffer[1] = (((TimeVAL->minute%10)&0x0f) | ((TimeVAL->minute/10)&0x0f)<<4);
    Time_Buffer[2] = (((TimeVAL->hour%10)&0x0f) | ((TimeVAL->hour/10)&0x03)<<4);
    Time_Buffer[3] = TimeVAL->week&0x07;
    Time_Buffer[4] = (((TimeVAL->date%10)&0x0f) | ((TimeVAL->date/10)&0x03)<<4);
    Time_Buffer[5] = (((TimeVAL->month%10)&0x0f) | ((TimeVAL->month/10)&0x01)<<4);
    Time_Buffer[6] = (((TimeVAL->year%10)&0x0f) | ((TimeVAL->year/10)&0x0f)<<4);
    //Ƶ���������

    #ifdef  Chip_Type   //��������ˣ���ʹ�õ���DS1307оƬ
    
        Time_Buffer[7] = TimeVAL->SQWE;    //Ƶ���������

    #else   //û���壬��ʹ�õ���DS1338����DS1338ZоƬ

        Time_Buffer[7] = TimeVAL->SQWE | OSF_Enable;    //Ƶ���������

    #endif

//    DS1307_Write_Byte(Address_second, Control_Chip_Run);    //������оƬ
    
    DS1307_Operate_Register(Address_second,Time_Buffer,8,0);    //����Ĵ�����0x00����ʼд��8������

    DS1307_Write_Byte(RAM_Address55, test_data);    //�����һ��RAM��ַд��ʶ��ֵ
}

