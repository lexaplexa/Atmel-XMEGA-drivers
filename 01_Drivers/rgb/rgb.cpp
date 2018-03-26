/*************************************************************************
 * rgb.cpp
 *
 * Created: 17.9.2015 14:24:57
 * Revised: 21.9.2015
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 * Pulse Width Modulation RGB LEDs
 *************************************************************************/


#include "rgb.h"

#define COMPBUF(pin)		*(&m_psTimerCounter->CCABUF + m_unPinPos[pin])	//Addres to compare buffer

RGB::RGB(PORT_t *psPort, 
		 TC0_t *psTimerCounter, 
		 uint32_t unCpuFrequency, 
		 uint32_t unPwmFrequency, 
		 uint8_t unRedPinPos, 
		 uint8_t unGreenPinPos, 
		 uint8_t unBluePinPos,
		 PORT_OPC_enum ePinOPC)
{
	m_psPort = psPort;
	m_psTimerCounter = psTimerCounter;
	
	m_unPinPos[RGB_PIN_RED] =		unRedPinPos;
	m_unPinPos[RGB_PIN_GREEN] =		unGreenPinPos;
	m_unPinPos[RGB_PIN_BLUE] =		unBluePinPos;
	
	SetFreq(unCpuFrequency,unPwmFrequency);
	
	/* Set pin as output and low */
	m_psPort->OUTCLR = 1<<m_unPinPos[RGB_PIN_RED]|1<<m_unPinPos[RGB_PIN_GREEN]|1<<m_unPinPos[RGB_PIN_BLUE];	
	m_psPort->DIRSET = 1<<m_unPinPos[RGB_PIN_RED]|1<<m_unPinPos[RGB_PIN_GREEN]|1<<m_unPinPos[RGB_PIN_BLUE];
	
	/* Set Output and Pull configuration*/
	for (uint8_t eColor=RGB_COLOR_RED; eColor<=RGB_COLOR_BLUE; eColor++)
	{
		if (m_unPinPos[eColor] == PIN0_bp) {m_psPort->PIN0CTRL = ePinOPC;}
		else if (m_unPinPos[eColor] == PIN1_bp) {m_psPort->PIN1CTRL = ePinOPC;}
		else if (m_unPinPos[eColor] == PIN2_bp) {m_psPort->PIN2CTRL = ePinOPC;}
		else if (m_unPinPos[eColor] == PIN3_bp) {m_psPort->PIN3CTRL = ePinOPC;}
		else if (m_unPinPos[eColor] == PIN4_bp) {m_psPort->PIN4CTRL = ePinOPC; m_unPinPos[eColor] = PIN0_bp;}
		else if (m_unPinPos[eColor] == PIN5_bp) {m_psPort->PIN5CTRL = ePinOPC; m_unPinPos[eColor] = PIN1_bp;}
	}
		
	/* Single slope PWM mode */
	m_psTimerCounter->CTRLB |= 0<<TC0_WGMODE2_bp|1<<TC0_WGMODE1_bp|1<<TC0_WGMODE0_bp;
	
	/* Default is PWM stopped */
	Stop();
	
	/* Set default value */
	SetValue(RGB_COLOR_BRIGTHNESS,255);
}

void RGB::SetFreq(uint32_t unCpuFrequency, uint32_t unPwmFrequency)
{
	uint16_t div;
	m_unPwmFrequency = unPwmFrequency;
	
	/* Calculate divider. Period can be set up to 655535 */
	div = (uint16_t)((unCpuFrequency/unPwmFrequency)>>16);
	
	/* Set divider and period for chosen PWM frequency. Period has to be corrected by +1,5% */
	if (div < 1)		{m_psTimerCounter->CTRLA = 0<<TC0_CLKSEL2_bp|0<<TC0_CLKSEL1_bp|1<<TC0_CLKSEL0_bp; m_psTimerCounter->PERBUF = (uint16_t)(unCpuFrequency*1.015/unPwmFrequency);}
	else if (div < 2)	{m_psTimerCounter->CTRLA = 0<<TC0_CLKSEL2_bp|1<<TC0_CLKSEL1_bp|0<<TC0_CLKSEL0_bp; m_psTimerCounter->PERBUF = (uint16_t)(unCpuFrequency*1.015/2/unPwmFrequency);}
	else if (div < 4)	{m_psTimerCounter->CTRLA = 0<<TC0_CLKSEL2_bp|1<<TC0_CLKSEL1_bp|1<<TC0_CLKSEL0_bp; m_psTimerCounter->PERBUF = (uint16_t)(unCpuFrequency*1.015/4/unPwmFrequency);}
	else if (div < 8)	{m_psTimerCounter->CTRLA = 1<<TC0_CLKSEL2_bp|0<<TC0_CLKSEL1_bp|0<<TC0_CLKSEL0_bp; m_psTimerCounter->PERBUF = (uint16_t)(unCpuFrequency*1.015/8/unPwmFrequency);}
	else if (div < 64)	{m_psTimerCounter->CTRLA = 1<<TC0_CLKSEL2_bp|0<<TC0_CLKSEL1_bp|1<<TC0_CLKSEL0_bp; m_psTimerCounter->PERBUF = (uint16_t)(unCpuFrequency*1.015/64/unPwmFrequency);}
	else if (div < 256)	{m_psTimerCounter->CTRLA = 1<<TC0_CLKSEL2_bp|1<<TC0_CLKSEL1_bp|0<<TC0_CLKSEL0_bp; m_psTimerCounter->PERBUF = (uint16_t)(unCpuFrequency*1.015/256/unPwmFrequency);}
	else				{m_psTimerCounter->CTRLA = 1<<TC0_CLKSEL2_bp|1<<TC0_CLKSEL1_bp|1<<TC0_CLKSEL0_bp; m_psTimerCounter->PERBUF = (uint16_t)(unCpuFrequency*1.015/1024/unPwmFrequency);}
	/* Period register will be overwritten after counter overflows */
	m_psTimerCounter->CTRLGSET = TC0_PERBV_bm;
}

void RGB::SetValue(RGB_COLOR_enum eColor, uint8_t unValue)
{
	m_unValue[eColor] = unValue;
	
	if (eColor == RGB_COLOR_BRIGTHNESS)
	{
		COMPBUF(RGB_COLOR_RED) = (uint16_t) ((uint32_t)(m_psTimerCounter->PERBUF)*m_unValue[RGB_COLOR_RED]/255*m_unValue[RGB_COLOR_BRIGTHNESS]/255);
		COMPBUF(RGB_COLOR_GREEN) = (uint16_t) ((uint32_t)(m_psTimerCounter->PERBUF)*m_unValue[RGB_COLOR_GREEN]/255*m_unValue[RGB_COLOR_BRIGTHNESS]/255);
		COMPBUF(RGB_PIN_BLUE) = (uint16_t) ((uint32_t)(m_psTimerCounter->PERBUF)*m_unValue[RGB_COLOR_BLUE]/255*m_unValue[RGB_COLOR_BRIGTHNESS]/255);
	}
	else {COMPBUF(eColor) = (uint16_t) ((uint32_t)(m_psTimerCounter->PERBUF)*m_unValue[eColor]/255*m_unValue[RGB_COLOR_BRIGTHNESS]/255);}
	
	/* Compare registers will be overwritten after counter overflows */
	m_psTimerCounter->CTRLGSET = TC0_CCABV_bm|TC0_CCBBV_bm|TC0_CCCBV_bm|TC0_CCDBV_bm;
}

void RGB::SetValue(uint8_t unValueRed, uint8_t unValueGreen, uint8_t unValueBlue)
{
	m_unValue[RGB_COLOR_RED] = unValueRed;
	m_unValue[RGB_COLOR_GREEN] = unValueGreen;
	m_unValue[RGB_COLOR_BLUE] = unValueBlue;
	
	COMPBUF(RGB_PIN_RED) = (uint16_t) ((uint32_t)(m_psTimerCounter->PERBUF)*m_unValue[RGB_COLOR_RED]/255*m_unValue[RGB_COLOR_BRIGTHNESS]/255);
	COMPBUF(RGB_PIN_GREEN) = (uint16_t) ((uint32_t)(m_psTimerCounter->PERBUF)*m_unValue[RGB_COLOR_GREEN]/255*m_unValue[RGB_COLOR_BRIGTHNESS]/255);
	COMPBUF(RGB_PIN_BLUE) = (uint16_t) ((uint32_t)(m_psTimerCounter->PERBUF)*m_unValue[RGB_COLOR_BLUE]/255*m_unValue[RGB_COLOR_BRIGTHNESS]/255);
	
	/* Compare registers will be overwritten after counter overflows */
	m_psTimerCounter->CTRLGSET = TC0_CCABV_bm|TC0_CCBBV_bm|TC0_CCCBV_bm|TC0_CCDBV_bm;
}

void RGB::SetValue(uint8_t unValueRed, uint8_t unValueGreen, uint8_t unValueBlue, uint8_t unValueBrightness)
{
	m_unValue[RGB_COLOR_RED] = unValueRed;
	m_unValue[RGB_COLOR_GREEN] = unValueGreen;
	m_unValue[RGB_COLOR_BLUE] = unValueBlue;
	m_unValue[RGB_COLOR_BRIGTHNESS] = unValueBrightness;
	
	COMPBUF(RGB_PIN_RED) = (uint16_t) ((uint32_t)(m_psTimerCounter->PERBUF)*m_unValue[RGB_COLOR_RED]/255*m_unValue[RGB_COLOR_BRIGTHNESS]/255);
	COMPBUF(RGB_PIN_GREEN) = (uint16_t) ((uint32_t)(m_psTimerCounter->PERBUF)*m_unValue[RGB_COLOR_GREEN]/255*m_unValue[RGB_COLOR_BRIGTHNESS]/255);
	COMPBUF(RGB_PIN_BLUE) = (uint16_t) ((uint32_t)(m_psTimerCounter->PERBUF)*m_unValue[RGB_COLOR_BLUE]/255*m_unValue[RGB_COLOR_BRIGTHNESS]/255);
	
	/* Compare registers will be overwritten after counter overflows */
	m_psTimerCounter->CTRLGSET = TC0_CCABV_bm|TC0_CCBBV_bm|TC0_CCCBV_bm|TC0_CCDBV_bm;
}

void RGB::Run()
{
	for (uint8_t eColor=RGB_COLOR_RED; eColor<=RGB_COLOR_BLUE; eColor++) {m_psTimerCounter->CTRLB |= 1<<(m_unPinPos[eColor]+4);}	
}

void RGB::Stop()
{
	for (uint8_t eColor=RGB_COLOR_RED; eColor<=RGB_COLOR_BLUE; eColor++) {m_psTimerCounter->CTRLB &= ~(1<<(m_unPinPos[eColor]+4));}
}