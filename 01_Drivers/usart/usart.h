/*************************************************************************
 * usart.h
 *
 * Created: 11.10.2014 21:07:33
 * Revised: 16.9.2018
 * Author: LeXa
 * BOARD: 
 *
 * ABOUT:
 *
 *************************************************************************/


#ifndef USART_H_
#define USART_H_

#include <avr/io.h>

/************************************************************************/
/* DEFINES                                                              */
/************************************************************************/
#define usart_config(usart_ch,ChSize,Parity)                usart_ch->CTRLC = ChSize|Parity|USART_CMODE_ASYNCHRONOUS_gc|0<<USART_SBMODE_bp
#define usart_tx_enable(usart_ch)                           usart_ch->CTRLB |= USART_TXEN_bm
#define usart_rx_enable(usart_ch)                           usart_ch->CTRLB |= USART_RXEN_bm
#define usart_tx_disable(usart_ch)                          usart_ch->CTRLB &= ~USART_TXEN_bm
#define usart_rx_disable(usart_ch)                          usart_ch->CTRLB &= ~USART_RXEN_bm
                                                            
#define usart_set_interrupt(usart_ch,tx_int,rx_int,dre_int) usart_ch->CTRLA = rx_int|tx_int|dre_int;

#define usart_baud(usart_ch,Fcpu,Baud)                      usart_ch->BAUDCTRLA = (uint16_t) (Fcpu/16/Baud-1) & 0x00FF;\
                                                            usart_ch->BAUDCTRLB = (uint16_t) (((Fcpu/16/Baud-1) & 0x0F00)>>8)

#define usart_port_config(port_ch,unTxPin,unRxPin)          port_ch->OUTSET = 1<<unTxPin;\
                                                            port_ch->DIRSET = 1<<unTxPin;\
                                                            port_ch->DIRCLR = 1<<unRxPin

#define usart_send(usart_ch,byte)                           usart_ch->DATA = byte;\
                                                            while (!(usart_ch->STATUS & USART_TXCIF_bm));\
                                                            usart_ch->STATUS |= USART_TXCIF_bm

#define usart_read(usart_ch,byte)                           while(!(usart_ch->STATUS & USART_RXCIF_bm));\
                                                            usart_ch->STATUS |= USART_RXCIF_bm;\
                                                            byte = usart_ch->DATA


/************************************************************************/
/* CLASS                                                                */
/************************************************************************/
class USART
{
    private:
            
    public:
        USART_t *m_psUsart;
        
        USART(USART_t *spUsart, uint32_t unFcpu, uint32_t unBaud, USART_CHSIZE_enum eChSize, USART_PMODE_enum eParity);
        void Send(uint8_t unByte);
        void Send(uint8_t *punBytes, uint16_t unCnt);
        void Send(uint8_t *psString);
        uint8_t Read();
        void Read(uint8_t *punBytes, uint16_t unCnt);
        void Read(uint8_t *psString);
        
        void inline RxIntDisable() {m_psUsart->CTRLA |= USART_RXCINTLVL_HI_gc;}
        
        void inline RxIntEnable() {m_psUsart->CTRLA &= ~USART_RXCINTLVL_gm;}
};
                                
#endif /* USART_H_ */