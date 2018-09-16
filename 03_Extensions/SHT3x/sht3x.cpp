/*************************************************************************
* sht3x.cpp
*
* Created: 10.9.2018 20:50:51
* Revised:
* Author: LeXa
*
* ABOUT:
*
*************************************************************************/

#include "sht3x.h"

SHT3X::SHT3X(TWI_t *psTwi, uint32_t unFBaud, uint32_t unFCpu)
{
    m_psTwi = psTwi;
    
    TWI_MASTER_BAUD(m_psTwi, unFCpu, unFBaud);
    TWI_MASTER_ENABLE(m_psTwi);
    TWI_MASTER_SET_BUS_IDLE(m_psTwi);
}

void SHT3X::SingleShotMeasure(SHT3X_SINGLESHOT_ACCURACY eAccuracy)
{
    uint8_t aData[6];
    uint16_t unMesTempC, unMesHum;
    
    cli();
    
    /* Measure temperature and humidity */
    TWI_MASTER_WAIT_BUSY(m_psTwi);
    
    m_psTwi->MASTER.ADDR = SHT3X_ADDR & 0xFE;                   /* Address for writing */
    TWI_MASTER_WAIT_WRITE_DONE(m_psTwi);                        /* Wait until address is sent */
    
    if (TWI_MASTER_NACK_FLAG(m_psTwi) || TWI_MASTER_ARBLOST_FLAG(m_psTwi)) {return;}    /* No acknowledge or Arbitration lost */
    TWI_MASTER_CLEAR_WRITE_FLAG(m_psTwi);                       /* Clear flag */
    
    m_psTwi->MASTER.DATA = 0x2C;                                /* Clock stretching enabled */
    TWI_MASTER_WAIT_WRITE_DONE(m_psTwi);                        /* Wait until byte is sent */
    TWI_MASTER_CLEAR_WRITE_FLAG(m_psTwi);                       /* Clear flag */
    
    m_psTwi->MASTER.DATA = eAccuracy;                           /* Accuracy */
    TWI_MASTER_WAIT_WRITE_DONE(m_psTwi);                        /* Wait until byte is sent */
    TWI_MASTER_CLEAR_WRITE_FLAG(m_psTwi);                       /* Clear flag */
    
    TWI_MASTER_STOP(m_psTwi);                                   /* Stop communication */
    
    /* Read measured temperature and humidity */
    TWI_MASTER_WAIT_BUSY(m_psTwi);
    
    m_psTwi->MASTER.ADDR = (SHT3X_ADDR & 0xFE)|0x01;            /* Address for reading */
    TWI_MASTER_WAIT_READ_DONE(m_psTwi);                         /* Wait until address is sent */
    
    if (TWI_MASTER_NACK_FLAG(m_psTwi) || TWI_MASTER_ARBLOST_FLAG(m_psTwi)) {return;}    /* No acknowledge or Arbitration lost */
    
    for (uint8_t i=0; i<6; i++)
    {
        TWI_MASTER_WAIT_READ_DONE(m_psTwi);                     /* Wait until data received */
        TWI_MASTER_CLEAR_READ_FLAG(m_psTwi);                    /* Clear flag */
        aData[i] = m_psTwi->MASTER.DATA;                        /* Read data from register */
        if (i<5) {TWI_MASTER_ACKM(m_psTwi);}                    /* Acknowledge */
        else {TWI_MASTER_NACKM(m_psTwi);}                       /* No acknowledge */
    }
    
    TWI_MASTER_STOP(m_psTwi);                                   /* Stop communication */
    sei();
    
    /* Calculate measured temperature and humidity */
    unMesTempC = (uint16_t)aData[0]<<8|aData[1];
    unMesHum = (uint16_t)aData[3]<<8|aData[4];
    
    m_fTempC = ((175*(float)unMesTempC)/0xFFFF)-45;
    m_fHum = (100*(float)unMesHum)/0xFFFF;
}
