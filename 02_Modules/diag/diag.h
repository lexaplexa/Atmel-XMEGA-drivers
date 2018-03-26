/*************************************************************************
 * diag.h
 *
 * Created: 12.1.2015 10:55:21
 * Revised: 21.1.2016
 * Author: LeXa
 * BOARD: 
 *
 * ABOUT:
 * CAN diagnostic according ISO 15765-2
 *************************************************************************/

#ifndef DIAG_H_
#define DIAG_H_

#include <avr/io.h>
#include <conf/conf_modules.h>
#include <extensions/mcp2515/mcp2515.h>

/************************************************************************/
/* DIAGNOSTIC CONFIGURATION                                             */
/************************************************************************/
#ifndef DIAG_TESTER_ID
	#warning Default configuration DIAG_TESTER_ID
	#define DIAG_TESTER_ID					0x714	
#endif
#ifndef DIAG_CLUSTER_ID	
	#warning Default configuration DIAG_CLUSTER_ID	
	#define DIAG_CLUSTER_ID					0x77E
#endif
#ifndef FLOW_CONTROL_BLOCK_SIZE
	#warning Default configuration FLOW_CONTROL_BLOCK_SIZE
	#define FLOW_CONTROL_BLOCK_SIZE			15
#endif
#ifndef FLOW_CONTROL_SEPARATION_TIME
	#warning Default configuration FLOW_CONTROL_SEPARATION_TIME
	#define FLOW_CONTROL_SEPARATION_TIME	2
#endif

/************************************************************************/
/* PROTOCOL CONTROL INFORMATION                                         */
/************************************************************************/
#define PCI_SINGLE_FRAME		0x00
#define PCI_FIRST_FRAME			0x10
#define PCI_CONSECUTIVE_FRAME	0x20
#define PCI_FLOW_CONTROL		0x30

/************************************************************************/
/* DIAG RETURN ERROR CODES                                              */
/************************************************************************/
enum DIAG_enum{
	DIAG_OK,
	DIAG_NOK,
	DIAG_OUT_OF_RANGE,
	DIAG_NO_FRAME,
	DIAG_LAST_CF,
	DIAG_SINGLE_FRAME,
	DIAG_FIRST_FRAME,
	DIAG_CONSECUTIVE_FRAME,
	DIAG_FLOW_CONTROL,
	DIAG_WAIT_FOR_FLOW_CONTROL,
};

/************************************************************************/
/* FLOW CONTROL															*/
/************************************************************************/
enum FLOW_CONTROL_enum{
	FLOW_CONTROL_CONTINUE_TO_SEND,
	FLOW_CONTROL_WAIT,
	FLOW_CONTROL_OVERFLOW,
	};	
	
/************************************************************************/
/* DIAGNOSTIC MESSAGE STRUCTURE                                         */
/************************************************************************/
struct DIAG_MSG_struct {
	uint8_t aData[DIAG_MSG_DATA_LENGTH];
	uint16_t unLength;
	};

/************************************************************************/
/* FUNCTIONS DECLARATION                                                */
/************************************************************************/

DIAG_enum DiagSingleFrame(uint8_t *paData, uint8_t unDataLength);
DIAG_enum DiagFirstFrame(uint8_t *paData, uint16_t unDataLength);
DIAG_enum DiagConsecutiveFrame(uint8_t *paData, uint16_t unDataLength);
DIAG_enum DiagFlowControl(FLOW_CONTROL_enum eFlowStatus, uint8_t unBlockSize, uint8_t unSeparationTime);
DIAG_enum DiagAnalyzeFrame(CAN_MSG_t *psCanMsg);


#endif /* DIAG_H_ */