/*************************************************************************
 * bmp.cpp
 *
 * Created: 6.5.2015 14:27:36
 * Revised: 1.4.2018
 * Author: LeXa
 *
 * ABOUT:
 * Library for temperature and pressure sensor BMP180
 *************************************************************************/

#include "bmp180.h"
#include <drivers/twi.h>
#include <avr/interrupt.h>
#include <conf/conf_board.h>

BMP180::BMP180(TWI_t *psTwi, uint32_t unFcpu, uint32_t unFtwi)
{
    uint8_t data[22];
    
    m_psTwi = psTwi;
    
    TWI_MASTER_BAUD(m_psTwi, unFcpu, unFtwi);
    TWI_MASTER_ENABLE(m_psTwi);
    TWI_MASTER_SET_BUS_IDLE(m_psTwi);
    
    ReadReg(BMP180_REG_CAL_DATA,data,22);               /* Read calibration data */
    m_sCaldata.AC1 = (uint16_t)data[0]<<8|data[1];
    m_sCaldata.AC2 = (uint16_t)data[2]<<8|data[3];
    m_sCaldata.AC3 = (uint16_t)data[4]<<8|data[5];
    m_sCaldata.AC4 = (uint16_t)data[6]<<8|data[7];
    m_sCaldata.AC5 = (uint16_t)data[8]<<8|data[9];
    m_sCaldata.AC6 = (uint16_t)data[10]<<8|data[11];
    m_sCaldata.B1 = (uint16_t)data[12]<<8|data[13];
    m_sCaldata.B2 = (uint16_t)data[14]<<8|data[15];
    m_sCaldata.MB = (uint16_t)data[16]<<8|data[17];
    m_sCaldata.MC = (uint16_t)data[18]<<8|data[19];
    m_sCaldata.MD = (uint16_t)data[20]<<8|data[21]; 
}

void BMP180::WriteReg(BMP180_REG_enum eReg, uint8_t byte)
{
    cli();
    TWI_MASTER_WAIT_BUSY(m_psTwi);
    
    m_psTwi->MASTER.ADDR = BMP180_ADDR & 0xFE;                  /* Address for writing */
    TWI_MASTER_WAIT_WRITE_DONE(m_psTwi);                        /* Wait until address is sent */
    TWI_MASTER_CLEAR_WRITE_FLAG(m_psTwi);                       /* Clear flag */
    
    if (TWI_MASTER_ARBLOST_FLAG(m_psTwi)) {return;}             /* Arbitration lost */
    TWI_MASTER_CLEAR_WRITE_FLAG(m_psTwi);                       /* Clear flag */
    
    m_psTwi->MASTER.DATA = eReg;                                /* Register address */
    TWI_MASTER_WAIT_WRITE_DONE(m_psTwi);                        /* Wait until byte is sent */
    TWI_MASTER_CLEAR_WRITE_FLAG(m_psTwi);                       /* Clear flag */
    
    m_psTwi->MASTER.DATA = byte;                                /* Write data to register */
    TWI_MASTER_WAIT_WRITE_DONE(m_psTwi);                        /* Wait until byte is sent */
    TWI_MASTER_CLEAR_WRITE_FLAG(m_psTwi);                       /* Clear flag */
    
    TWI_MASTER_STOP(m_psTwi);                                   /* Stop communication */
    sei();
}

void BMP180::ReadReg(BMP180_REG_enum eReg, uint8_t *data, uint8_t num)
{
    cli();
    TWI_MASTER_WAIT_BUSY(m_psTwi);
    
    m_psTwi->MASTER.ADDR = BMP180_ADDR & 0xFE;                  /* Write address */
    TWI_MASTER_WAIT_WRITE_DONE(m_psTwi);                        /* Wait until address is sent */
        
    if (TWI_MASTER_ARBLOST_FLAG(m_psTwi)) {return;}             /* Arbitration lost */
    TWI_MASTER_CLEAR_WRITE_FLAG(m_psTwi);                       /* Clear flag */
        
    m_psTwi->MASTER.DATA = eReg;                                /* Register address */
    TWI_MASTER_WAIT_WRITE_DONE(m_psTwi);                        /* Wait until byte is sent */
    TWI_MASTER_CLEAR_WRITE_FLAG(m_psTwi);                       /* Clear flag */
    
    m_psTwi->MASTER.ADDR = (BMP180_ADDR & 0xFE)|0x01;           /* Address for reading */
    
    for (uint8_t i=0; i<num; i++)
    {
        TWI_MASTER_WAIT_READ_DONE(m_psTwi);                     /* Wait until data received */
        TWI_MASTER_CLEAR_READ_FLAG(m_psTwi);                    /* Clear flag */
        data[i] = m_psTwi->MASTER.DATA;                         /* Read data from register */
        
        TWI_MASTER_ACKM(m_psTwi);                               /* Acknowledge */
    }
    
    TWI_MASTER_STOP(m_psTwi);                                   /* Stop communication */
    sei();
}

int16_t BMP180::CalcTemp()
{
    uint8_t data[2];
    
    ReadReg(BMP180_REG_ADC,data,2);
    bmp180_adc = (uint32_t)data[0]<<8|data[1];
    
    x1 = (((int32_t)bmp180_adc - (int32_t)m_sCaldata.AC6)*(int32_t)m_sCaldata.AC5)>>15;
    x2 = ((int32_t)m_sCaldata.MC<<11)/(x1 + (int32_t)m_sCaldata.MD);
    b5 = x1+x2;
    return m_nTemp = (int16_t)((b5+8)>>4);
}

int32_t BMP180::CalcPress(SAMPLE_ACCURACY_enum eSmplAccy)
{
    uint8_t data[3];
    
    ReadReg(BMP180_REG_ADC,data,3);
    bmp180_adc = (uint32_t)data[0]<<16|(uint32_t)data[1]<<8|data[2];
    bmp180_adc >>= 8-eSmplAccy;
    
    b6 = b5 - 4000;
    x3 = (((int32_t)m_sCaldata.B2*(b6*b6>>12))>>11)+((int32_t)m_sCaldata.AC2*b6>>11);
    b3 = ((((int32_t)m_sCaldata.AC1*4+x3)<<eSmplAccy)+2)>>2;
    x3 = (((int32_t)m_sCaldata.AC3*b6>>13)+(((int32_t)m_sCaldata.B1*(b6*b6>>12))>>16)+2)>>2;
    b4 = (uint32_t)m_sCaldata.AC4*(x3+32768)>>15;
    b7 = ((uint32_t)bmp180_adc-b3)*(50000>>eSmplAccy);
    if (b7 < 0x80000000) {p = (b7<<1)/b4;}  else {p = (b7/b4)<<2;}
    p += ((((p>>8)*(p>>8)*3038)>>16)+(((-7357*p)>>16)+3791)>>4);
    
    return m_nPress = p;
}