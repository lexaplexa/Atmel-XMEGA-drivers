/*****************************************************************************
 * damp.cpp
 *
 * Created: 22.7.2016 11:38:31
 * Revised:
 * Author: LeXa
 * BOARD:
 * ABOUT:
 *
 *****************************************************************************/

#include "damp.h"

uint8_t DampValue(uint8_t unCurrentVal, uint8_t unLastVal, uint8_t unDampCoefficient)
{
	return (uint8_t)(((uint16_t)unLastVal*((uint16_t)unDampCoefficient-1)+(uint16_t)unCurrentVal)/unDampCoefficient);
}

int8_t DampValue(int8_t unCurrentVal, int8_t unLastVal, uint8_t unFilterCoefficient)
{
	return (int8_t)(((int16_t)unLastVal*((int16_t)unFilterCoefficient-1)+(int16_t)unCurrentVal)/unFilterCoefficient);
}

uint16_t DampValue(uint16_t unCurrentVal, uint16_t unLastVal, uint8_t unFilterCoefficient)
{
	return (uint16_t)(((uint32_t)unLastVal*((uint32_t)unFilterCoefficient-1)+(uint32_t)unCurrentVal)/unFilterCoefficient);
}

int16_t DampValue(int16_t unCurrentVal, int16_t unLastVal, uint8_t unFilterCoefficient)
{
	return (int16_t)(((int32_t)unLastVal*((int32_t)unFilterCoefficient-1)+(int32_t)unCurrentVal)/unFilterCoefficient);
}