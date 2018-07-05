/*************************************************************************
 * usart.cpp
 *
 * Created: 11.10.2014 21:24:08
 * Revised: 17.5.2015
 * Author: LeXa
 * BOARD: 
 *
 * ABOUT:
 *
 *************************************************************************/

#include "usart.h"

USART::USART(USART_t *psUsart, uint32_t unFcpu, uint32_t unBaud, USART_CHSIZE_enum eChSize, USART_PMODE_enum eParity)
{
    PORT_t *psPort;
    uint16_t prescale;
    uint8_t unTxPin,unRxPin;

    m_psUsart = psUsart;

    if (m_psUsart == &USARTC0) {psPort = &PORTC; unTxPin = PIN3_bm; unRxPin = PIN2_bm;}
    #ifdef USARTC1
    else if (m_psUsart == &USARTC1) {psPort = &PORTC; unTxPin = PIN7_bm; unRxPin = PIN6_bm;}
    #endif
    #ifdef USARTD0
    else if (m_psUsart == &USARTD0) {psPort = &PORTD; unTxPin = PIN3_bm; unRxPin = PIN2_bm;}
    #endif
    #ifdef USARTD1
    else if (m_psUsart == &USARTD1) {psPort = &PORTD; unTxPin = PIN7_bm; unRxPin = PIN6_bm;}
    #endif
    #ifdef USARTE0
    else if (m_psUsart == &USARTE0) {psPort = &PORTE; unTxPin = PIN3_bm; unRxPin = PIN2_bm;}
    #endif
    #ifdef USARTE1
    else if (m_psUsart == &USARTE1) {psPort = &PORTE; unTxPin = PIN7_bm; unRxPin = PIN6_bm;}
    #endif
    #ifdef USARTF0
    else if (m_psUsart == &USARTF0) {psPort = &PORTF; unTxPin = PIN3_bm; unRxPin = PIN2_bm;}
    #endif
    #ifdef USARTF1
    else if (m_psUsart == &USARTF1) {psPort = &PORTF; unTxPin = PIN7_bm; unRxPin = PIN6_bm;}
    #endif
    
    /* USART configuration */
    m_psUsart->CTRLA = USART_RXCINTLVL_HI_gc|USART_TXCINTLVL_OFF_gc|USART_DREINTLVL_OFF_gc;
    m_psUsart->CTRLB = USART_RXEN_bm|USART_TXEN_bm;
    m_psUsart->CTRLC = eChSize|eParity|USART_CMODE_ASYNCHRONOUS_gc|0<<USART_SBMODE_bp;
    
    /* Baudrate configuration */
    prescale = (uint16_t)(unFcpu/16/unBaud-1) & 0x0FFF;
    m_psUsart->BAUDCTRLA = (uint8_t) (prescale);
    m_psUsart->BAUDCTRLB = (uint8_t) (prescale>>8);
    
    /* PORT configuration */
    psPort->OUTSET = unTxPin;           /* Standby value for TX pin */
    psPort->DIRSET = unTxPin;           /* Output pins */
    psPort->DIRCLR = unRxPin;           /* Input pins */
}

void USART::Send(uint8_t unByte)
{   
    m_psUsart->DATA = unByte;                               /* Send byte */
    while (!(m_psUsart->STATUS & USART_TXCIF_bm));          /* Wait until byte is transmitted */
    m_psUsart->STATUS |= USART_TXCIF_bm;                    /* Clear flag */
}

void USART::Send(uint8_t *punBytes, uint16_t unCnt)
{
    do
    {
        m_psUsart->DATA = *(punBytes++);                    /* Send byte */
        while (!(m_psUsart->STATUS & USART_TXCIF_bm));      /* Wait until byte is transmitted */
        m_psUsart->STATUS |= USART_TXCIF_bm;                /* Clear flag */
    } while (--unCnt);
}

void USART::Send(uint8_t *psString)
{
    do
    {
        if (!*psString) {break;}
        m_psUsart->DATA = *(psString++);                    /* Send character */
        while (!(m_psUsart->STATUS & USART_TXCIF_bm));      /* Wait until character is transmitted */
        m_psUsart->STATUS |= USART_TXCIF_bm;                /* Clear flag */
        if (*(psString) == 0x00) {break;}
    } while (1);
}

uint8_t USART::Read()
{
    while(!(m_psUsart->STATUS & USART_RXCIF_bm));           /* Wait until byte is received */
    m_psUsart->STATUS |= USART_RXCIF_bm;                    /* Clear flag */
    return m_psUsart->DATA;                                 /* Store byte */
}

void USART::Read(uint8_t *punBytes, uint16_t unCnt)
{
    do 
    {
        while(!(m_psUsart->STATUS & USART_RXCIF_bm));       /* Wait until byte is received */
        m_psUsart->STATUS |= USART_RXCIF_bm;                /* Clear flag */
        *(punBytes++) = m_psUsart->DATA;                    /* Store byte */
    } while (--unCnt);
}

void USART::Read(uint8_t *psString)
{
    do
    {
        while(!(m_psUsart->STATUS & USART_RXCIF_bm));       /* Wait until character is received */
        m_psUsart->STATUS |= USART_RXCIF_bm;                /* Clear flag */
        *(psString++) = m_psUsart->DATA;                    /* Store character and increment pointer */     
        if (*(psString-1) == '\n' || *(psString-1) == '\r' || !*(psString-1)) {break;}
    } while (1);
}
