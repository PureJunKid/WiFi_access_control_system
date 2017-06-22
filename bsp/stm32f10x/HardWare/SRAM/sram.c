#include "sram.h"      
#include "global.h"

//ʹ��NOR/SRAM�� Bank1.sector1,��ַλHADDR[27,26]=00
//��IS61LV25616/IS62WV25616,��ַ�߷�ΧΪA0~A17 
//��IS61LV51216/IS62WV51216,��ַ�߷�ΧΪA0~A18
#define Bank1_SRAM3_ADDR    ((uint32_t)(0x60000000))        

//��ʼ���ⲿSRAM
void rt_hw_sram_init(void)
{    
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
    GPIO_InitTypeDef  GPIO_InitStructure;
 
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG,ENABLE);
      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);

     //PORTD�����������  
    GPIO_InitStructure.GPIO_Pin =     GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7|
                                    GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|
                                    GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;  
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;          //�����������   
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_Init(GPIOD, &GPIO_InitStructure); 
      
    //PORTE�����������  
    GPIO_InitStructure.GPIO_Pin =     GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|
                                    GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|
                                    GPIO_Pin_14|GPIO_Pin_15;  
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;          //�����������   
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_Init(GPIOE, &GPIO_InitStructure); 
      
    //PORTF�����������  
    GPIO_InitStructure.GPIO_Pin =     GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|
                                    GPIO_Pin_5|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;  
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;          //�����������   
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_Init(GPIOF, &GPIO_InitStructure); 
      
       //PORTG�����������    
    GPIO_InitStructure.GPIO_Pin =     GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|
                                    GPIO_Pin_5|GPIO_Pin_9|GPIO_Pin_10;  
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;          //�����������   
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_Init(GPIOG, &GPIO_InitStructure); 
                         
     readWriteTiming.FSMC_AddressSetupTime = 0x00;     //��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK 1/36M=27ns
    readWriteTiming.FSMC_AddressHoldTime = 0x00;     //��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�    
    readWriteTiming.FSMC_DataSetupTime = 0x03;         //���ݱ���ʱ�䣨DATAST��Ϊ3��HCLK 4/72M=55ns(��EM��SRAMоƬ)     
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;     //ģʽA 
     
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;//  ��������ʹ��NE1 ��Ҳ�Ͷ�ӦBTCR[4],[5]��
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM   
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//�洢�����ݿ��Ϊ16bit  
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;    //�洢��дʹ�� 
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;  
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable; // ��дʹ����ͬ��ʱ��
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;  
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &readWriteTiming; //��дͬ��ʱ��

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //��ʼ��FSMC����

       FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  // ʹ��BANK1    
}
                                                                
//��ָ����ַ��ʼ,����д��n���ֽ�.
//pBuffer:�ֽ�ָ��
//WriteAddr:Ҫд��ĵ�ַ
//n:Ҫд����ֽ���
void FSMC_SRAM_WriteBuffer(uint8_t* pBuffer,uint32_t WriteAddr,uint32_t n)
{
    for(;n!=0;n--)  
    {                                            
        *(vu8*)(Bank1_SRAM3_ADDR+WriteAddr)=*pBuffer;      
        WriteAddr++; 
        pBuffer++;
    }   
}                                                                                
//��ָ����ַ��ʼ,��������n���ֽ�.
//pBuffer:�ֽ�ָ��
//ReadAddr:Ҫ��������ʼ��ַ
//n:Ҫд����ֽ���
void FSMC_SRAM_ReadBuffer(uint8_t* pBuffer,uint32_t ReadAddr,uint32_t n)
{
    for(;n!=0;n--)  
    {                                                
        *pBuffer++=*(vu8*)(Bank1_SRAM3_ADDR+ReadAddr);    
        ReadAddr++; 
    }  
} 
////////////////////////////////////////////////////////////////////////////////////////
//���Ժ���
//��ָ����ַд��1���ֽ�
//addr:��ַ
//data:Ҫд�������
void fsmc_sram_test_write(uint8_t data,uint32_t addr)
{               
    FSMC_SRAM_WriteBuffer(&data,addr,1);//д��1���ֽ�
}
//��ȡ1���ֽ�
//addr:Ҫ��ȡ�ĵ�ַ
//����ֵ:��ȡ��������
uint8_t fsmc_sram_test_read(uint32_t addr)
{
    uint8_t data;
    FSMC_SRAM_ReadBuffer(&data,addr,1);
    return data;
}
