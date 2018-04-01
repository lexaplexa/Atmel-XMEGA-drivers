/*
 * eol.h
 *
 * Created: 17.7.2015 12:57:02
 * Revised:	31.3.2018
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 * EOL (End Of Line) communication
 *************************************************************************/


#ifndef EOL_H_
#define EOL_H_

#include <conf.h>
#include <extensions/mcp2515/mcp2515.h>

/************************************************************************/
/* EOL CONFUGURATION                                                    */
/************************************************************************/
#ifndef EOL_TESTER_ID
    #warning Default configuration EOL_TESTER_ID
    #define EOL_TESTER_ID           0x1BFCA2F2
#endif
#ifndef EOL_CLUSTER_ID
    #warning Default configuration EOL_CLUSTER_ID
    #define EOL_CLUSTER_ID          0x1BFCA2F3
#endif
#ifndef EOL_MSG_DATA_LENGTH
    #warning Default configuration EOL_MSG_DATA_LENGTH
    #define EOL_MSG_DATA_LENGTH     100
#endif

/************************************************************************/
/* EOL RETURN ERROR CODES                                               */
/************************************************************************/
enum EOL_enum {
    EOL_OK,
    EOL_NOK,
    EOL_LAST_MSG_SEND,
    EOL_LAST_MSG_READ,
    EOL_STIMULATION_ANSWER = 0x84,
    };

/************************************************************************/
/* CLASS                                                                */
/************************************************************************/
class EOL
{
    private:
        MCP2515 *m_pcMcp;
        uint16_t m_unByteCounter;
    
    public:
        uint8_t m_aData[EOL_MSG_DATA_LENGTH];
        EOL(MCP2515 *pcMcp);
        EOL_enum Send(uint8_t *paData, uint16_t unDataLength);
        EOL_enum Read(CAN_MSG_t *psCanMsg);
};

#endif /* EOL_H_ */