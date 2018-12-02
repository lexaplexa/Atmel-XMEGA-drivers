/****************************************************************************
 * ds18b20.h
 *
 * Created: 18.4.2017 16:44:17
 * Revised: 16.9.2018
 * Author: LeXa
 * BOARD:
 * ABOUT:
 *
 *****************************************************************************/


#ifndef DS18B20_H_
#define DS18B20_H_

#include <core/drivers/usart.h>

/************************************************************************/
/* CLASS                                                                */
/************************************************************************/
class DS18B20
{
    private:
        USART_t *m_psUsart;
        PORT_t *m_psPort;
        uint32_t m_unFCpu;
        uint8_t m_aTempData[2];
    
    public:
        DS18B20(USART_t *psUsart, PORT_t *psPort, uint32_t unFCpu);
        bool Write(uint8_t *pData, uint8_t unDataLen);
        bool Read(uint8_t *pDataToWrite, uint8_t unDataWriteLen, uint8_t *pDataToRead, uint8_t unDataReadLen);
        void MeasTemp();
        void ReadTemp();
        float GetTempC();
};

#endif /* 1WIRE_UART_H_ */