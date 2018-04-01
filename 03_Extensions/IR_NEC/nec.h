/*************************************************************************
 * nec.h
 *
 * Created: 16.9.2015 11:51:04
 * Revised: 1.4.2018
 * Author: LeXa
 *
 * ABOUT:
 * NEC code is a pulse wide code used for infrared remote control.
 * For this library you need to connect IR receiver with demodulator to 
 * microcintroler. For example IR receiver TSOP4838 (Vishay).
 * This library does not demodulation! Only decoding.
 *************************************************************************/

#ifndef NEC_H_
#define NEC_H_

#include <avr/io.h>
#include <conf/conf_board.h>

/*===== HW AND SW CONFIGURATION ============================================
 *==========================================================================*/
#ifndef IR_COUNTER
    #define IR_COUNTER      TCC0
#endif
#ifndef IR_PORT
    #define IR_PORT         PORTC
#endif
#ifndef IR_PIN
	#define IR_PIN          6
#endif	


/*===== COUNTER DEFINES ====================================================
 *==========================================================================*/
#define TIMEOUT_US(x)       (x*(F_CPU/1000000))/8
#define START_COUNTER()     IR_COUNTER.CTRLA = TC_CLKSEL_DIV8_gc
#define CLEAR_COUNTER()     IR_COUNTER.CNT = 0
#define STOP_COUNTER()      IR_COUNTER.CTRLA = TC_CLKSEL_OFF_gc


/*===== PULSE LENGTHS ======================================================
 * Every pulse in NEC coding has different lengths. 
 * Start    -   13500us     (low 13000us    high 14000us)
 * 0        -    1125us     (low 1050us     high 1200us)
 * 1        -    2250us     (low 2100us     high 2400us)
 * Repeat   -   11812us     (low 11300us    high 12300us)
 *==========================================================================*/
#define PULSE_START_LOW     TIMEOUT_US(13000)
#define PULSE_START_HIGH    TIMEOUT_US(14000)
#define PULSE_ZERO_LOW      TIMEOUT_US(1050)
#define PULSE_ZERO_HIGH     TIMEOUT_US(1200)
#define PULSE_ONE_LOW       TIMEOUT_US(2100)
#define PULSE_ONE_HIGH      TIMEOUT_US(2400)
#define PULSE_REPEAT_LOW    TIMEOUT_US(11300)
#define PULSE_REPEAT_HIGH   TIMEOUT_US(12300)


/*===== RETURN CODES =======================================================
 *==========================================================================*/
enum NEC_RETURN_enum {
    NEC_RETURN_NOK,
    NEC_RETURN_OK,
    NEC_RETURN_WAIT,
    NEC_RETURN_RECEIVING,
    NEC_RETURN_COMPLETE,
    NEC_RETURN_REPEAT,
};


/*===== CLASS ==============================================================
 *==========================================================================*/
class NECCODE {
    private:
        NEC_RETURN_enum eStatus;
        uint16_t m_aunCounterValue[33];
        uint8_t m_unBitCounter;
        uint8_t m_unAddressInv;
        uint8_t m_unDataInv;
        
    public:
        uint8_t m_unAddress;
        uint8_t m_unData;
        uint8_t m_RepeatCounter;
    
        /**
         * \brief   Initialize port and counter
         * 
         * 
         * \return 
         */
        NECCODE()
        {
            /* IR pin configuration */
            IR_PORT.DIRCLR = 1<<IR_PIN;
            IR_PORT.INT0MASK = 1<<IR_PIN;
            IR_PORT.INTCTRL = PORT_INT0LVL_LO_gc;
#if IR_PIN == 0
            IR_PORT.PIN0CTRL = PORT_ISC_FALLING_gc|PORT_OPC_TOTEM_gc;
#elif IR_PIN == 1
            IR_PORT.PIN1CTRL = PORT_ISC_FALLING_gc|PORT_OPC_TOTEM_gc;
#elif IR_PIN == 2
            IR_PORT.PIN2CTRL = PORT_ISC_FALLING_gc|PORT_OPC_TOTEM_gc;
#elif IR_PIN == 3
            IR_PORT.PIN3CTRL = PORT_ISC_FALLING_gc|PORT_OPC_TOTEM_gc;
#elif IR_PIN == 4
            IR_PORT.PIN4CTRL = PORT_ISC_FALLING_gc|PORT_OPC_TOTEM_gc;
#elif IR_PIN == 5
            IR_PORT.PIN5CTRL = PORT_ISC_FALLING_gc|PORT_OPC_TOTEM_gc;
#elif IR_PIN == 6
            IR_PORT.PIN6CTRL = PORT_ISC_FALLING_gc|PORT_OPC_TOTEM_gc;
#elif IR_PIN == 7
            IR_PORT.PIN7CTRL = PORT_ISC_FALLING_gc|PORT_OPC_TOTEM_gc;
#endif
            
            /* COUNTER configuration */
            IR_COUNTER.INTCTRLA = TC_OVFINTLVL_LO_gc;
        };
        
        /**
         * \brief   Read pulse width and starts counting new pulse width
         * 
         * 
         * \return NEC_RETURN_enum
         */
        NEC_RETURN_enum ReadPulse()
        {
            /* Test if receive is starting or continues */
            if (eStatus != NEC_RETURN_RECEIVING)
            {
                CLEAR_COUNTER();
                START_COUNTER();
                m_unBitCounter = 0;
                eStatus = NEC_RETURN_RECEIVING;
                return eStatus;
            }
            
            /* Save time from the last bit (failing edge of signal) */
            m_aunCounterValue[m_unBitCounter++] = IR_COUNTER.CNT;
            CLEAR_COUNTER();
            eStatus = NEC_RETURN_RECEIVING;
            
            /* Test if Repeat is present */
            if (m_unBitCounter == 1 && m_aunCounterValue[0] >= PULSE_REPEAT_LOW && m_aunCounterValue[0] <= PULSE_REPEAT_HIGH)
            {
                STOP_COUNTER();
                CLEAR_COUNTER();
                m_RepeatCounter++;
                eStatus = NEC_RETURN_REPEAT;
                return NEC_RETURN_COMPLETE;
            }
            
            /* Last bit */
            if (m_unBitCounter >= 33)
            {
                STOP_COUNTER();
                CLEAR_COUNTER();
                m_RepeatCounter = 0;
                eStatus = NEC_RETURN_COMPLETE;
            }
            
            return eStatus;
        }
        
        /**
         * \brief   After all pulses are read, decoding bit values according pulse width
         * 
         * 
         * \return NEC_RETURN_enum
         */
        NEC_RETURN_enum Decode()
        {
            if (eStatus == NEC_RETURN_REPEAT) {eStatus = NEC_RETURN_WAIT; return NEC_RETURN_REPEAT;}
            else if (eStatus == NEC_RETURN_COMPLETE)
            {
                m_unAddress = 0;
                m_unAddressInv = 0;
                m_unData = 0;
                m_unDataInv = 0;
            }
            else {eStatus = NEC_RETURN_WAIT; return NEC_RETURN_NOK;}
            
            /* Test Start pulse length */
            if (m_aunCounterValue[0] <= PULSE_START_LOW || m_aunCounterValue[0] >= PULSE_START_HIGH) {return NEC_RETURN_NOK;}
            
            /* Decode Address */
            for (uint8_t i=1; i<=8; i++)
            {
                if (m_aunCounterValue[i] >= PULSE_ZERO_LOW && m_aunCounterValue[i] <= PULSE_ZERO_HIGH) {m_unAddress &= ~(1<<(i-1));}
                else if (m_aunCounterValue[i] >= PULSE_ONE_LOW && m_aunCounterValue[i] <= PULSE_ONE_HIGH) {m_unAddress |= (1<<(i-1));}
                else {return NEC_RETURN_NOK;}
            }
            
            /* Decode Address Inverse */
            for (uint8_t i=9; i<=16; i++)
            {
                if (m_aunCounterValue[i] >= PULSE_ZERO_LOW && m_aunCounterValue[i] <= PULSE_ZERO_HIGH) {m_unAddressInv &= ~(1<<(i-9));}
                else if (m_aunCounterValue[i] >= PULSE_ONE_LOW && m_aunCounterValue[i] <= PULSE_ONE_HIGH) {m_unAddressInv |= (1<<(i-9));}
                else {return NEC_RETURN_NOK;}
            }
            
            /* Decode Data */
            for (uint8_t i=17; i<=24; i++)
            {
                if (m_aunCounterValue[i] >= PULSE_ZERO_LOW && m_aunCounterValue[i] <= PULSE_ZERO_HIGH) {m_unData &= ~(1<<(i-17));}
                else if (m_aunCounterValue[i] >= PULSE_ONE_LOW && m_aunCounterValue[i] <= PULSE_ONE_HIGH) {m_unData |= (1<<(i-17));}
                else {return NEC_RETURN_NOK;}
            }
            
            /* Decode Data Inverse */
            for (uint8_t i=25; i<=32; i++)
            {
                if (m_aunCounterValue[i] >= PULSE_ZERO_LOW && m_aunCounterValue[i] <= PULSE_ZERO_HIGH) {m_unDataInv &= ~(1<<(i-25));}
                else if (m_aunCounterValue[i] >= PULSE_ONE_LOW && m_aunCounterValue[i] <= PULSE_ONE_HIGH) {m_unDataInv |= (1<<(i-25));}
                else {return NEC_RETURN_NOK;}
            }
            
            eStatus = NEC_RETURN_WAIT;
            return NEC_RETURN_OK;
        }
        
        /**
         * \brief   Stop counter and set all values to initial state
         * 
         * 
         * \return NEC_RETURN_enum
         */
        NEC_RETURN_enum Reset()
        {
            STOP_COUNTER();
            CLEAR_COUNTER();
            m_unBitCounter = 0;
            m_RepeatCounter = 0;
            m_unAddress = 0;
            m_unAddressInv = 0;
            m_unData = 0;
            m_unDataInv = 0;
            IR_COUNTER.INTFLAGS |= TC0_OVFIF_bm;
            eStatus = NEC_RETURN_WAIT;
            return NEC_RETURN_OK;
        }
};


#endif /* NEC_H_ */