/*************************************************************************
 * sd.cpp
 *
 * Created: 27.5.2015 21:00:56
 * Revised: 24.6.2015
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 * 
 *************************************************************************/

#include "sd.h"

SD::SD()
{
	uint8_t retry = 0;
	
	/* Setup pins direction */
	spi_set_master_port_dir(SD_PORT);

#ifdef COMPILE_SD_INSERT_DETECT	
	/* Setup SD switch pin */
	SD_PORT.DIRCLR = 1<<SD_SWITCH_PIN;
	SD_PORT.INT0MASK = 1<<SD_SWITCH_PIN;
	SD_PORT.INTCTRL = PORT_INT0LVL_LO_gc;
	SD_PORT.PIN1CTRL = PORT_ISC_BOTHEDGES_gc|PORT_OPC_PULLUP_gc;
#endif
	
	spi_cs_disable(SD_PORT);
	
	spi_set_mode(SD_SPI,SPI_MODE_0_gc);
	spi_set_dord(SD_SPI,SPI_DORD_MSB_FIRST);
	
	spi_enable_master(SD_SPI);
	
	while ((retry++<10) && !m_bCardInitialized) {Init();}
}

bool SD::Init()
{
		uint8_t retry;
		R1_ERROR_enum eR1resp;
		uint8_t unResp;
		
		m_bCardInitialized = false;
		m_eCardType = SD_CARD_TYPE_NONE;
		
		/* Set low clock 400kHz */
		/* Clear prescaler */
		SD_SPI.CTRL &= ~(SPI_PRESCALER_gm);
		SD_SPI.CTRL &= ~(1<<SPI_CLK2X_bp);
		/* Baudrate */
#if (400000 <= F_CPU/128)
		SD_SPI.CTRL |= SPI_PRESCALER_DIV128_gc;
#elif (400000 <= F_CPU/64)
		SD_SPI.CTRL |= SPI_PRESCALER_DIV64_gc;
#elif (400000 <= F_CPU/32)
		SD_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV64_gc;
#elif (400000 <= F_CPU/16)
		SD_SPI.CTRL |= SPI_PRESCALER_DIV16_gc;
#elif (400000 <= F_CPU/8)
		SD_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV16_gc;
#elif (400000 <= F_CPU/4)
		SD_SPI.CTRL |= SPI_PRESCALER_DIV4_gc;
#elif (400000 <= F_CPU/2)
		SD_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV4_gc;
#else
		SD_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV4_gc;
#endif
				
		/* 80 clock cycles */
		spi_cs_enable(SD_PORT);
		for (uint8_t i=0;i<10;i++) {spi_send(SD_SPI,0xFF);}
		spi_cs_disable(SD_PORT);
		
		/* Reset and go idle */
		if (!(SendCommand(SD_CMD_GO_IDLE_STATE,0,0x95) & R1_IDLE_STATE_bm)) {return m_bCardInitialized;}
		
		if (!(SendCommand(SD_CMD_SEND_IF_COND,0x1AA,0x87) & R1_IDLE_STATE_bm)) {return m_bCardInitialized;}
		
		SendCommand(SD_CMD_APP_CMD,0,0xFF);
		
		/* Wait for initialization (0x00 = OK, else error)*/
		do
		{
			eR1resp = SendCommand(SD_CMD_SEND_OP_COND,0x40000000,0xFF);		/* ACMD41 */
			if (retry++ > 0xFE) {return m_bCardInitialized;}
		} while (eR1resp != R1_NO_ERROR);
		
		/* Set CRC off */
		if (SendCommand(SD_CMD_CRC_ON_OFF,0,0xFF) != R1_NO_ERROR) {return m_bCardInitialized;}
		/* Set Block length */
		if (SendCommand(SD_CMD_SET_BLOCK_LEN,SD_BLOCK_SIZE, 0xFF) != R1_NO_ERROR) {return m_bCardInitialized;}
		
		/* Read OCR register - check type of card */
		spi_cs_enable(SD_PORT);
		spi_send(SD_SPI,0x40|SD_CMD_READ_OCR);
		spi_send(SD_SPI,0x00);
		spi_send(SD_SPI,0x00);
		spi_send(SD_SPI,0x00);
		spi_send(SD_SPI,0x00);
		spi_send(SD_SPI,0xFF);
		
		/* Wait for response */
		do
		{
			spi_send(SD_SPI,0xFF);
			unResp = spi_read(SD_SPI);
			if (retry++ > 0xFE) {break;}
		} while(unResp == 0xFF);
		
		//while(spi_send(SD_SPI,0xFF) == 0xFF)	{if (retry++ > 0xFE) {break;}}
			
		/* OCR - 4 bytes (MSB first) */	
		spi_send(SD_SPI,0xFF);
		unResp = spi_read(SD_SPI);
		if (unResp & 0x40) {m_eCardType = SD_CARD_TYPE_SDHC_SDXC;}
		else {m_eCardType = SD_CARD_TYPE_SDSC;}
		spi_send(SD_SPI,0xFF);
		unResp = spi_read(SD_SPI);
		spi_send(SD_SPI,0xFF);
		unResp = spi_read(SD_SPI);
		spi_send(SD_SPI,0xFF);
		unResp = spi_read(SD_SPI);
		spi_cs_disable(SD_PORT);
		
		/* Set back to full baud rate */
		/* Clear prescaler */
		SD_SPI.CTRL &= ~(SPI_PRESCALER_gm);
		SD_SPI.CTRL &= ~(1<<SPI_CLK2X_bp);
		/* Baud rate */
#if (SD_BAUD <= F_CPU/128)
		SD_SPI.CTRL |= SPI_PRESCALER_DIV128_gc;
#elif (SD_BAUD <= F_CPU/64)
		SD_SPI.CTRL |= SPI_PRESCALER_DIV64_gc;
#elif (SD_BAUD <= F_CPU/32)
		SD_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV64_gc;
#elif (SD_BAUD <= F_CPU/16)
		SD_SPI.CTRL |= SPI_PRESCALER_DIV16_gc;
#elif (SD_BAUD <= F_CPU/8)
		SD_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV16_gc;
#elif (SD_BAUD <= F_CPU/4)
		SD_SPI.CTRL |= SPI_PRESCALER_DIV4_gc;
#elif (SD_BAUD <= F_CPU/2)
		SD_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV4_gc;
#else
		SD_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV4_gc;
#endif
		
		return (m_bCardInitialized = true);
}

R1_ERROR_enum SD::SendCommand(SD_CMD_enum eCmd, uint32_t unAttrib, uint8_t unCrc)
{
	R1_ERROR_enum eResp = R1_INVALID_ERROR_bm;
	uint8_t retry;
	
	spi_cs_enable(SD_PORT);
	spi_send(SD_SPI,0x40|eCmd);
	spi_send(SD_SPI,(uint8_t)(unAttrib>>24));
	spi_send(SD_SPI,(uint8_t)(unAttrib>>16));
	spi_send(SD_SPI,(uint8_t)(unAttrib>>8));
	spi_send(SD_SPI,(uint8_t)(unAttrib));
	spi_send(SD_SPI,unCrc);
	
	/* Wait for response */
	while(eResp == R1_INVALID_ERROR_bm)
	{
		spi_send(SD_SPI,0xFF);
		eResp = (R1_ERROR_enum)spi_read(SD_SPI);
		if (retry++ > 0xFE) {break;}
	}
	spi_cs_disable(SD_PORT);
	spi_send(SD_SPI,0xFF);		/* DUMMY */
	return eResp;	
}

bool SD::ReadBlock(uint32_t unBlockNum, uint8_t *punBuffer)
{
	uint8_t unResp;
	uint16_t retry;
	
	if (!m_bCardInitialized) {return false;}
	if (m_eCardType = SD_CARD_TYPE_SDSC) {unBlockNum <<= 9;}		/* SDSC use byte address, SDHC and SDXC use block address */
	
	if (SendCommand(SD_CMD_READ_SINGLE_BLOCK, unBlockNum, 0xFF) != R1_NO_ERROR) {return false;}	/* 0x00 = OK, else error */
	
	spi_cs_enable(SD_PORT);
	/* Wait for data token 0xFE */
	do 
	{
		spi_send(SD_SPI,0xFF);
		unResp = spi_read(SD_SPI);
		if (retry++ > 0xFFFE) {spi_cs_disable(SD_PORT); return false;}
	} while (unResp != 0xFE);
	//while (spi_send(SD_SPI,0xFF) != 0xFE) {if (retry++ > 0xFFFE) {spi_cs_disable(SD_PORT); return false;};}
	
	/* Read data from block */
	for (uint16_t i=0; i<SD_BLOCK_SIZE; i++)
	{
		spi_send(SD_SPI,0xFF);
		punBuffer[i] = spi_read(SD_SPI);
	}
	
	/* Last 2 bytes CRC */
	spi_send(SD_SPI,0xFF);
	spi_send(SD_SPI,0xFF);	
	spi_cs_disable(SD_PORT);
	
	return true;
}

bool SD::WriteBlock(uint32_t unBlockNum, uint8_t *punBuffer)
{
	uint8_t unResp;
	uint16_t retry;
	
	if (!m_bCardInitialized) {return false;}
	if (m_eCardType = SD_CARD_TYPE_SDSC) {unBlockNum <<= 9;}		/* SDSC use byte address, SDHC and SDXC use block address */
	
	if (SendCommand(SD_CMD_WRITE_SINGLE_BLOCK, unBlockNum, 0xFF) != R1_NO_ERROR) {return false;}	/* 0x00 = OK, else error */
	
	spi_cs_enable(SD_PORT);
	/* dummy */
	spi_send(SD_SPI,0xFF);
	/* Send data token */
	spi_send(SD_SPI,0xFE);
	
	/* Send data to sd card from buffer */
	for (uint16_t i=0; i<SD_BLOCK_SIZE; i++) {spi_send(SD_SPI,punBuffer[i]);}
	
	/* Last 2 bytes dummy CRC */
	spi_send(SD_SPI,0xFF);
	spi_send(SD_SPI,0xFF);
	
	/* Check if data accepted from sd card */
	spi_send(SD_SPI,0xFF);
	unResp = spi_read(SD_SPI);
	if ((unResp & DATA_ANSWER_bm) != DATA_ACCEPT_gc) {spi_cs_disable(SD_PORT); return false;}
	
	spi_send(SD_SPI,0xFF);
	/* Wait until data written */
	do
	{
		spi_send(SD_SPI,0xFF);
		unResp = spi_read(SD_SPI);
		if (retry++ > 0xFFFE) {spi_cs_disable(SD_PORT); return false;}
	} while (!unResp);
	//while (!spi_send(SD_SPI,0xFF)) {if (retry++ > 0xFFFE) {spi_cs_disable(SD_PORT); return false;};}	
	spi_cs_disable(SD_PORT);
	
	return true;
}

bool SD::EraseBlocks(uint32_t unStartBlock, uint32_t unTotalBlocks)
{
	if (!m_bCardInitialized) {return false;}
	if (m_eCardType = SD_CARD_TYPE_SDSC) {unStartBlock <<= 9;}		/* SDSC use byte address, SDHC and SDXC use block address */
	
	if (SendCommand(SD_CMD_ERASE_BLOCK_START_ADDR,unStartBlock, 0xFF) != R1_NO_ERROR) {return false;}					/* 0x00 = OK, else error */
	if (SendCommand(SD_CMD_ERASE_BLOCK_END_ADDR,(unStartBlock+unTotalBlocks-1), 0xFF) != R1_NO_ERROR) {return false;}	/* 0x00 = OK, else error */
	if (SendCommand(SD_CMD_ERASE_SELECTED_BLOCKS,0, 0xFF) != R1_NO_ERROR) {return false;}								/* 0x00 = OK, else error */
	
	return true;
}