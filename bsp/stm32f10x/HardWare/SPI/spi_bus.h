/**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: spi_bus.h
**
** ��   ��   ��: �Ž���
**
** �ļ���������: 2016 �� 09 �� 06 ��
**
** ��        ��: stm32 spi��غ���

** ��־:
2016.09.06  �������ļ�
*********************************************************************************************************/
#ifndef _SPI_BUS_H_
#define _SPI_BUS_H_
#include "stm32f10x.h"
#include <drivers/spi.h>

struct stm32_spi_bus
{
    struct rt_spi_bus parent;
    SPI_TypeDef * SPI;
};

struct stm32_spi_cs
{
    GPIO_TypeDef * GPIOx;
    uint16_t GPIO_Pin;
};

extern void rt_hw_stm32_spi_bus_init(void);
     
#endif

