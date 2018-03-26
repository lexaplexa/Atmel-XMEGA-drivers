/*************************************************************************
 * crc.cpp
 *
 * Created: 16.7.2015 11:35:10
 * Revised: 15.10.2015
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 * Cyclic redundancy check
 *************************************************************************/

#include <avr/io.h>

#define bitmask(value,mask)			(value & mask)

/*===== CRC 8 bit ==========================================================*/
uint8_t crc8(uint8_t unByte, uint8_t unGenPol)
{
	uint8_t unRem = (uint16_t)unByte;		/* Remainder */
	
	for (uint8_t i=1; i<=8; i++)
	{
		if (bitmask(unRem,0x80)) {unRem = (unRem<<1) ^ unGenPol;}
		else {unRem <<= 1;}
	}
	return unRem;
}

uint8_t crc8(uint8_t *aByteArray, uint16_t unLen, uint8_t unGenPol)
{
	uint16_t unRem = 0x0000;				/* Remainder */
	
	for (uint16_t j=0; j<unLen; j++)
	{
		unRem ^= *aByteArray++;
		for (uint8_t i=1; i<=8; i++)
		{
			if (bitmask(unRem,0x80)) {unRem = (unRem<<1) ^ unGenPol;}
			else {unRem <<= 1;}
		}
	}
	return (uint8_t)unRem;
}


uint8_t SPDUChechsum(uint8_t *aByteArray, uint16_t unLen, const uint8_t unSafetyCode)
{
	/* Generator polynomial constant = 0x2F */
	uint16_t unRem = 0xFFFF;				/* Remainder */
	uint8_t unByteCounter;
	
	/* Increment byte counter */
	unByteCounter = *aByteArray & 0x0F;
	*aByteArray = (*aByteArray & 0xF0)|(++unByteCounter & 0x0F);
	
	/* Calculate CRC */
	for (uint16_t j=0; j<unLen; j++)
	{
		unRem ^= *aByteArray++;
		for (uint8_t i=1; i<=8; i++)
		{
			if (bitmask(unRem,0x80)) {unRem = (unRem<<1) ^ 0x2F;}
			else {unRem <<= 1;}
		}
	}
	
	/* Add Safety Code */
	unRem ^= unSafetyCode;
	for (uint8_t i=1; i<=8; i++)
	{
		if (bitmask(unRem,0x80)) {unRem = (unRem<<1) ^ 0x2F;}
		else {unRem <<= 1;}
	}
	
	unRem = ~unRem;
	return unRem;
}

uint8_t SPDUChechsum(uint8_t *aByteArray, uint16_t unLen, const uint8_t *aSafetyCodes)
{
	/* Generator polynomial constant = 0x2F */
	uint16_t unRem = 0xFFFF;				/* Remainder */
	uint8_t unByteCounter;
	
	/* Increment byte counter */
	unByteCounter = *aByteArray & 0x0F;
	*aByteArray = (*aByteArray & 0xF0)|(++unByteCounter & 0x0F);
	
	/* Calculate CRC */
	for (uint16_t j=0; j<unLen; j++)
	{
		unRem ^= *aByteArray++;
		for (uint8_t i=1; i<=8; i++)
		{
			if (bitmask(unRem,0x80)) {unRem = (unRem<<1) ^ 0x2F;}
			else {unRem <<= 1;}
		}
	}
	
	/* Add Safety Code */
	unRem ^= *(aSafetyCodes + unByteCounter);
	for (uint8_t i=1; i<=8; i++)
	{
		if (bitmask(unRem,0x80)) {unRem = (unRem<<1) ^ 0x2F;}
		else {unRem <<= 1;}
	}
	unRem = ~unRem;
	return unRem;
}


/*===== CRC 16 bit =========================================================*/
uint16_t crc16(uint8_t unByte, uint16_t unGenPol)
{
	uint16_t unRem = (uint16_t)unByte<<8;		/* Remainder */
	
	for (uint8_t i=1; i<=8; i++)
	{
		if (bitmask(unRem,0x8000)) {unRem = (unRem<<1) ^ unGenPol;}
		else {unRem <<= 1;}
	}
	return unRem;
}

uint16_t crc16(uint8_t *aByteArray, uint16_t unLen, uint16_t unGenPol)
{
	uint16_t unRem = 0x0000;		/* Remainder */
	
	for (uint16_t j=0; j<unLen; j++)
	{
		unRem ^= ((*aByteArray++)<<8);
		for (uint8_t i=1; i<=8; i++)
		{
			if (bitmask(unRem,0x8000)) {unRem = (unRem<<1) ^ unGenPol;}
			else {unRem <<= 1;}
		}
	}
	return unRem;
}