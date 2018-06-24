/*************************************************************************
 * mfrc522.h
 *
 * Created: 13.1.2016 15:22:07
 * Revised:	24.6.2018
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 * Driver for RFID module MFRC522
 * -----------------------------------------------------------------------
 * Pins in port (see conf_board.h):
 * Signal		Pin				Pin
 *				Xplained A1		MFRC522 board
 * ------------------------------------------
 * Reset		0				RST
 * IRQ			1				IRQ
 * SPI SS		5				SDA
 * SPI MOSI		6				MOSI
 * SPI MISO		7				MISO
 * SPI SCK		8				SCK
 *************************************************************************/


#ifndef MFRC522_H_
#define MFRC522_H_

#include <avr/io.h>
#include <core/drivers/spi.h>
#include <conf/conf_board.h>

/************************************************************************/
/* READ AND WRITE SIGNATURES                                            */
/************************************************************************/
#define READ_REG_SIG			0x80
#define WRITE_REG_SIG			0x00

/************************************************************************/
/* REGISTER ADDRESSES                                                   */
/************************************************************************/
enum REG_enum {
	/* Page 0: Command and status */
	REG_Reserved00			= 0x00,		/* reserved																			*/
	REG_CommandReg 			= 0x01,		/* starts and stops command execution												*/
	REG_ComlEnReg 			= 0x02,		/* enable and disable interrupt request control bits								*/
	REG_DivlEnReg 			= 0x03,		/* enable and disable interrupt request control bits								*/
	REG_ComIrqReg 			= 0x04,		/* interrupt request bits															*/
	REG_DivIrqReg 			= 0x05,		/* interrupt request bits															*/
	REG_ErrorReg 			= 0x06,		/* error bits showing the error status of the last command executed					*/
	REG_Status1Reg 			= 0x07,		/* communication status bits														*/
	REG_Status2Reg 			= 0x08,		/* receiver and transmitter status bits												*/
	REG_FIFODataReg 		= 0x09,		/* input and output of 64 byte FIFO buffer											*/
	REG_FIFOLevelReg 		= 0x0A,		/* number of bytes stored in the FIFO buffer										*/
	REG_WaterLevelReg 		= 0x0B,		/* level for FIFO underflow and overflow warning									*/
	REG_ControlReg 			= 0x0C,		/* miscellaneous control registers													*/
	REG_BitFramingReg 		= 0x0D,		/* adjustments for bit-oriented frames												*/
	REG_CollReg 			= 0x0E,		/* bit position of the first bit-collision detected on the RF interface				*/
	REG_Reserved0F			= 0x0F,		/* reserved																			*/
	/* Page 1: Command */
	REG_Reserved10			= 0x10,		/* reserved																			*/
	REG_ModeReg 			= 0x11,		/* defines general modes for transmitting and receiving								*/
	REG_TxModeReg 			= 0x12,		/* defines transmission data rate and framing										*/
	REG_RxModeReg 			= 0x13,		/* defines reception data rate and framing											*/
	REG_TxControlReg 		= 0x14,		/* controls the logical behavior of the antenna driver pins TX1 and TX2				*/
	REG_TxASKReg 			= 0x15,		/* controls the setting of the transmission modulation								*/
	REG_TxSelReg 			= 0x16,		/* selects the internal sources for the antenna driver								*/
	REG_RxSelReg 			= 0x17,		/* selects internal receiver settings												*/
	REG_RxThresholdReg 		= 0x18,		/* selects thresholds for the bit decoder											*/
	REG_DemodReg 			= 0x19,		/* defines demodulator settings														*/
	REG_Reserved1A			= 0x1A,		/* reserved																			*/
	REG_Reserved1B			= 0x1B,		/* reserved																			*/
	REG_MfTxReg 			= 0x1C,		/* controls some MIFARE communication transmit parameters							*/
	REG_MfRxReg 			= 0x1D,		/* controls some MIFARE communication receive parameters							*/
	REG_Reserved1E			= 0x1E,		/* reserved																			*/
	REG_SerialSpeedReg 		= 0x1F,		/* selects the speed of the serial UART interface									*/
	/* Page 2: Configuration */
	REG_Reserved20			= 0x20,		/* reserved																			*/
	REG_CRCResultReg 		= 0x21,		/* shows the MSB and LSB values of the CRC calculation								*/
	REG_Reserved23			= 0x23,		/* reserved																			*/
	REG_ModWidthReg 		= 0x24,		/* controls the ModWidth setting Table 93 on page 58								*/
	REG_Reserved25			= 0x25,		/* reserved																			*/
	REG_RFCfgReg 			= 0x26,		/* configures the receiver gain Table 97 on page 59									*/
	REG_GsNReg 				= 0x27,		/* selects the conductance of the antenna driver pins TX1 and TX2 for modulation	*/
	REG_CWGsPReg 			= 0x28,		/* defines the conductance of the p-driver output during periods of no modulation	*/
	REG_ModGsPReg 			= 0x29,		/* defines the conductance of the p-driver output during periods of modulation		*/
	REG_TModeReg 			= 0x2A,		/* defines settings for the internal timer											*/
	REG_TPrescalerReg 		= 0x2B,		/* prescaler																		*/
	REG_TReloadRegH			= 0x2C,		/* defines the 16-bit timer reload value											*/
	REG_TReloadRegL			= 0x2D,		/* defines the 16-bit timer reload value											*/
	REG_TCounterValRegH		= 0x2E,		/* shows the 16-bit timer value														*/
	REG_TCounterValRegL		= 0x2F,		/* shows the 16-bit timer value														*/
	/* Page 3: Test register */
	REG_Reserved30			= 0x30,		/* reserved																			*/
	REG_TestSel1Reg 		= 0x31,		/* general test signal configuration												*/
	REG_TestSel2Reg 		= 0x32,		/* general test signal configuration and PRBS control								*/
	REG_TestPinEnReg 		= 0x33,		/* enables pin output driver on pins D1 to D7										*/
	REG_TestPinValueReg		= 0x34,		/* defines the values for D1 to D7 when it is used as an I/O bus					*/
	REG_TestBusReg 			= 0x35,		/* shows the status of the internal test bus										*/
	REG_AutoTestReg 		= 0x36,		/* controls the digital self test													*/
	REG_VersionReg 			= 0x37,		/* shows the software version														*/
	REG_AnalogTestReg 		= 0x38,		/* controls the pins AUX1 and AUX2													*/
	REG_TestDAC1Reg 		= 0x39,		/* defines the test value for TestDAC1												*/
	REG_TestDAC2Reg 		= 0x3A,		/* defines the test value for TestDAC2												*/
	REG_TestADCReg 			= 0x3B,		/* shows the value of ADC I and Q channels											*/
	REG_Reserved3C 			= 0x3C,		/* reserved for production tests													*/
	REG_Reserved3D 			= 0x3D,		/* reserved for production tests													*/
	REG_Reserved3E 			= 0x3E,		/* reserved for production tests													*/
	REG_Reserved3F			= 0x3F,		/* reserved for production tests													*/
};

/************************************************************************/
/* COMMANDS                                                             */
/************************************************************************/
enum COMM_enum {
	COMM_Idle 				= 0b0000,	/* no action, cancels current command execution																							*/
	COMM_Mem 				= 0b0001,	/* stores 25 bytes into the internal buffer																								*/
	COMM_GenerateRandomID 	= 0b0010,	/* generates a 10-byte random ID number																									*/
	COMM_CalcCRC 			= 0b0011,	/* activates the CRC coprocessor or performs a self test																				*/
	COMM_Transmit 			= 0b0100,	/* transmits data from the FIFO buffer																									*/
	COMM_NoCmdChange 		= 0b0111,	/* no command change, can be used to modify the CommandReg register bits without affecting the command,for example, the PowerDown bit	*/
	COMM_Receive 			= 0b1000,	/* activates the receiver circuits																										*/
	COMM_Transceive 		= 0b1100,	/* transmits data from FIFO buffer to antenna and automatically activates the receiver after transmission								*/
	COMM_Reserved 			= 0b1101,	/* reserved for future use																												*/
	COMM_MFAuthent 			= 0b1110,	/* performs the MIFARE standard authentication as a reader																				*/
	COMM_SoftReset 			= 0b1111,	/* resets the MFRC522																													*/
};

/************************************************************************/
/* RETURN VALUES                                                        */
/************************************************************************/
enum STATUS_enum {
	STATUS_OK,
	STATUS_NOK,
	STATUS_TIMEOUT,
	STATUS_ERROR,
	STATUS_COLLISION,
};

/************************************************************************/
/* CLASS                                                                */
/************************************************************************/
class MFRC522 {

	public:	
		
		/************************************************************************/
		/* FUNCTION DECLARATION                                                 */
		/************************************************************************/

		/**
		 * \brief					MFRC522 constructor. Initialize PORT, SPI and MFRC522 registers
		 * 
		 * 
		 * \return 
		 */
		MFRC522();


		/**
		 * \brief					Read register value from address
		 * 
		 * \param eReg				Register address
		 * 
		 * \return uint8_t			Register value
		 */
		inline uint8_t ReadReg(REG_enum eReg);


		/**
		 * \brief					Read register values from starting address
		 * 
		 * \param eReg				Register start address
		 * \param unNum				Number of bytes
		 * \param punBuffer			Pointer to buffer
		 * 
		 * \return void
		 */
		inline void ReadReg(REG_enum eReg, uint8_t unNum, uint8_t *punBuffer);
		
		
		/**
		 * \brief					Write data to register address
		 * 
		 * \param eReg				Register address
		 * \param unData			Data to write
		 * 
		 * \return void
		 */
		inline void WriteReg(REG_enum eReg, uint8_t punData);
		
		
		/**
		 * \brief					Write data to registers from starting address
		 * 
		 * \param eReg				Register start address
		 * \param punData			Pointer to data buffer
		 * \param unLen				Number of data
		 * 
		 * \return void
		 */
		inline void WriteReg(REG_enum eReg, uint8_t *punData, uint8_t unLen);
		
		
		/**
		 * \brief					Set register value according bit mask
		 * 
		 * \param eReg				Register address
		 * \param unBitMask			Bit mask
		 * 
		 * \return void
		 */
		inline void SetRegBitMask(REG_enum eReg, uint8_t unBitMask);
		
		
		/**
		 * \brief					Clear register value according bit mask
		 * 
		 * \param eReg				Register address
		 * \param unBitMask			Bit mask
		 * 
		 * \return void
		 */
		inline void ClrRegBitMask(REG_enum eReg, uint8_t unBitMask);


		/**
		 * \brief					Set Antenna on
		 * 
		 * 
		 * \return void
		 */
		void AntennaOn();


		/**
		 * \brief					Set Antenna off
		 * 
		 * 
		 * \return void
		 */
		void AntennaOff();
		

		/**
		 * \brief					Send command to MFRC522
		 * 
		 * \param eComm				Command
		 * \param punDatatoSend		Pointer to transmit data buffer
		 * \param unLen				Number of bytes in transmit buffer
		 * \param punReturnData		Pointer to buffer for received data
		 * \param punReturnLen		Number of received bytes
		 * \param punValidBits		Valid bits in last received byte
		 * 
		 * \return STATUS_enum		Status
		 */
		STATUS_enum SendCommand(COMM_enum eComm, uint8_t *punDatatoSend, uint8_t unLen, uint8_t *punReturnData, uint8_t *punReturnLen, uint8_t *punValidBits);
	
	
		/**
		 * \brief					Calculates CRC checksum
		 * 
		 * \param punDatatoSend		Pointer to data buffer
		 * \param unLen				Number of bytes in 
		 * \param punReturnDat
		 * 
		 * \return STATUS_enum		Status
		 */
		STATUS_enum CalculateCRC(uint8_t *punDatatoSend, uint8_t unLen, uint8_t *punReturnDat);

		
		/**
		 * \brief					Transmit data to PICC (Proximity card) and returns back answer from PICC
		 * 
		 * \param punDatatoSend		Pointer to transmit data buffer
		 * \param unLen				Number of bytes in transmit buffer
		 * \param punReturnData		Pointer to buffer for received data
		 * \param unReturnLen		Number of received bytes
		 * \param punValidBits		Valid bits in last received byte
		 * 							
		 * \return STATUS_enum		Status
		 */
		STATUS_enum TransceiveData(uint8_t *punDatatoSend, uint8_t unLen, uint8_t *punReturnData, uint8_t *unReturnLen, uint8_t *punValidBits)
		{
			return SendCommand(COMM_Transceive,punDatatoSend,unLen,punReturnData,unReturnLen,punValidBits);
		}


		/**
		 * \brief					Request A
		 * 
		 * \param punReturnData		Pointer to Answer from PICC
		 * \param punReturnLen		Number of received bytes
		 * 
		 * \return STATUS_enum		Status
		 */
		STATUS_enum REQA(uint8_t *punReturnData, uint8_t *punReturnLen)
		{
			uint8_t unValidBits = 7;
			uint8_t unComm = 0x26;		
			return TransceiveData(&unComm,1,punReturnData,punReturnLen,&unValidBits);
		}


		/**
		 * \brief					Wake up A
		 * 
		 * \param punReturnData		Pointer to Answer from PICC
		 * \param punReturnLen		Number of received bytes
		 * 
		 * \return STATUS_enum		Status
		 */
		STATUS_enum WUPA(uint8_t *punReturnData, uint8_t *punReturnLen)
		{
			uint8_t unValidBits = 7;
			uint8_t unComm = 0x52;		
			return TransceiveData(&unComm,1,punReturnData,punReturnLen,&unValidBits);
		}

};

#endif /* MFRC522_H_ */