/*************************************************************************
 * sd.h
 *
 * Created: 27.5.2015 21:01:13
 * Revised: 1.4.2018
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 *
 *************************************************************************/

#include <avr/io.h>
#include <drivers/spi.h>
#include <conf.h>

#ifndef SD_H_
#define SD_H_

/*===== COMPILER SWICHTES ==================================================
 * Uncomment if needed.
 *==========================================================================*/
#ifndef CONF_H_
    #define COMPILE_SD_INSERT_DETECT
#endif


/*===== PORT AND PIN CONFIGURATION ========================================
 *==========================================================================*/
#ifndef SD_PORT
    #define SD_PORT                 PORTC
#endif
#ifndef SD_SPI
    #define SD_SPI                  SPIC
#endif
#ifndef SD_BAUD
    #define SD_BAUD                 16000000
#endif
#ifndef SD_SWITCH_PIN
    #define SD_SWITCH_PIN           PIN1_bp
#endif


/*===== DATA BLOCK SIZE ===================================================
*==========================================================================*/
#define SD_BLOCK_SIZE               512


/*===== SD COMMANDS ========================================================
 *==========================================================================*/
;enum SD_CMD_enum {
    SD_CMD_GO_IDLE_STATE                = 0 ,       /* CMD0 */
    SD_CMD_SEND_OP_COND                 = 1 ,       /* CMD1 */
    SD_CMD_SEND_IF_COND                 = 8 ,       /* CMD8 */
    SD_CMD_SEND_CSD                     = 9 ,       /* CMD9 */
    SD_CMD_STOP_TRANSMISSION            = 12,       /* CMD12 */
    SD_CMD_SEND_STATUS		            = 13,       /* CMD13 */
    SD_CMD_SET_BLOCK_LEN	            = 16,       /* CMD16 */
    SD_CMD_READ_SINGLE_BLOCK            = 17,       /* CMD17 */
    SD_CMD_READ_MULTIPLE_BLOCKS         = 18,       /* CMD18 */
    SD_CMD_WRITE_SINGLE_BLOCK           = 24,       /* CMD24 */
    SD_CMD_WRITE_MULTIPLE_BLOCKS        = 25,       /* CMD25 */
    SD_CMD_ERASE_BLOCK_START_ADDR       = 32,       /* CMD32 */
    SD_CMD_ERASE_BLOCK_END_ADDR         = 33,       /* CMD33 */
    SD_CMD_ERASE_SELECTED_BLOCKS        = 38,       /* CMD38 */
    SD_CMD_APP_CMD                      = 55,       /* CMD55 */
    SD_CMD_READ_OCR                     = 58,       /* CMD58 */
    SD_CMD_CRC_ON_OFF                   = 59,       /* CMD59 */
};


/*===== RETURN R1 ERRORS ===================================================
 *==========================================================================*/
enum R1_ERROR_enum {
    R1_NO_ERROR             = (0<<0),
    R1_IDLE_STATE_bm        = (1<<0),
    R1_ERASE_RESET_bm       = (1<<1),
    R1_ILLEGAL_COMMAND_bm   = (1<<2),
    R1_COM_CRC_ERROR_bm     = (1<<3),
    R1_ERASE_SEQ_ERROR_bm   = (1<<4),
    R1_ADDRESS_ERROR_bm     = (1<<5),
    R1_PARAMETER_ERROR_bm   = (1<<6),
    R1_INVALID_ERROR_bm     = 0xFF,
};


/*===== DATA ANSWER ========================================================
 *==========================================================================*/
#define DATA_ANSWER_bm       0x0E
enum DATA_ANSWER_enum {
    DATA_ACCEPT_gc =        (0x02<<1),
    DATA_CRC_ERROR_gc =     (0x05<<1),
    DATA_WRITE_ERROR_gc =   (0x06<<1),
};


/*===== SD CARD TYPE =======================================================
 *==========================================================================*/
enum SD_CARD_TYPE_enum {
    SD_CARD_TYPE_NONE,
    SD_CARD_TYPE_SDSC,
    SD_CARD_TYPE_SDHC_SDXC,
};


/*===== CLASS ==============================================================
 *==========================================================================*/
class SD
{
    private:
        SD_CARD_TYPE_enum m_eCardType;
        bool m_bCardInitialized;
        
    public:
        SD();
        bool Init();
        R1_ERROR_enum SendCommand(SD_CMD_enum eCmd, uint32_t unAttrib, uint8_t unCrc);
        bool ReadBlock(uint32_t unBlockNum, uint8_t *punBuffer);
        bool WriteBlock(uint32_t unBlockNum, uint8_t *punBuffer);
        bool EraseBlocks(uint32_t unStartBlock, uint32_t unTotalBlocks);
};

#endif /* SD_H_ */