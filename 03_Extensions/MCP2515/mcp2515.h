/*************************************************************************
 * mcp2515.h
 *
 * Created: 9.10.2014 13:56:34
 * Revised: 1.4.2018
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 * CAN transceiver MCP2515
 *************************************************************************/

#include <avr/io.h>
#include <drivers/spi.h>
#include <conf.h>

#ifndef MCP2515_H_
#define MCP2515_H_

/*===== PORT AND PIN CONFIGURATION ========================================
 *==========================================================================*/
#ifndef MCP_SPI
    #warning Default configuration MCP_SPI
    #define MCP_SPI                 SPIC
#endif
#ifndef MCP_PORT
    #warning Default configuration MCP_PORT
    #define MCP_PORT                PORTC
#endif
#ifndef MCP_INT_PIN
    #warning Default configuration MCP_INT_PIN
    #define MCP_INT_PIN             PIN3_bp
#endif
#ifndef MCP_RESET_PIN
    #warning Default configuration MCP_RESET_PIN
    #define MCP_RESET_PIN           PIN2_bp
#endif
#ifndef MCP_BAUD
    #warning Default configuration MCP_BAUD
    #define MCP_BAUD                2000000
#endif


/*===== INSTRUCTIONS =======================================================
 *==========================================================================*/
#define INST_RESET                  0xC0
#define INST_READ                   0x03
#define INST_READ_RX0_BUF_ID        0x90
#define INST_READ_RX0_BUF_DATA      0x92
#define INST_READ_RX1_BUF_ID        0x94
#define INST_READ_RX1_BUF_DATA      0x96
#define INST_WRITE                  0x02
#define INST_LOAD_TX0_BUF_ID        0x40
#define INST_LOAD_TX0_BUF_DATA      0x41
#define INST_LOAD_TX1_BUF_ID        0x42
#define INST_LOAD_TX1_BUF_DATA      0x43
#define INST_LOAD_TX2_BUF_ID        0x44
#define INST_LOAD_TX2_BUF_DATA      0x45
#define INST_RTS(num)               0x80|(1<<num)	/* Request to send CAN message. TX num = {0,1,2} */
#define INST_READ_STATUS            0xA0
#define INST_RX_STATUS              0xB0
#define INST_BIT_MODIFY             0x05
#define NO_OPERATION                0xFF


/*===== TRANSMIT REGISTERS =================================================
 *==========================================================================*/
/* ===Transmit Buffer address=== */
#define TXBCTRL(num)    0x30|(num<<4)       /* num = {0,1,2} */
#define TXBSIDH(num)    0x31|(num<<4)       /* num = {0,1,2} */
#define TXBSIDL(num)    0x32|(num<<4)       /* num = {0,1,2} */
#define TXBEIDH(num)    0x33|(num<<4)       /* num = {0,1,2} */
#define TXBEIDL(num)    0x34|(num<<4)       /* num = {0,1,2} */
#define TXBDLC(num)     0x35|(num<<4)       /* num = {0,1,2} */
#define TXBDATA(num)    0x36|(num<<4)       /* num = {0,1,2} */
#define TXRTSCTRL       0x0D

/* ===TXBCTRL register bits=== */
enum TXBCTRL_enum{
    TXBCTRL_TXP_gp = 0,
    TXBCTRL_TXREQ_bp = 3,
    TXBCTRL_TXERR_bp = 4,
    TXBCTRL_MLOA_bp = 5,
    TXBCTRL_ABTF = 6,
};

enum TXBCTRL_PRIO_enum{
    TXBCTRL_PRIO_LOW = (0<<TXBCTRL_TXP_gp),
    TXBCTRL_PRIO_LOW_INTERMEDIATE = (1<<TXBCTRL_TXP_gp),
    TXBCTRL_PRIO_HIGH_INTERMEDIATE = (2<<TXBCTRL_TXP_gp),
    TXBCTRL_PRIO_HIGH = (3<<TXBCTRL_TXP_gp),
};


/*===== RECEIVE REGISTERS =================================================
 *==========================================================================*/
/* ===Receive Buffer address=== */
#define RXBCTRL(num)    0x60|(num<<4)       /* num = {0,1} */
#define BFPCTRL         0xC0
#define RXBSIDH(num)    0x61|(num<<4)       /* num = {0,1} */
#define RXBSIDL(num)    0x62|(num<<4)       /* num = {0,1} */
#define RXBEIDH(num)    0x63|(num<<4)       /* num = {0,1} */
#define RXBEIDL(num)    0x64|(num<<4)       /* num = {0,1} */
#define RXBDLC(num)     0x65|(num<<4)       /* num = {0,1} */
#define RXBDATA(num)    0x66|(num<<4)       /* num = {0,1} */

/* ===Receive Filter address=== */
#define RXFSIDH(num)    0x00|(num<<2)       /* num = {0,1,2,3,4,5} */
#define RXFSIDL(num)    0x01|(num<<2)       /* num = {0,1,2,3,4,5} */
#define RXFEIDH(num)    0x02|(num<<2)       /* num = {0,1,2,3,4,5} */
#define RXFEIDL(num)    0x03|(num<<2)       /* num = {0,1,2,3,4,5} */
#define RXF0SIDH        0x00
#define RXF1SIDH        0x04
#define RXF2SIDH        0x08
#define RXF3SIDH        0x10
#define RXF4SIDH        0x14
#define RXF5SIDH        0x18

/* ===Receive mask address=== */
#define RXMSIDH(num)    0x20|(num<<2)       /* num = {0,1} */
#define RXMSIDL(num)    0x21|(num<<2)       /* num = {0,1} */
#define RXMEIDH(num)    0x22|(num<<2)       /* num = {0,1} */
#define RXMEIDL(num)    0x23|(num<<2)       /* num = {0,1} */

/* ===RXBCTRL register bits=== */
enum RXBCTRL_enum{
    RXBCTRL_FILHIT_gp,
    RXBCTRL_BUKT1_bp,
    RXBCTRL_BUKT_bp,
    RXBCTRL_RXRTR_bp,
    RXBCTRL_RXM_gp = 5
};

enum RXBCTRL_RXM_enum{
    RXBCTRL_RXM_gm = 0x03<<RXBCTRL_RXM_gp,
    RXBCTRL_RXM_STD_EXT_ID_gc = 0<<RXBCTRL_RXM_gp,
    RXBCTRL_RXM_STDID_ONLY_gc = 1<<RXBCTRL_RXM_gp,
    RXBCTRL_RXM_EXTID_ONLY_gc = 2<<RXBCTRL_RXM_gp,
    RXBCTRL_RXM_IDFILTERMASK_OFF_gc = 3<<RXBCTRL_RXM_gp,
};

enum RXBCTRL_FILHIT_enum{
    RXBCTRL_FILHIT_ACCEPT_FILTER_gm = 0x07<<RXBCTRL_FILHIT_gp,
    RXBCTRL_FILHIT_ACCEPT_FILTER_0 = 0<<RXBCTRL_FILHIT_gp,
    RXBCTRL_FILHIT_ACCEPT_FILTER_1 = 1<<RXBCTRL_FILHIT_gp,
    RXBCTRL_FILHIT_ACCEPT_FILTER_2 = 2<<RXBCTRL_FILHIT_gp,
    RXBCTRL_FILHIT_ACCEPT_FILTER_3 = 3<<RXBCTRL_FILHIT_gp,
    RXBCTRL_FILHIT_ACCEPT_FILTER_4 = 4<<RXBCTRL_FILHIT_gp,
    RXBCTRL_FILHIT_ACCEPT_FILTER_5 = 5<<RXBCTRL_FILHIT_gp,
};


/*===== BIT TIMING =================================================
 *==========================================================================*/
#define CNF1        0x2A
#define CNF2        0x29
#define CNF3        0x28

/* ===CNF1 register bits=== */
enum CNF1_enum{
    CNF1_BRP_gp,
    CNF1_SWJ_gp = 6
};

enum CNF1_BRP_enum{
    CNF1_BRP_gm = 0x3F<<CNF1_BRP_gp,
};

enum CNF1_SWJ_enum{
    CNF1_SWJ_gm = 0x03<<CNF1_SWJ_gp,
    CNF1_SWJ_1TQ = 0<<CNF1_SWJ_gp,
    CNF1_SWJ_2TQ = 1<<CNF1_SWJ_gp,
    CNF1_SWJ_3TQ = 2<<CNF1_SWJ_gp,
    CNF1_SWJ_4TQ = 3<<CNF1_SWJ_gp,
};

/* ===CNF2 register bits=== */
enum CNF2_enum{
    CNF2_PRGSEG_gp,
    CNF2_PHSEG1_gp = 3,
    CNF2_SAM_bp = 6,
    CNF2_BTLMODE_bp = 7
};

enum CNF2_PRGSEG_enum{
    CNF2_PRGSEG_gm = 0x07<<CNF2_PRGSEG_gp,
};

enum CNF2_PHSEG1_enum{
    CNF2_PHSEG1_gm = 0x07<<CNF2_PHSEG1_gp,
};

/* ===CNF3 register bits=== */
enum CNF3_enum{
    CNF3_PHSEG2_gp,
    CNF3_WAKFIL_bp = 6,
    CNF3_SOF_bp = 7
};

enum CNF3_PHSEG2_enum{
    PHSEG2_gm = 0x07<<CNF3_PHSEG2_gp,
};

/* ===CAN BAUD SETTING 16TQ 16MHZ=== */
#define CNF1_BRP_100k_16TQ      4<<CNF1_BRP_gp
#define CNF1_BRP_125k_16TQ      3<<CNF1_BRP_gp
#define CNF1_BRP_250k_16TQ      1<<CNF1_BRP_gp
#define CNF1_BRP_500k_16TQ      0<<CNF1_BRP_gp
#define CNF2_PRGSEG_16TQ        6<<CNF2_PRGSEG_gp
#define CNF2_PHSEG1_16TQ        3<<CNF2_PHSEG1_gp
#define CNF3_PHSEG2_16TQ        3<<CNF3_PHSEG2_gp

/* ===CAN BAUD SETTING 8TQ 16MHZ=== */
#define CNF1_BRP_100k_8TQ       9<<CNF1_BRP_gp
#define CNF1_BRP_125k_8TQ       7<<CNF1_BRP_gp
#define CNF1_BRP_200k_8TQ       4<<CNF1_BRP_gp
#define CNF1_BRP_250k_8TQ       3<<CNF1_BRP_gp
#define CNF1_BRP_500k_8TQ       1<<CNF1_BRP_gp
#define CNF1_BRP_1M_8TQ         0<<CNF1_BRP_gp
#define CNF2_PRGSEG_8TQ         2<<CNF2_PRGSEG_gp
#define CNF2_PHSEG1_8TQ         1<<CNF2_PHSEG1_gp
#define CNF3_PHSEG2_8TQ         1<<CNF3_PHSEG2_gp


/*===== ERROR REGISTERS ====================================================
 *==========================================================================*/
#define TEC         0x1C
#define REC         0x1D
#define ELFG        0x2E

/* ===ELFG register bits=== */
enum ELFG_enum{
    ELFG_EWARN_bp,
    ELFG_RXWAR_bp,
    ELFG_TXWAR_bp,
    ELFG_RXEP_bp,
    ELFG_TXEP_bp,
    ELFG_TXBO_bp,
    ELFG_RX0OVR_bp,
    ELFG_RX1OVR_bp
};


/*===== INTERRUPT REGISTERS ================================================
 *==========================================================================*/
#define CANINTE     0x2B
#define CANINTF     0x2C

/* ===CANINTE register bits=== */
enum CANINTE_enum{
    CANINTE_RX0IE_bp,
    CANINTE_RX1IE_bp,
    CANINTE_TX0IE_bp,
    CANINTE_TX1IE_bp,
    CANINTE_TX2IE_bp,
    CANINTE_ERRIE_bp,
    CANINTE_WAKIE_bp,
    CANINTE_MERRIE_bp
};

/* ===CANINTF register bits=== */
enum CANINTF_enum{
    CANINTF_RX0IF_bp,
    CANINTF_RX1IF_bp,
    CANINTF_TX0IF_bp,
    CANINTF_TX1IF_bp,
    CANINTF_TX2IF_bp,
    CANINTF_ERRIF_bp,
    CANINTF_WAKIF_bp,
    CANINTF_MERRIF_bp
};


/*===== CONFIGURATION  =====================================================
 *==========================================================================*/
#define CANCTRL		0x0F
#define CANSTAT		0x0E

/* ===CANCTRL register bits=== */
enum CANCTRL_enum{
    CANCTRL_CLKPRE_gp,
    CANCTRL_CLKEN_bp = 2,
    CANCTRL_OSM_bp,
    CANCTRL_ABAT_bp,
    CANCTRL_REQOP_gp,
};

enum CANTRL_CLKPRE_enum{
    CANCTRL_CLKPRE_gm = 0x03<<CANCTRL_CLKPRE_gp,
    CANCTRL_CLKPRE_FCLKOUT_DIV1 = 0<<CANCTRL_CLKPRE_gp,
    CANCTRL_CLKPRE_FCLKOUT_DIV2 = 1<<CANCTRL_CLKPRE_gp,
    CANCTRL_CLKPRE_FCLKOUT_DIV4 = 2<<CANCTRL_CLKPRE_gp,
    CANCTRL_CLKPRE_FCLKOUT_DIV8 = 3<<CANCTRL_CLKPRE_gp,
};

enum CANCTRL_REQOP_enum{
    CANCTRL_REQOP_gm = 0x07<<CANCTRL_REQOP_gp,
    CANCTRL_REQOP_NORMAL_MODE = 0<<CANCTRL_REQOP_gp,
    CANCTRL_REQOP_SLEEP_MODE = 1<<CANCTRL_REQOP_gp,
    CANCTRL_REQOP_LOOPBACK_MODE = 2<<CANCTRL_REQOP_gp,
    CANCTRL_REQOP_LISTEN_MODE = 3<<CANCTRL_REQOP_gp,
    CANCTRL_REQOP_CONFIGURATION_MODE = 4<<CANCTRL_REQOP_gp,
};

/* ===CANSTAT register bits=== */
enum CANSTAT_enum{
    CANSTAT_ICOD_gp = 1,
    CANSTAT_OPMOD_gp = 5,
};

enum CANSTAT_ICOD_enum{
    CANSTAT_ICOD_gm = 0x07<<CANSTAT_ICOD_gp,
    CANSTAT_ICOD_NO_INTERRUPT = 0<<CANSTAT_ICOD_gp,
    CANSTAT_ICOD_ERROR_INTERRUPT = 1<<CANSTAT_ICOD_gp,
    CANSTAT_ICOD_WAKE_UP_INTERRUPT = 2<<CANSTAT_ICOD_gp,
    CANSTAT_ICOD_TXB0_INTERRUPT = 3<<CANSTAT_ICOD_gp,
    CANSTAT_ICOD_TXB1_INTERRUPT = 4<<CANSTAT_ICOD_gp,
    CANSTAT_ICOD_TXB2_INTERRUPT = 5<<CANSTAT_ICOD_gp,
    CANSTAT_ICOD_RXB0_INTERRUPT = 6<<CANSTAT_ICOD_gp,
    CANSTAT_ICOD_RXB1_INTERRUPT = 7<<CANSTAT_ICOD_gp,
};

enum CANSTAT_OPMOD_enum{
    CANSTAT_OPMOD_gm = 0x07<<CANSTAT_OPMOD_gp,
    CANSTAT_OPMOD_NORMAL_MODE = 0<<CANSTAT_OPMOD_gp,
    CANSTAT_OPMOD_SLEEP_MODE = 1<<CANSTAT_OPMOD_gp,
    CANSTAT_OPMOD_LOOPBACK_MODE = 2<<CANSTAT_OPMOD_gp,
    CANSTAT_OPMOD_LISTEN_MODE = 3<<CANSTAT_OPMOD_gp,
    CANSTAT_OPMOD_CONFIGURATION_MODE = 4<<CANSTAT_OPMOD_gp,
};

/*===== CAN MESSAGE BUFFER ENUM ============================================
 *==========================================================================*/
enum RXTXBUFFER_enum {
    TX0_BUFFER,
    TX1_BUFFER,
    TX2_BUFFER,
    RX0_BUFFER,
    RX1_BUFFER,
};

/*===== CAN MESSAGE STRUCTURE ==============================================
 *==========================================================================*/
struct CAN_MSG_t {
    uint16_t timeout;
    uint32_t id;
    uint8_t dlc;
    uint8_t data[8];
};

/*===== CLASS ==============================================================
 *==========================================================================*/
class MCP2515
{
    private:
    
    public:
        MCP2515();
        void Init();
        uint8_t ReadReg(uint8_t unRegAddress);
        void WriteReg(uint8_t unRegAddress, uint8_t unVal);
        bool SendCANmsg(CAN_MSG_t *psCanmsg);
        void ReadCANmsg(RXTXBUFFER_enum eRxBuf, CAN_MSG_t *psCanmsg);
        void BitModify(uint8_t unReg, uint8_t unMask, uint8_t unBits);
        void SetFilter(uint8_t unRxTxBuf, uint32_t unMask, uint8_t unRegRxfSidH, uint32_t unID);
};

#endif /* MCP2515_H_ */