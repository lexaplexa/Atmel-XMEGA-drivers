/*************************************************************************
 * pwm.h
 *
 * Created: 25.1.2015 21:24:22
 * Revised: 18.5.2015
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
		PORT_t *m_psPort;
		uint8_t m_unPinPosition;
		uint32_t m_unPwmFrequency;
		
	public:
		int8_t m_nPercent;
		
		PWM(PORT_t *psPort, TC0_t *psTimerCounter, uint8_t unPinPosition, PORT_OPC_enum ePinOPC, uint32_t unCpuFrequency, uint32_t unPwmFrequency);
		
		/* Set width of PWM in percent.
		 * 0 - 100% - PWM width
		 * 101%		- Capture compare value is bigger then Period value. Output is not refreshed and value is 1 */
		void SetWidth(int8_t nPercent);
		
		/* Set frequency of PWM */
		void SetFreq(uint32_t unCpuFrequency, uint32_t unPwmFrequency);
		
		/* PWM starts */
		void Run();
		
		/* PWM stops. Output value 0 */
		void Stop();
};

#endif /* PWM_H_ */