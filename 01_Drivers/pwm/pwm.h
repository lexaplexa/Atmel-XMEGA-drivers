/*************************************************************************
 * pwm.h
 *
 * Created: 25.1.2015 21:24:22
 * Revised: 31.3.2018
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 * Pulse Width Modulation
 *************************************************************************/


#ifndef PWM_H_
#define PWM_H_

#include <avr/io.h>

class PWM
{
    private:
        TC0_t *m_psTimerCounter;
        uint8_t m_unPinPosition;
        
    public:
        int16_t m_nPercent;
        
        
        /**
        * \brief 
        * 
        * \param psPort
        * \param psTimerCounter
        * \param unPinPosition
        * \param ePinOPC
        * \param unCpuFrequency
        * \param unPwmFrequency
        * 
        * \return 
        */
        PWM(PORT_t *psPort, TC0_t *psTimerCounter, uint8_t unPinPosition, PORT_OPC_enum ePinOPC, uint32_t unCpuFrequency, uint32_t unPwmFrequency);
        
        
        /**
        * \brief 
        * 
        * \param nPercent      - 1bit = 0.1%
        * 
        * \return void
        */
        void SetWidth(int16_t nPercent);
        
        
        /**
        * \brief 
        * 
        * \param nPercent      - 1bit = 0.1%
        * 
        * \return void
        */
        void Add(int16_t nPercent);
        
        
        /**
        * \brief 
        * 
        * \param unCpuFrequency
        * \param unPwmFrequency
        * 
        * \return void
        */
        void SetFreq(uint32_t unCpuFrequency, uint32_t unPwmFrequency);
        
        
        /**
        * \brief 
        * 
        * 
        * \return void
        */
        void Run();
        
        
        /**
        * \brief 
        * 
        * 
        * \return void
        */
        void Stop();
};

#endif /* PWM_H_ */