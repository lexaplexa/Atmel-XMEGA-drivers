/*
 * eol.cpp
 *
 * Created: 17.7.2015 12:57:19
 * Revised: 31.3.2018
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 * EOL (End Of Line) communication
 *************************************************************************/

#include "eol.h"
#include <avr/interrupt.h>


EOL::EOL(MCP2515 *pcMcp)
{
    m_pcMcp = pcMcp;
}

EOL_enum EOL::Send(uint8_t *paData, uint16_t unDataLength)
{
    CAN_MSG_t sCanMsg = {0,EOL_TESTER_ID,8,0,0,0,0,0,0,0,0};
    uint8_t i;
    
    do 
    {
        for (i=0; i<8; i++)
        {
            sCanMsg.data[i] = *(paData+m_unByteCounter);
            if (m_unByteCounter == unDataLength) {sCanMsg.dlc = i; break;}
            m_unByteCounter++;
        }
        
        cli();
        /* If CAN message can't be send, then return false */
        if (!m_pcMcp->SendCANmsg(&sCanMsg)) {m_unByteCounter = 0; sei(); return EOL_NOK;}
        sei();
        
        if (m_unByteCounter == unDataLength) {m_unByteCounter = 0; return EOL_LAST_MSG_SEND;}
    } while (m_unByteCounter != unDataLength);
    
    return EOL_OK;
}

EOL_enum EOL::Read(CAN_MSG_t *psCanMsg)
{
    if (psCanMsg->id != EOL_CLUSTER_ID) {return EOL_NOK;}
    if (!m_unByteCounter && psCanMsg->dlc == 1 && psCanMsg->data[0] == 0x84) {return EOL_STIMULATION_ANSWER;}
    
    for (uint8_t i=0; i<psCanMsg->dlc; i++)
    {
        m_aData[m_unByteCounter++] = psCanMsg->data[i];
        if (m_unByteCounter >= *(uint16_t*)&m_aData + 2) {m_unByteCounter = 0; return EOL_LAST_MSG_READ;}
    }
    return EOL_OK;
}