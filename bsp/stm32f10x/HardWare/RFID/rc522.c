#include "global.h"
#include "rc522.h"
#include "string.h"            //�ַ�������
#include "spi_bus.h"
#include <drivers/spi.h>
#include <rtthread.h>

/*******************************
*SPI1����˵����         ����
*1--SS  <----->PA4            
*2--SCK <----->PA5
*3--MOSI<----->PA7
*4--MISO<----->PA6
*5--����
*6--GND <----->GND
*7--RST <----->PC4
*8--VCC <----->VCC��
************************************/
/*******************************
*SPI3����˵����         ����
*1--SS  <----->PG15            
*2--SCK <----->PB3
*3--MOSI<----->PB5
*4--MISO<----->PB4
*5--����
*6--GND <----->GND
*7--RST <----->PB6
*8--VCC <----->VCC��
************************************/

/*ȫ�ֱ���*/
unsigned char CT[2];//������
unsigned char get_card_id_array[4];//��ID����

struct rt_spi_device *rt_spi_rc522_device;//RC522�豸(��ǰ�������豸)
struct rt_spi_device *rt_spi_rc522_in_device;//RC522�豸(����)
struct rt_spi_device *rt_spi_rc522_out_device;//RC522�豸(����)
struct rt_spi_message rc522_message;    //SPI�豸ͨ������Ϣ�ṹ��

uint8_t rc522_readBuf[2],rc522_writeBuf[2];    //SPIͨ�Ż���

#define delay_ns(ms) rt_thread_delay(rt_tick_from_millisecond(1))
#define delay_ms(ms) rt_thread_delay(rt_tick_from_millisecond(ms))

void rc522_attach_device()
{
    static struct rt_spi_device rc522_in_spi_device;
    static struct stm32_spi_cs  rc522_in_spi_cs;
    static struct rt_spi_device rc522_out_spi_device;
    static struct stm32_spi_cs  rc522_out_spi_cs;
    GPIO_InitTypeDef  GPIO_InitStructure;
    
     //ʹ��PA,PB,PG�˿�ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOG, ENABLE);
    
    //����RC522 RST����
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;//RST_IN
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    //IO���ٶ�Ϊ50MHz
     GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��
    
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;//RST_OUT
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    //IO���ٶ�Ϊ50MHz
     GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��
    
    //����RC522_IN_CS����
    rc522_in_spi_cs.GPIOx = GPIOB;
    rc522_in_spi_cs.GPIO_Pin = GPIO_Pin_12;

    GPIO_InitStructure.GPIO_Pin = rc522_in_spi_cs.GPIO_Pin;//rc522_in_cs
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    //IO���ٶ�Ϊ50MHz
    GPIO_Init(rc522_in_spi_cs.GPIOx, &GPIO_InitStructure);        //�����趨������ʼ��
    GPIO_SetBits(rc522_in_spi_cs.GPIOx,rc522_in_spi_cs.GPIO_Pin);//�����
    //��������RFID�豸��SPI1����
    rt_spi_bus_attach_device(&rc522_in_spi_device, "RFID_IN", "SPI2", (void*)&rc522_in_spi_cs);
    
    //����RC522_OUT_CS����
    rc522_out_spi_cs.GPIOx = GPIOA;
    rc522_out_spi_cs.GPIO_Pin = GPIO_Pin_15;

    GPIO_InitStructure.GPIO_Pin = rc522_out_spi_cs.GPIO_Pin;//rc522_out_cs
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    //IO���ٶ�Ϊ50MHz
    GPIO_Init(rc522_out_spi_cs.GPIOx, &GPIO_InitStructure);        //�����趨������ʼ��
    GPIO_SetBits(rc522_out_spi_cs.GPIOx,rc522_out_spi_cs.GPIO_Pin);//�����
    //��������RFID�豸��SPI1����
    rt_spi_bus_attach_device(&rc522_out_spi_device, "RFID_OUT", "SPI2", (void*)&rc522_out_spi_cs);
}

int8_t InitRC522(void)
{
    /* ��SPI2_BUS�ϸ���RC522_IN�豸 */
    rc522_attach_device();
    /* ��������RFID�豸 */
    rt_spi_rc522_in_device = (struct rt_spi_device *) rt_device_find("RFID_IN");//����RFID_IN�豸
    if (rt_spi_rc522_in_device == RT_NULL)
    {
        return -1;
    }

    /* config spi */
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = RT_SPI_MODE_0 | RT_SPI_MSB;
        cfg.max_hz = 10 * 1000 * 1000;
        rt_spi_configure(rt_spi_rc522_in_device, &cfg);
    }
    
    SET_RC522_IN_RST;
    delay_ns(10);
    CLR_RC522_IN_RST;
    delay_ns(10);
    SET_RC522_IN_RST;
    delay_ns(10);

    rt_spi_rc522_device = rt_spi_rc522_in_device;//���õ�ǰRFID�豸Ϊ�����豸
    PcdReset();             
    PcdAntennaOff();
    delay_ms(2);  
    PcdAntennaOn();
    M500PcdConfigISOType( 'A' );
    /* ��������RFID�豸 */
    rt_spi_rc522_out_device = (struct rt_spi_device *) rt_device_find("RFID_OUT");//����RFID_IN�豸
    if (rt_spi_rc522_out_device == RT_NULL)
    {
        return -1;
    }

    /* config spi */
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = RT_SPI_MODE_0 | RT_SPI_MSB;
        cfg.max_hz = 10 * 1000 * 1000;
        rt_spi_configure(rt_spi_rc522_out_device, &cfg);
    }

    SET_RC522_OUT_RST;
    delay_ns(10);
    CLR_RC522_OUT_RST;
    delay_ns(10);
    SET_RC522_OUT_RST;
    delay_ns(10);

    rt_spi_rc522_device = rt_spi_rc522_out_device;//���õ�ǰRFID�豸Ϊ�����豸
    PcdReset();             
    PcdAntennaOff();
    delay_ms(2);  
    PcdAntennaOn();
    M500PcdConfigISOType( 'A' );
    
    return 0;
}
void Reset_RC522(void)
{
  PcdReset();
  PcdAntennaOff();
  delay_ms(2);  
  PcdAntennaOn();
}                         
/////////////////////////////////////////////////////////////////////
//��    �ܣ�Ѱ��
//����˵��: req_code[IN]:Ѱ����ʽ
//                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
//                0x26 = Ѱδ��������״̬�Ŀ�
//                   pTagType[OUT]����Ƭ���ʹ���
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(uint8_t req_code,uint8_t *pTagType)
{
    char   status;  
    uint8_t   unLen;
    uint8_t   ucComMF522Buf[MAXRLEN]; 

    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x07);
    SetBitMask(TxControlReg,0x03);
 
    ucComMF522Buf[0] = req_code;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);

    if ((status == MI_OK) && (unLen == 0x10))
    {    
        *pTagType     = ucComMF522Buf[0];
        *(pTagType+1) = ucComMF522Buf[1];
    }
    else
    {   status = MI_ERR;   }
   
    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�����ײ
//����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////  
char PcdAnticoll(uint8_t *pSnr)
{
    char   status;
    uint8_t   i,snr_check=0;
    uint8_t   unLen;
    uint8_t   ucComMF522Buf[MAXRLEN]; 
    

    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
         for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    
    SetBitMask(CollReg,0x80);
    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�ѡ����Ƭ
//����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(uint8_t *pSnr)
{
    char   status;
    uint8_t   i;
    uint8_t   unLen;
    uint8_t   ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
        ucComMF522Buf[i+2] = *(pSnr+i);
        ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    ClearBitMask(Status2Reg,0x08);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    
    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���֤��Ƭ����
//����˵��: auth_mode[IN]: ������֤ģʽ
//                 0x60 = ��֤A��Կ
//                 0x61 = ��֤B��Կ 
//          addr[IN]�����ַ
//          pKey[IN]������
//          pSnr[IN]����Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////               
char PcdAuthState(uint8_t   auth_mode,uint8_t   addr,uint8_t *pKey,uint8_t *pSnr)
{
    char   status;
    uint8_t   unLen;
    uint8_t   ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
//    for (i=0; i<6; i++)
//    {    ucComMF522Buf[i+2] = *(pKey+i);   }
//    for (i=0; i<6; i++)
//    {    ucComMF522Buf[i+8] = *(pSnr+i);   }
    memcpy(&ucComMF522Buf[2], pKey, 6); 
    memcpy(&ucComMF522Buf[8], pSnr, 4); 
    
    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���ȡM1��һ������
//����˵��: addr[IN]�����ַ
//          p [OUT]�����������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
///////////////////////////////////////////////////////////////////// 
char PcdRead(uint8_t   addr,uint8_t *p )
{
    char   status;
    uint8_t   unLen;
    uint8_t   i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
 //   {   memcpy(p , ucComMF522Buf, 16);   }
    {
        for (i=0; i<16; i++)
        {    *(p +i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�д���ݵ�M1��һ��
//����˵��: addr[IN]�����ַ
//          p [IN]��д������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////                  
char PcdWrite(uint8_t   addr,uint8_t *p )
{
    char   status;
    uint8_t   unLen;
    uint8_t   i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, p , 16);
        for (i=0; i<16; i++)
        {    
            ucComMF522Buf[i] = *(p +i);   
        }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ����Ƭ��������״̬
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
    uint8_t   status;
    uint8_t   unLen;
    uint8_t   ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    status = status;//��������
    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//��MF522����CRC16����
/////////////////////////////////////////////////////////////////////
void CalulateCRC(uint8_t *pIn ,uint8_t   len,uint8_t *pOut )
{
    uint8_t   i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   WriteRawRC(FIFODataReg, *(pIn +i));   }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOut [0] = ReadRawRC(CRCResultRegL);
    pOut [1] = ReadRawRC(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���λRC522
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReset(void)
{
    WriteRawRC(CommandReg,PCD_RESETPHASE);
    WriteRawRC(CommandReg,PCD_RESETPHASE);
    delay_ns(10);
    
    WriteRawRC(ModeReg,0x3D);            //��Mifare��ͨѶ��CRC��ʼֵ0x6363
    WriteRawRC(TReloadRegL,30);           
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);
    WriteRawRC(TPrescalerReg,0x3E);
    
    WriteRawRC(TxAutoReg,0x40);//����Ҫ
   
    return MI_OK;
}
//////////////////////////////////////////////////////////////////////
//����RC632�Ĺ�����ʽ 
//////////////////////////////////////////////////////////////////////
char M500PcdConfigISOType(uint8_t   type)
{
   if (type == 'A')                     //ISO14443_A
   { 
       ClearBitMask(Status2Reg,0x08);
       WriteRawRC(ModeReg,0x3D);//3F
       WriteRawRC(RxSelReg,0x86);//84
       WriteRawRC(RFCfgReg,0x7F);   //4F
          WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
       WriteRawRC(TReloadRegH,0);
       WriteRawRC(TModeReg,0x8D);
       WriteRawRC(TPrescalerReg,0x3E);
       delay_ns(1000);
       PcdAntennaOn();
   }
   else{ return 1; }
   
   return MI_OK;
}
/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//��    �أ�������ֵ
/////////////////////////////////////////////////////////////////////
uint8_t ReadRawRC(uint8_t Address)
{
    rc522_writeBuf[0] = ((Address<<1)&0x7E)|0x80;

    rc522_message.send_buf = rc522_writeBuf;
    rc522_message.recv_buf = rc522_readBuf;    //���ö�д����
    rc522_message.length = 1;            //���ö�д����ReadID
    rc522_message.cs_take = 1;            //��ʼͨ��ʱ����CS
    rc522_message.cs_release = 0;        //����ͨ��ʱ������CS
    rc522_message.next = RT_NULL;
    rt_spi_transfer_message(rt_spi_rc522_device, &rc522_message);//����һ�����ݴ���

    rc522_writeBuf[0] = 0x00;
    rc522_message.cs_release = 1;        //����ͨ��ʱ����CS
    rt_spi_transfer_message(rt_spi_rc522_device, &rc522_message);//����һ�����ݴ���
    
    return rc522_readBuf[0];
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�дRC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//          value[IN]:д���ֵ
/////////////////////////////////////////////////////////////////////
void WriteRawRC(uint8_t Address, uint8_t value)
{  
    rc522_writeBuf[0] = ((Address<<1)&0x7E);
    rc522_writeBuf[1] = value;

    rc522_message.send_buf = rc522_writeBuf;
    rc522_message.recv_buf = rc522_readBuf;    //���ö�д����
    rc522_message.length = 2;            //���ö�д����
    rc522_message.cs_take = 1;            //��ʼͨ��ʱ����CS
    rc522_message.cs_release = 1;        //����ͨ��ʱ����CS
    rc522_message.next = RT_NULL;
    rt_spi_transfer_message(rt_spi_rc522_device, &rc522_message);//����һ�����ݴ���
}
/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void SetBitMask(uint8_t   reg,uint8_t   mask)  
{
    char   tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void ClearBitMask(uint8_t   reg,uint8_t   mask)  
{
    char   tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 

/////////////////////////////////////////////////////////////////////
//��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
//����˵����Command[IN]:RC522������
//          pIn [IN]:ͨ��RC522���͵���Ƭ������
//          InLenByte[IN]:�������ݵ��ֽڳ���
//          pOut [OUT]:���յ��Ŀ�Ƭ��������
//          *pOutLenBit[OUT]:�������ݵ�λ����
/////////////////////////////////////////////////////////////////////
char PcdComMF522(uint8_t   Command, 
                 uint8_t *pIn , 
                 uint8_t   InLenByte,
                 uint8_t *pOut , 
                 uint8_t *pOutLenBit)
{
    char   status = MI_ERR;
    uint8_t   irqEn   = 0x00;
    uint8_t   waitFor = 0x00;
    uint8_t   lastBits;
    uint8_t   n;
    uint16_t   i;
    switch (Command)
    {
        case PCD_AUTHENT:
            irqEn   = 0x12;
            waitFor = 0x10;
            break;
        case PCD_TRANSCEIVE:
            irqEn   = 0x77;
            waitFor = 0x30;
            break;
        default:
            break;
    }
   
    WriteRawRC(ComIEnReg,irqEn|0x80);
    ClearBitMask(ComIrqReg,0x80);    //�������ж�λ
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);         //��FIFO����
    
    for (i=0; i<InLenByte; i++)
    {   WriteRawRC(FIFODataReg, pIn [i]);    }
    WriteRawRC(CommandReg, Command);      
    
    if (Command == PCD_TRANSCEIVE)
    {    SetBitMask(BitFramingReg,0x80);  }     //��ʼ����
                                             
    i = 256;    //����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
    do
    {
        n = ReadRawRC(ComIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg,0x80);

    if (i!=0)
    {    
        if(!(ReadRawRC(ErrorReg)&0x1B))
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {   status = MI_NOTAGERR;   }
            if (Command == PCD_TRANSCEIVE)
            {
                   n = ReadRawRC(FIFOLevelReg);
                  lastBits = ReadRawRC(ControlReg) & 0x07;
                if (lastBits)
                {   *pOutLenBit = (n-1)*8 + lastBits;   }
                else
                {   *pOutLenBit = n*8;   }
                if (n == 0)
                {   n = 1;    }
                if (n > MAXRLEN)
                {   n = MAXRLEN;   }
                for (i=0; i<n; i++)
                {   pOut [i] = ReadRawRC(FIFODataReg);    }
            }
        }
        else
        {   status = MI_ERR;   }
        
    }

    SetBitMask(ControlReg,0x80);           // stop timer now
    WriteRawRC(CommandReg,PCD_IDLE); 
    return status;
}

/////////////////////////////////////////////////////////////////////
//��������  
//ÿ��������ر����շ���֮��Ӧ������1ms�ļ��
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn(void)
{
    uint8_t   i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}


/////////////////////////////////////////////////////////////////////
//�ر�����
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ��ۿ�ͳ�ֵ
//����˵��: dd_mode[IN]��������
//               0xC0 = �ۿ�
//               0xC1 = ��ֵ
//          addr[IN]��Ǯ����ַ
//          pValue[IN]��4�ֽ���(��)ֵ����λ��ǰ
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////                 
char PcdValue(uint8_t dd_mode,uint8_t addr,uint8_t *pValue)
{
    char status;
    uint8_t  unLen;
    uint8_t ucComMF522Buf[MAXRLEN]; 
    //uint8_t i;
    
    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        memcpy(ucComMF522Buf, pValue, 4);
        //for (i=0; i<16; i++)
        //{    ucComMF522Buf[i] = *(pValue+i);   }
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 
   
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�����Ǯ��
//����˵��: sourceaddr[IN]��Դ��ַ
//          goaladdr[IN]��Ŀ���ַ
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdBakValue(uint8_t sourceaddr, uint8_t goaladdr)
{
    char status;
    uint8_t  unLen;
    uint8_t ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
 
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status != MI_OK)
    {    return MI_ERR;   }
    
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    return status;
}

/*************************************
*�������ܣ���ʾ���Ŀ��ţ���ʮ��������ʾ
*������x��y ����
*        p ���ŵĵ�ַ
*        charcolor �ַ�����ɫ
*        bkcolor   ��������ɫ
***************************************/
void TurnID(uint8_t *p, char* card_id)     //ת�����Ŀ��ţ���ʮ��������ʾ
{
    unsigned char i;
    for(i=0;i<4;i++)
    {
        card_id[i*2]=p[i]>>4;
        card_id[i*2]>9?(card_id[i*2]+='7'):(card_id[i*2]+='0');
        card_id[i*2+1]=p[i]&0x0f;
        card_id[i*2+1]>9?(card_id[i*2+1]+='7'):(card_id[i*2+1]+='0');
    }
    card_id[8]=0;
}

int8_t ReadID(uint32_t* card_id)//����
{
    unsigned char status;

    status = PcdRequest(PICC_REQALL,CT);/*����*/
    if(status==MI_OK)//�����ɹ�
    {
        status = PcdAnticoll(get_card_id_array);/*����ײ*/         
    }
    if (status==MI_OK)//���nײ�ɹ�
    {        
        status=MI_ERR;
        /* ����ȡ���Ŀ�������ת��Ϊ32λ���� */
        *card_id =     get_card_id_array[3]+(get_card_id_array[2]<<8)+
                    (get_card_id_array[1]<<16)+(get_card_id_array[0]<<24);
        return 0;
    }
    return -1;
}
