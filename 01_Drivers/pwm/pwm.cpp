/*************************************************************************
 * pwm.cpp
 *
 * Created: 25.1.2015 21:36:23
 * Revised: 31.3.2018
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 * Pulse Width Modulation
 *************************************************************************/

#include "pwm.h"

PWM::PWM(PORT_t *psPort, TC0_t *psTimerCounter, uint8_t unPinPosition, PORT_OPC_enum ePinOPC, uint32_t unCpuFrequency, uint32_t unPwmFrequency)
{
    m_psTimerCounter = psTimerCounter;
    m_unPinPosition = unPinPosition;
    
    /* Set Frequency */
    SetFreq(unCpuFrequency,unPwmFrequency);
    
    /* Set pin as output and low */
    psPort->OUTCLR = 1<<m_unPinPosition;
    psPort->DIRSET = 1<<m_unPinPosition;
    
    /* Set Output and Pull configuration*/
    if (m_unPinPosition == PIN0_bp)         {psPort->PIN0CTRL = ePinOPC;}
    else if (m_unPinPosition == PIN1_bp)    {psPort->PIN1CTRL = ePinOPC;}
    else if (m_unPinPosition == PIN2_bp)    {psPort->PIN2CTRL = ePinOPC;}
    else if (m_unPinPosition == PIN3_bp)    {psPort->PIN3CTRL = ePinOPC;}
    else if (m_unPinPosition == PIN4_bp)    {psPort->PIN4CTRL = ePinOPC; m_unPinPosition = PIN0_bp;}
    else if (m_unPinPosition == PIN5_bp)    {psPort->PIN5CTRL = ePinOPC; m_unPinPosition = PIN1_bp;}
        
    /* Single slope PWM mode */	
    m_psTimerCounter->CTRLB |= 0<<TC0_WGMODE2_bp|1<<TC0_WGMODE1_bp|1<<TC0_WGMODE0_bp;
    
    /* Default is PWM stopped */
    Stop();
}

void PWM::SetWidth(int16_t nPercent)
{
    m_nPercent = nPercent;
    if (m_nPercent < 0) {m_nPercent = 0;}
    if (m_nPercent >= 1000) 
    {
        m_nPercent = 1000;
        /* Compare value will be overwritten until counter overflows */
        *(&m_psTimerCounter->CCABUF + m_unPinPosition) = (uint16_t) ((uint32_t)(m_psTimerCounter->PERBUF)+1);
    }
    else
    {
        /* Compare value will be overwritten until counter overflows */
        *(&m_psTimerCounter->CCABUF + m_unPinPosition) = (uint16_t) ((uint32_t)(m_psTimerCounter->PERBUF)*m_nPercent/1000);
    }
}

void PWM::Add(int16_t nPercent)
{
    m_nPercent += nPercent;
    if (m_nPercent < 0) {m_nPercent = 0;}
    if (m_nPercent >= 1000)
    {
        m_nPercent = 1000;
        /* Compare value will be overwritten until counter overflows */
        *(&m_psTimerCounter->CCABUF + m_unPinPosition) = (uint16_t) ((uint32_t)(m_psTimerCounter->PERBUF)+1);
    }
    else
    {
        /* Compare value will be overwritten until counter overflows */
        *(&m_psTimerCounter->CCABUF + m_unPinPosition) = (uint16_t) ((uint32_t)(m_psTimerCounter->PERBUF)*m_nPercent/1000);
    }
}

void PWM::SetFreq(uint32_t unCpuFrequency, uint32_t unPwmFrequency)
{
    /* Calculate divider. Period can be set up to 655535 */
    uint16_t div = (uint16_t)((unCpuFrequency/unPwmFrequency)>>16);
    
    /* Set divider and period for chosen PWM frequency. Period has to be corrected by +1,5% */
    if      (div < 1)   {m_psTimerCounter->CTRLA = 0<<TC0_CLKSEL2_bp|0<<TC0_CLKSEL1_bp|1<<TC0_CLKSEL0_bp; m_psTimerCounter->PERBUF = (uint16_t)(unCpuFrequency*1.015/unPwmFrequency);}
    else if (div < 2)   {m_psTimerCounter->CTRLA = 0<<TC0_CLKSEL2_bp|1<<TC0_CLKSEL1_bp|0<<TC0_CLKSEL0_bp; m_psTimerCounter->PERBUF = (uint16_t)(unCpuFrequency*1.015/2/unPwmFrequency);}
    else if (div < 4)   {m_psTimerCounter->CTRLA = 0<<TC0_CLKSEL2_bp|1<<TC0_CLKSEL1_bp|1<<TC0_CLKSEL0_bp; m_psTimerCounter->PERBUF = (uint16_t)(unCpuFrequency*1.015/4/unPwmFrequency);}
    else if (div < 8)   {m_psTimerCounter->CTRLA = 1<<TC0_CLKSEL2_bp|0<<TC0_CLKSEL1_bp|0<<TC0_CLKSEL0_bp; m_psTimerCounter->PERBUF = (uint16_t)(unCpuFrequency*1.015/8/unPwmFrequency);}
    else if (div < 64)  {m_psTimerCounter->CTRLA = 1<<TC0_CLKSEL2_bp|0<<TC0_CLKSEL1_bp|1<<TC0_CLKSEL0_bp; m_psTimerCounter->PERBUF = (uint16_t)(unCpuFrequency*1.015/64/unPwmFrequency);}
    else if (div < 256) {m_psTimerCounter->CTRLA = 1<<TC0_CLKSEL2_bp|1<<TC0_CLKSEL1_bp|0<<TC0_CLKSEL0_bp; m_psTimerCounter->PERBUF = (uint16_t)(unCpuFrequency*1.015/256/unPwmFrequency);}
    else                {m_psTimerCounter->CTRLA = 1<<TC0_CLKSEL2_bp|1<<TC0_CLKSEL1_bp|1<<TC0_CLKSEL0_bp; m_psTimerCounter->PERBUF = (uint16_t)(unCpuFrequency*1.015/1024/unPwmFrequency);}
}

void PWM::Run()
{
    m_psTimerCounter->CTRLB |= 1<<(m_unPinPosition+4);
}

void PWM::Stop()
{
    m_psTimerCounter->CTRLB &= ~(1<<(m_unPinPosition+4));
}