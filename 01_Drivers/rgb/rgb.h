/*************************************************************************
 * rgb.h
 *
 * Created: 17.9.2015 14:15:29
 * Revised: 21.9.2015
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 * Pulse Width Modulation RGB LEDs
 *************************************************************************/


#ifndef RGB_H_
#define RGB_H_

#include <avr/io.h>

enum RGB_COLOR_enum {
    RGB_COLOR_RED,
    RGB_COLOR_GREEN,
    RGB_COLOR_BLUE,
    RGB_COLOR_BRIGTHNESS,
    RGB_COLOR_SUM,
    };

enum RGB_PIN_enum {
    RGB_PIN_RED,
    RGB_PIN_GREEN,
    RGB_PIN_BLUE,
    RGB_PIN_SUM,
};  
    
class RGB 
{
    private:
        TC0_t *m_psTimerCounter;
        PORT_t *m_psPort;
        uint32_t m_unPwmFrequency;      
        int8_t m_unPinPos[RGB_PIN_SUM];     
        uint8_t m_unValue[RGB_COLOR_SUM];
        
    public:
        RGB(PORT_t *psPort, 
            TC0_t *psTimerCounter, 
            uint32_t unCpuFrequency,
            uint32_t unPwmFrequency,
            uint8_t unRedPinPos, 
            uint8_t unGreenPinPos, 
            uint8_t unBluePinPos, 
            PORT_OPC_enum ePinOPC 
            );
        
        /* Set frequency of PWM */
        void SetFreq(uint32_t unCpuFrequency, uint32_t unPwmFrequency);
        
        /*  */
        void SetValue(RGB_COLOR_enum eColor, uint8_t unValue);
        
        /*  */
        void SetValue(uint8_t unValueRed, uint8_t unValueGreen, uint8_t unValueBlue);
        
        /*  */
        void SetValue(uint8_t unValueRed, uint8_t unValueGreen, uint8_t unValueBlue, uint8_t unValueBrightness);
        
        void Run();
        
        void Stop();
};



#endif /* RGB_H_ */