/*************************************************************************
 * conf_board.h
 *
 * Created: 13.1.2016 15:00:46
 * Revised: 31.3.2018
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 * Configuration of board
 *************************************************************************/

#ifndef CONF_BOARD_H_
#define CONF_BOARD_H_

/************************************************************************/
/* BOARD                                                                */
/************************************************************************/
    #define XPLAINED_A1                 /* Board name and version */


/************************************************************************/
/* SYSTEM CLOCK OPTIONS                                                 */
/************************************************************************/
    
    #define F_CPU                       CONF_SOURCE_FREQ*CONF_FREQ_MULTIPLICATION
    #define CONF_SOURCE_FREQ            32000000
    #define CONF_FREQ_MULTIPLICATION    1
    
    /* Define source for PLL if used. Uncomment the right one */
    //#define CONF_PLL_SOURCE           OSC_PLLSRC_XOSC_gc
    //#define CONF_PLL_SOURCE           OSC_PLLSRC_RC2M_gc
    //#define CONF_PLL_SOURCE           OSC_PLLSRC_RC32M_gc


/************************************************************************/
/* HARDWARE OPTIONS                                                     */
/************************************************************************/

    /*===== SUPPLY VOLTAGE =================================================*/
    #define SUPPLY_VOLTAGE          3.3
    
    
    /*===== BUTTONS ========================================================*/
    #define NUM_OF_BUTTONS          8
    #define BUT_PORT1               PORTD
    #define BUT_PORT2               PORTR
    #define BUT_PORT1_INT0_vect     PORTD_INT0_vect
    #define BUT_PORT2_INT0_vect     PORTR_INT0_vect
    /* Buttons allocation to pins in port                                   */
    enum BUTTONS_enum {
        BUT_SW0                     = PIN0_bp,
        BUT_SW1                     = PIN1_bp,
        BUT_SW2                     = PIN2_bp,
        BUT_SW3                     = PIN3_bp,
        BUT_SW4                     = PIN4_bp,
        BUT_SW5                     = PIN5_bp,
        BUT_SW6                     = PIN6_bp,
        BUT_SW7                     = PIN7_bp,
    };
    
    /*===== LEDs ===========================================================*/
    #define LED_PORT                PORTE
    #define LED_ON(pin)             LED_PORT.OUTCLR = 1<<pin
    #define LED_OFF(pin)            LED_PORT.OUTSET = 1<<pin
    #define LED_TGL(pin)            LED_PORT.OUTTGL = 1<<pin
    
    
    /*===== ADC ============================================================*/
    #define ADC_LIGHT_SENSOR        ADC_CH_MUXPOS_PIN1_gc
    #define ADC_TEMP_NTC            ADC_CH_MUXPOS_PIN0_gc
    
    
    /*===== NTC TEMPERATURE ================================================*/
    /* R = R0 * exp(B * (1/T - 1/T0))                                       */
    /* R    - Resistance in ambient temperature T                           */
    /* R0   - Resistance in ambient temperature T0                          */
    /* B    - Constant of thermistor                                        */
    
    #define NTC_PULLUP              1000000
    #define B                       4300
    #define R0                      100000
    #define T0                      298.15
    
    /*===== XPLAINED COM PORT ==============================================*/
    /* XPLAINED COM PORT settings                                           */
    #define XPLAINED_BAUD           1000000
    #define XPLAINED_USART          USARTC0
    #define XPLAINED_PORT           PORTC
    #define XPLAINED_RECEIVE_vect   USARTC0_RXC_vect

#endif /* CONF_BOARD_H_ */