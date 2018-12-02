/*****************************************************************************
 * ds18b20.cpp
 *
 * Created: 16.9.2018 16:13:57
 * Revised: 
 * Author: LeXa
 * BOARD:
 * ABOUT:
 *
 *****************************************************************************/

#include "ds18b20.h"

 DS18B20::DS18B20(USART_t *psUsart, PORT_t *psPort, uint32_t unFCpu)
{
    m_psUsart = psUsart;
    m_psPort = psPort;
    m_unFCpu = unFCpu;
    
    usart_port_config(m_psPort,PIN3_bp,PIN2_bp);
    usart_config(m_psUsart,USART_CHSIZE_8BIT_gc,USART_PMODE_DISABLED_gc);
    usart_set_interrupt(m_psUsart,USART_TXCINTLVL_OFF_gc,USART_RXCINTLVL_OFF_gc,USART_DREINTLVL_OFF_gc);
    usart_tx_enable(m_psUsart);
    usart_rx_enable(m_psUsart);
}

bool DS18B20::Write(uint8_t *pData, uint8_t unDataLen)
{
    uint8_t unRet;
    
    m_psPort->PIN3CTRL = PORT_OPC_WIREDAND_gc;
    
    /* Reset device and presence test */
    usart_rx_enable(m_psUsart);
    usart_baud(m_psUsart,m_unFCpu,9600);
    usart_send(m_psUsart,0xF0);
    usart_read(m_psUsart,unRet);
    if (unRet == 0xF0) {return false;}
    
    /* Set faster baud rate */
    usart_baud(m_psUsart,m_unFCpu,115200);
    usart_rx_disable(m_psUsart);
    m_psPort->PIN3CTRL = PORT_OPC_TOTEM_gc;
    
    /* Data write */
    for (uint8_t byte=0; byte<unDataLen; byte++)
    {
        for (uint8_t i=0; i<8; i++)
        {
            if (pData[byte] & (1<<i))   {usart_send(m_psUsart,0xFF);}
            else                        {usart_send(m_psUsart,0x00);}
        }
    }
    
    return true;
}

bool DS18B20::Read(uint8_t *pDataToWrite, uint8_t unDataWriteLen, uint8_t *pDataToRead, uint8_t unDataReadLen)
{
    uint8_t unRet;
    
    m_psPort->PIN3CTRL = PORT_OPC_WIREDAND_gc;
    
    /* Reset device and presence test */
    usart_rx_enable(m_psUsart);
    usart_baud(m_psUsart,m_unFCpu,9600);
    usart_send(m_psUsart,0xF0);
    usart_read(m_psUsart,unRet);
    if (unRet == 0xF0) {return false;}
    
    /* Set faster baud rate */
    usart_baud(m_psUsart,m_unFCpu,115200);
    usart_rx_disable(m_psUsart);
    m_psPort->PIN3CTRL = PORT_OPC_TOTEM_gc;
    
    /* Data write */
    for (uint8_t byte=0; byte<unDataWriteLen; byte++)
    {
        for (uint8_t i=0; i<8; i++)
        {
            if (pDataToWrite[byte] & (1<<i))    {usart_send(m_psUsart,0xFF);}
            else                                {usart_send(m_psUsart,0x00);}
        }
    }
    
    /* Data read */
    m_psPort->PIN3CTRL = PORT_OPC_WIREDAND_gc;
    usart_rx_enable(m_psUsart);
    for (uint8_t byte=0; byte<unDataReadLen; byte++)
    {
        for (uint8_t i=0; i<8; i++)
        {
            usart_send(m_psUsart,0xFF);
            usart_read(m_psUsart,unRet);
            if (unRet == 0xFF) {pDataToRead[byte] |= 1<<i;}
            else {pDataToRead[byte] &= ~(1<<i);}
        }
    }
    return true;
}

void DS18B20::MeasTemp()
{
    uint8_t aData[2] = {0xCC,0x44};
    Write(aData,2);
}

void DS18B20::ReadTemp()
{
    uint8_t aData[2] = {0xCC,0xBE};
    Read(aData,2,m_aTempData,2);
}

float DS18B20::GetTempC()
{
    float fTemp;
    
    fTemp = (float)(m_aTempData[0]>>4|m_aTempData[1]<<4);
    if (m_aTempData[0] & 0x08) {fTemp += 0.5;}
    if (m_aTempData[0] & 0x04) {fTemp += 0.25;}
    if (m_aTempData[0] & 0x02) {fTemp += 0.125;}
    if (m_aTempData[0] & 0x01) {fTemp += 0.0625;}
    
    return fTemp;
}
