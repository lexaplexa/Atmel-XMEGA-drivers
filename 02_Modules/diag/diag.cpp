/*************************************************************************
 * diag.cpp
 *
 * Created: 12.1.2015 11:20:23
 * Revised:	21.1.2016
 * Author: LeXa
 * BOARD: 
 *
 * ABOUT:
 * CAN diagnostic according ISO 15765-2
 *************************************************************************/

#include <avr/interrupt.h>
#include "diag.h"

MCP2515 cMcp;
uint16_t m_unSequence;
uint16_t m_unByteCounter;
uint8_t m_BlockSize;
DIAG_MSG_struct sDiagMsg;

DIAG_enum DiagSingleFrame(uint8_t *paData, uint8_t unDataLength)
{
	CAN_MSG_t sCanMsg = {0,DIAG_TESTER_ID,8,0,0,0,0,0,0,0,0};
		
	if (unDataLength > 7) {return DIAG_OUT_OF_RANGE;}	
	
	/************************************************************************/
	/* Byte 0 - Protocol Control Information (PCI) and						*/
	/* Byte 1-7 - data														*/
	/* pnData can be max. 7 bytes long										*/
	/************************************************************************/
	sCanMsg.data[0] = PCI_SINGLE_FRAME|(unDataLength & 0x07);
	
	for (uint8_t i=1; i<=7; i++)
	{
		if (i > unDataLength) {sCanMsg.data[i] = 0xFF;}		/* Rest of bytes filled with 0xFF */
		else {sCanMsg.data[i] = *(paData+i-1);}
	}
	
	cli();
	cMcp.SendCANmsg(&sCanMsg);
	sei();
	return DIAG_OK;
}

DIAG_enum DiagFirstFrame(uint8_t *paData, uint16_t unDataLength)
{
	CAN_MSG_t sCanMsg = {0,DIAG_TESTER_ID,8,0,0,0,0,0,0,0,0};
		
	if (unDataLength > 4095) {return DIAG_OUT_OF_RANGE;}
	m_unSequence = 0;		/* With First frame sequence counter has to be cleared */
	m_unByteCounter = 6;	/* With First frame byte counter has to be set to 6 */
		
	/************************************************************************/
	/* Byte 0-1 - Protocol Control Information (PCI)						*/
	/* Byte 2-7 - data														*/
	/* pnData can be max. 4095 bytes long									*/
	/************************************************************************/
	sCanMsg.data[0] = PCI_FIRST_FRAME|((unDataLength>>8) & 0x0F);
	sCanMsg.data[1] = (uint8_t)unDataLength;
	
	for (uint8_t i=2; i<=7; i++)
	{
		sCanMsg.data[i] = *(paData+i-2);
	}
	
	cli();
	cMcp.SendCANmsg(&sCanMsg);
	sei();
	return DIAG_OK;
}

DIAG_enum DiagConsecutiveFrame(uint8_t *paData, uint16_t unDataLength)
{
	CAN_MSG_t sCanMsg = {0,DIAG_TESTER_ID,8,0,0,0,0,0,0,0,0};
	
	if (unDataLength > 4095) {return DIAG_OUT_OF_RANGE;}
	m_unSequence++;			/* Increment sequence number */
		
	/************************************************************************/
	/* Byte 0 - Protocol Control Information (PCI)							*/
	/* Byte 1-7	- data														*/
	/* data_length can be max. 4095 bytes long                              */
	/* Sequence 1-15 - sequence number										*/
	/************************************************************************/
	sCanMsg.data[0] = PCI_CONSECUTIVE_FRAME|(uint8_t)(m_unSequence & 0x0F);
	
	for (uint8_t i=1; i<=7; i++)
	{
		if (m_unByteCounter < unDataLength) {sCanMsg.data[i] = *(paData + m_unByteCounter);}
		else {sCanMsg.data[i] = 0xAA;}	/* Rest of bytes filled with 0xAA */
		m_unByteCounter++;
	}
	
	cli();
	cMcp.SendCANmsg(&sCanMsg);
	sei();
	
	if (m_unByteCounter >= unDataLength) {return DIAG_LAST_CF;}	/* Last consecutive frame */
	if (m_unSequence == m_BlockSize) {return DIAG_WAIT_FOR_FLOW_CONTROL;}	/* Wait for next flow control */
	return DIAG_OK;
}

DIAG_enum DiagFlowControl(FLOW_CONTROL_enum eFlowStatus, uint8_t unBlockSize, uint8_t unSeparationTime)
{
	CAN_MSG_t sCanMsg = {0,DIAG_TESTER_ID,8,0,0,0,0,0,0,0,0};
	
	/************************************************************************/
	/* Byte 0 - Protocol Control Information (PCI)							*/
	/* Byte 1 - block size (BS) - how many CF will be send					*/
	/* Byte 2 - separation time												*/
	/************************************************************************/
	sCanMsg.data[0] = PCI_FLOW_CONTROL|(eFlowStatus & 0x0F);
	sCanMsg.data[1] = unBlockSize;
	sCanMsg.data[2] = unSeparationTime;
	sCanMsg.data[3] = 0xFF;
	sCanMsg.data[4] = 0xFF;
	sCanMsg.data[5] = 0xFF;
	sCanMsg.data[6] = 0xFF;
	sCanMsg.data[7] = 0xFF;
	
	cli();
	cMcp.SendCANmsg(&sCanMsg);
	sei();
	return DIAG_OK;
}

DIAG_enum DiagAnalyzeFrame(CAN_MSG_t *psCanMsg)
{
	if(psCanMsg->id != DIAG_CLUSTER_ID) {return DIAG_NOK;}
	switch (psCanMsg->data[0] & 0xF0)
	{
		/*===== RECEIVE SINGLE FRAME =========================================*/
		case PCI_SINGLE_FRAME:
			sDiagMsg.unLength = psCanMsg->data[0] & 0x0F;
			for (uint8_t i=0; i<sDiagMsg.unLength; i++)
			{
				sDiagMsg.aData[i] = psCanMsg->data[i+1];
			}
			return DIAG_SINGLE_FRAME;
			break;
			
			
		/*===== RECEIVE FIRST FRAME ==========================================*/	
		case PCI_FIRST_FRAME:
			sDiagMsg.unLength = (uint16_t)(psCanMsg->data[0] & 0x0F)<<8|(uint16_t)(psCanMsg->data[1]);
			m_unSequence = 0;		/* With First frame sequence counter has to be cleared */
			m_unByteCounter = 6;	/* First frame has always 6 bytes */
			
			for (uint8_t i=0; i<6; i++)
			{
				sDiagMsg.aData[i] = psCanMsg->data[i+2];
			}
			/* Answer to First Frame */
			DiagFlowControl(FLOW_CONTROL_CONTINUE_TO_SEND,FLOW_CONTROL_BLOCK_SIZE,FLOW_CONTROL_SEPARATION_TIME);
			return DIAG_FIRST_FRAME;
			break;
			
			
		/*===== RECEIVE CONSECUTIVE FRAME ====================================*/	
		case PCI_CONSECUTIVE_FRAME:				
			for (uint8_t i=1; i<=7; i++)
			{
				sDiagMsg.aData[i + m_unSequence*7 + m_unByteCounter - 1] = psCanMsg->data[i];
				if ((i + m_unSequence*7 + m_unByteCounter) == sDiagMsg.unLength) {return DIAG_LAST_CF;}				
			}
			/* Increment sequence number */
			m_unSequence++;	
			/* When block size reaches maximum */	
			if (m_unSequence == FLOW_CONTROL_BLOCK_SIZE)
			{
				m_unSequence = 0;
				m_unByteCounter += FLOW_CONTROL_BLOCK_SIZE*7;	/* Increment byte counter */
				DiagFlowControl(FLOW_CONTROL_CONTINUE_TO_SEND,FLOW_CONTROL_BLOCK_SIZE,FLOW_CONTROL_SEPARATION_TIME);				
			}
			return DIAG_CONSECUTIVE_FRAME;
			break;
			
			
		/*===== RECEIVE FLOW CONTROL FRAME ===================================*/	
		case PCI_FLOW_CONTROL:
			m_BlockSize = psCanMsg->data[1] & 0x0F;
			return DIAG_FLOW_CONTROL;
			break;
		
		
		default:
			return DIAG_NO_FRAME;				
	}
	return DIAG_OK;
}