/*************************************************************************
 * spi.h
 *
 * Created: 9.10.2014 19:39:03
 * Revised: 12.2.2016
 * Author: LeXa
 * BOARD: 
 *
 * ABOUT:
 * 
 *************************************************************************/

#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>

/* Set options */
#define spi_enable_master(spi_ch)           spi_ch.CTRL |= SPI_ENABLE_bm|SPI_MASTER_bm
#define spi_enable_slave(spi_ch)            spi_ch.CTRL = spi_ch.CTRL & ~SPI_MASTER_bm | SPI_ENABLE_bm
#define spi_disable(spi_ch)                 spi_ch.CTRL &= ~(SPI_ENABLE_bm)
#define spi_set_mode(spi_ch,mode)           spi_ch.CTRL &= ~(SPI_MODE_gm)|mode
#define spi_set_dord(spi_ch,dord)           spi_ch.CTRL &= ~(SPI_DORD_bm)|dord

/* Send and read data */
#define spi_send(spi_ch,data)               spi_ch.DATA = data; \
                                            while (!(spi_ch.STATUS & SPI_IF_bm))
#define spi_read(spi_ch)                    spi_ch.DATA

/* Chip select */
#define spi_cs_enable(spi_port)             spi_port.OUTCLR = 1<<PIN_SS_bp
#define spi_cs_disable(spi_port)            spi_port.OUTSET = 1<<PIN_SS_bp
#define spi_cspin_enable(spi_port,pin)      spi_port.OUTCLR = 1<<pin
#define spi_cspin_disable(spi_port,pin)     spi_port.OUTSET = 1<<pin

/* Master port directions */
#define spi_set_master_port_dir(spi_port)   spi_port.DIRSET = 1<<PIN_SS_bp|1<<PIN_MOSI_bp|1<<PIN_SCK_bp; \
                                            spi_port.DIRCLR = 1<<PIN_MISO_bp
/* Slave port directions */
#define spi_set_slave_port_dir(spi_port)    spi_port.DIRSET = 1<<PIN_MISO_bp; \
                                            spi_port.DIRCLR = 1<<PIN_SS_bp|1<<PIN_MOSI_bp|1<<PIN_SCK_bp


/*===== SPI PINS ========================================================*/
#define PIN_SS_bp       PIN4_bp
#define PIN_MOSI_bp     PIN5_bp
#define PIN_MISO_bp     PIN6_bp
#define PIN_SCK_bp      PIN7_bp

/*===== SPI DATA ORDER ==================================================*/
enum SPI_DORD_enum{
    SPI_DORD_MSB_FIRST = 0<<SPI_DORD_bp,
    SPI_DORD_LSB_FIRST = 1<<SPI_DORD_bp
    };

/*===== SPI CLASS =======================================================*/
class SPI
{
    private:
        SPI_t *m_psSpi;
    public:
        /**
         * \brief Initialize SPI Master
         * 
         * \param psPort
         * \param psSPIChannel
         * \param unFcpu
         * \param unBaud
         * \param eMode
         * \param eDataOrder
         * 
         * \return 
         */
        SPI(PORT_t *psPort, SPI_t *psSPIChannel, uint32_t unFcpu, uint32_t unBaud, SPI_MODE_enum eMode, SPI_DORD_enum eDataOrder)
        {
            m_psSpi = psSPIChannel;
            
            psPort->DIRSET = 1<<PIN_SS_bp|1<<PIN_MOSI_bp|1<<PIN_SCK_bp;     /* Output pins */
            psPort->DIRCLR = 1<<PIN_MISO_bp;                                /* Input pins */
            m_psSpi->CTRL = SPI_ENABLE_bm|1<<SPI_MASTER_bp|eMode|eDataOrder;
            
            /* Baudrate */
            if (unBaud <= unFcpu/128) {m_psSpi->CTRL |= SPI_PRESCALER_DIV128_gc;}
            else if (unBaud <= unFcpu/64) {m_psSpi->CTRL |= SPI_PRESCALER_DIV64_gc;}
            else if (unBaud <= unFcpu/32) {m_psSpi->CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV64_gc;}
            else if (unBaud <= unFcpu/16) {m_psSpi->CTRL |= SPI_PRESCALER_DIV16_gc;}
            else if (unBaud <= unFcpu/8) {m_psSpi->CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV16_gc;}
            else if (unBaud <= unFcpu/4) {m_psSpi->CTRL |= SPI_PRESCALER_DIV4_gc;}
            else if (unBaud <= unFcpu/2) {m_psSpi->CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV4_gc;}
            else {m_psSpi->CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV4_gc;}
        }
        
        /**
         * \brief Initialize SPI Slave
         * 
         * \param psPort
         * \param psSPIChannel
         * \param eMode
         * \param eDataOrder
         * 
         * \return 
         */
        SPI(PORT_t *psPort, SPI_t *psSPIChannel, SPI_MODE_enum eMode, SPI_DORD_enum eDataOrder)
        {
            m_psSpi = psSPIChannel;
            
            psPort->DIRSET = 1<<PIN_MISO_bp;                                /* Output pins */
            psPort->DIRCLR = 1<<PIN_SS_bp|1<<PIN_MOSI_bp|1<<PIN_SCK_bp;     /* Input pins */
            m_psSpi->CTRL = SPI_ENABLE_bm|0<<SPI_MASTER_bp|eMode|eDataOrder;
        }
        
        
        /**
         * \brief Set SPI baud rate
         * 
         * \param unFcpu
         * \param unBaud
         * 
         * \return void
         */
        void SetBaud(uint32_t unFcpu, uint32_t unBaud)
        {
            /* Clear prescaler */
            m_psSpi->CTRL &= ~(SPI_PRESCALER_gm);
            m_psSpi->CTRL &= ~(1<<SPI_CLK2X_bp);
            /* Baudrate */
            if (unBaud <= unFcpu/128) {m_psSpi->CTRL |= SPI_PRESCALER_DIV128_gc;}
            else if (unBaud <= unFcpu/64) {m_psSpi->CTRL |= SPI_PRESCALER_DIV64_gc;}
            else if (unBaud <= unFcpu/32) {m_psSpi->CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV64_gc;}
            else if (unBaud <= unFcpu/16) {m_psSpi->CTRL |= SPI_PRESCALER_DIV16_gc;}
            else if (unBaud <= unFcpu/8) {m_psSpi->CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV16_gc;}
            else if (unBaud <= unFcpu/4) {m_psSpi->CTRL |= SPI_PRESCALER_DIV4_gc;}
            else if (unBaud <= unFcpu/2) {m_psSpi->CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV4_gc;}
            else {m_psSpi->CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV4_gc;}
        }
        
        
        /**
         * \brief Send byte
         * 
         * \param unData
         * 
         * \return uint8_t
         */
        uint8_t Send(uint8_t unData)
        {
            m_psSpi->DATA = unData;
            while (!(m_psSpi->STATUS & SPI_IF_bm));
            return m_psSpi->DATA;
        }
        
        
        /**
         * \brief Send bytes
         * 
         * \param unData
         * \param unCnt
         * 
         * \return void
         */
        void Send(uint8_t *unData, uint16_t unCnt)
        {
            do {
                m_psSpi->DATA = *(unData++); while (!(m_psSpi->STATUS & SPI_IF_bm));
            } while (--unCnt);
        }
        
        
        /**
         * \brief Read byte
         * 
         * 
         * \return uint8_t
         */
        uint8_t Read()
        {
            while (!(m_psSpi->STATUS & SPI_IF_bm));
            return m_psSpi->DATA;
        }

        
        /**
         * \brief Read bytes
         * 
         * \param unData
         * \param unCnt
         * 
         * \return void
         */
        void Read(uint8_t *unData, uint16_t unCnt)
        {
            do {
                m_psSpi->DATA = 0xFF; while (!(m_psSpi->STATUS & SPI_IF_bm)); *(unData++) = m_psSpi->DATA;
            } while (--unCnt);
        }
};


#endif /* SPI_H_ */