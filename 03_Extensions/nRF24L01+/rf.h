/*************************************************************************
 * rf.h
 *
 * Created: 11.10.2014 16:04:48
 * Revised: 24.6.2018
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 *
 *************************************************************************/

#ifndef RF_H_
#define RF_H_

#include <core/drivers/spi.h>
#include <conf/conf_board.h>

/************************************************************************/
/* PORT AND PIN CONFIGURATION                                           */
/*----------------------------------------------------------------------*/
/* RF_PORT      - port connected to nRF24L01+ (PORTx)                   */
/* RF_SPI       - spi module must be same like port (SPIx)              */
/* RF_BAUD_SPI  - baud rate (bit/s)                                     */
/* RF_IRQ_PIN   - IRQ pin number (PINx_bp)                              */
/* RF_CE_PIN    - CE pin number (PINx_bp)                               */
/* RF_IRQ_vect  - interrupt vector (PORTx_INT0_vect)                    */
/************************************************************************/
#ifndef RF_PORT
    #define RF_PORT         PORTC
#endif
#ifndef RF_SPI
    #define RF_SPI          SPIC
#endif
#ifndef RF_BAUD_SPI
    #define RF_BAUD_SPI     1000000
#endif
#ifndef RF_IRQ_PIN
    #define RF_IRQ_PIN      PIN0_bp
#endif
#ifndef RF_CE_PIN
    #define RF_CE_PIN       PIN1_bp
#endif

/************************************************************************/
/* CE PIN CONTROL                                                       */
/************************************************************************/
#define RF_CE_ENABLE            RF_PORT.OUTSET = 1<<RF_CE_PIN
#define RF_CE_DISABLE           RF_PORT.OUTCLR = 1<<RF_CE_PIN

/************************************************************************/
/* STATIC_PAYLOAD OR DYNAMIC_PAYLOAD                                    */
/************************************************************************/
#ifndef DYNAMIC_PAYLOAD
    #define DYNAMIC_PAYLOAD                 false
#endif

#ifndef STATIC_PAYLOAD_BUFFER_LENGTH
    #define STATIC_PAYLOAD_BUFFER_LENGTH    4
#endif

/************************************************************************/
/* AUTO ACKNOWLEGMENT                                                   */
/************************************************************************/
#ifndef ENABLE_AUTOACK
    #define ENABLE_AUTOACK                  true
#endif

/************************************************************************/
/* ENABLE DATA PIPES                                                    */
/*----------------------------------------------------------------------*/
/* 0 - disable data pipe                                                */
/* 1 - enable data pipe                                                 */
/************************************************************************/
#ifndef DATA_PIPE0_EN
    #define DATA_PIPE0_EN   1
#endif

#ifndef DATA_PIPE1_EN
    #define DATA_PIPE1_EN   0
#endif

#ifndef DATA_PIPE2_EN
    #define DATA_PIPE2_EN   0
#endif

#ifndef DATA_PIPE3_EN
    #define DATA_PIPE3_EN   0
#endif

#ifndef DATA_PIPE4_EN
    #define DATA_PIPE4_EN   0
#endif

#ifndef DATA_PIPE5_EN
    #define DATA_PIPE5_EN   0
#endif

/************************************************************************/
/* COMMANDS                                                             */
/************************************************************************/
#define R_REGISTER          0x00
#define W_REGISTER          0x20
#define R_RX_PAYLOAD        0x61
#define W_TX_PAYLOAD        0xA0
#define FLUSH_TX            0xE1
#define FLUSH_RX            0xE2
#define REUSE_TX_PL         0xE3
#define R_RX_PL_WID         0x60
#define W_ACK_PAYLOAD       0xA8
#define W_TX_PAYLOAD_NOACK  0xB0
#define NO_OPERATION        0xFF

/************************************************************************/
/* REGISTER ADDRESS                                                     */
/************************************************************************/
#define RF_REG_CONFIG       0x00
#define RF_REG_EN_AA        0x01
#define RF_REG_EN_RXADDR    0x02
#define RF_REG_SETUP_AW     0x03
#define RF_REG_SETUP_RETR   0x04
#define RF_REG_RF_CH        0x05
#define RF_REG_RF_SETUP     0x06
#define RF_REG_STATUS       0x07
#define RF_REG_OBSERVE_TX   0x08
#define RF_REG_RPD          0x09
#define RF_REG_RX_ADDR_P0   0x0A
#define RF_REG_RX_ADDR_P1   0x0B
#define RF_REG_RX_ADDR_P2   0x0C
#define RF_REG_RX_ADDR_P3   0x0D
#define RF_REG_RX_ADDR_P4   0x0E
#define RF_REG_RX_ADDR_P5   0x0F
#define RF_REG_TX_ADDR      0x10
#define RF_REG_RX_PW_P0     0x11
#define RF_REG_RX_PW_P1     0x12
#define RF_REG_RX_PW_P2     0x13
#define RF_REG_RX_PW_P3     0x14
#define RF_REG_RX_PW_P4     0x15
#define RF_REG_RX_PW_P5     0x16
#define RF_REG_FIFO_STATUS  0x17
#define RF_REG_DYNPD        0x1C
#define RF_REG_FEATURE      0x1D

/************************************************************************/
/* STATUS REGISTER                                                      */
/************************************************************************/
enum RF_REG_STATUS_enum {
    TX_FULL_bp = 0,
    RX_P_NO_bm = 0x0E,
    MAX_RT_bp = 4,
    TX_DS_bp = 5,
    RX_DR_bp = 6
};

/************************************************************************/
/* CONFIG REGISTER                                                      */
/************************************************************************/
enum RF_REG_CONFIG_enum {
    PRIM_RX_bp,
    PWR_UP_bp,
    CRCO_bp,
    EN_CRC_bp,
    MASK_MAX_RT_bp,
    MASK_TX_DS_bp,
    MASK_RX_DR_bp,
};

/************************************************************************/
/* RF SETUP REGISTER                                                    */
/************************************************************************/
#define RF_PWR_bm               (3<<1)
#define RF_DR_bm                (5<<3)
enum RF_REG_RF_SETUP_enum {
    PLL_LOCK_bp =               4,
    CONT_WAVE_bp =              7
};

enum RF_PWR_enum {
    RF_PWR_minus18dBm_gc =      (0<<1),
    RF_PWR_minus12dBm_gc =      (1<<1),
    RF_PWR_minus6dBm_gc =       (2<<1),
    RF_PWR_0dBm_gc =            (3<<1),
};

enum RF_BAUD_enum  {
    RF_BAUD_1Mbps_gc =          (0<<3)|(0<<5),
    RF_BAUD_250kbps_gc =        (1<<5),
    RF_BAUD_2Mbps_gc =          (1<<3),
};

/************************************************************************/
/* FEATURE REGISTER                                                     */
/************************************************************************/
enum RF_REG_FEATURE_enum {
    EN_DYN_ACK_bp,
    EN_ACK_PAY_bp,
    EN_DPL_bp,
};

#define RF_PAYLOAD_MAX_BYTES    32
#define RF_CHANNEL_TOTAL        126     /* channel 0 - 125 */

/************************************************************************/
/* RF STATUS                                                            */
/************************************************************************/
enum RF_STATUS_enum {
    RF_STATUS_SLEEP,
    RF_STATUS_RECEIVING,
    RF_STATUS_SENDING,
    RF_STATUS_TRANSMIT_OK,
    RF_STATUS_RECEIVE_OK,
    RF_STATUS_MAX_RETRANSMIT,
};

/************************************************************************/
/* STRUCTURES                                                           */
/************************************************************************/
struct PAYLOAD_BUFFER_struct {
    uint8_t punBuffer[RF_PAYLOAD_MAX_BYTES];
    uint8_t unCounter;
    uint8_t unDataPipeNum;
};

/************************************************************************/
/* CLASS                                                                */
/************************************************************************/
class RF
{
    private:
                
    public:
        uint8_t m_unChannel = 2;
        RF_BAUD_enum m_eBaud = RF_BAUD_2Mbps_gc;
        RF_PWR_enum m_ePwrTx = RF_PWR_0dBm_gc;
        RF_STATUS_enum m_eStatus = RF_STATUS_SLEEP;
        PAYLOAD_BUFFER_struct m_sPayload;
        
        RF();
        void WriteReg(uint8_t unReg, uint8_t unVal);
        uint8_t ReadReg(uint8_t unReg);
        uint8_t SendCommand(uint8_t unCommand);
        void SendMsg(uint8_t *punData, uint8_t unLength);
        
        /**
        * \brief Interrupt caused by pin IRQ
        * 
        * 
        * \return void
        */
        void InterruptHandler()
        {
            uint8_t unStatus;
            uint8_t unResult;
            unStatus = ReadReg(RF_REG_STATUS);
            
            /* DATA RECEIVED */
            if (unStatus & (1<<RX_DR_bp))
            {
                /* Data pipe number where data stored */
                m_sPayload.unDataPipeNum = (unStatus & RX_P_NO_bm)>>1;
                
                /* Read number of data in RX FIFO */
                spi_cs_enable(RF_PORT);
                spi_send(RF_SPI,R_RX_PL_WID);
                spi_send(RF_SPI,NO_OPERATION);
                m_sPayload.unCounter = spi_read(RF_SPI);
                spi_cs_disable(RF_PORT);
                
                /* If payload is bigger then 32 bytes, something is wrong */
                if (m_sPayload.unCounter > RF_PAYLOAD_MAX_BYTES) {m_sPayload.unCounter = 0;}
                
                /* Read data from RX FIFO */
                spi_cs_enable(RF_PORT);
                spi_send(RF_SPI,R_RX_PAYLOAD);
                for (uint8_t i=0; i<m_sPayload.unCounter; i++)
                {
                    spi_send(RF_SPI,NO_OPERATION);
                    m_sPayload.punBuffer[i] = spi_read(RF_SPI);
                }
                spi_cs_disable(RF_PORT);
                
                m_eStatus = RF_STATUS_RECEIVE_OK;
            }
            
            /* DATA TRANSMITTED */
            if (unStatus & (1<<TX_DS_bp))
            {
                RF_CE_DISABLE;
                m_eStatus = RF_STATUS_TRANSMIT_OK;
            }
            
            /* MAXIMUM RETRANSMITS */
            if (unStatus & (1<<MAX_RT_bp))
            {
                RF_CE_DISABLE;
                m_eStatus = RF_STATUS_MAX_RETRANSMIT;
                
            }
            
            /* Clear flags */
            WriteReg(RF_REG_STATUS,1<<TX_DS_bp|1<<RX_DR_bp|1<<MAX_RT_bp);
            /* Flush buffers */
            SendCommand(FLUSH_TX);
            SendCommand(FLUSH_RX);
            
            /* Set to RX mode */
            unResult = ReadReg(RF_REG_CONFIG);
            WriteReg(RF_REG_CONFIG, unResult|1<<PRIM_RX_bp);
            RF_CE_ENABLE;
        }
        
        void Init();
        void WakeUp();
        void Sleep();
        void SetRXaddress(uint8_t *punAddress, uint8_t unLength, uint8_t unPipe);
        void SetTXaddress(uint8_t *punAddress, uint8_t unLength);
        void SetChannel(uint8_t unChNumber);
        void SetPowerTx(RF_PWR_enum ePwr);
        void SetBaud(RF_BAUD_enum eBaud);
};
#endif /* RF_H_ */