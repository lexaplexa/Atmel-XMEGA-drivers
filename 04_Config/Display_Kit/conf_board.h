/*************************************************************************
 * conf_board.h
 *
 * Created: 3.8.2015 22:14:27
 * Revised: 16.2.2016
 * Author: LeXa
 * BOARD: Display_kit_A
 *
 * ABOUT:
 *
 *************************************************************************/


#ifndef	CONF_BOARD_H_
#define CONF_BOARD_H_

/************************************************************************/
/* BOARD                                                                */
/************************************************************************/
#define DISPLAY_KIT_A					/* Board name and version */


/************************************************************************/
/* SYSTEM CLOCK OPTIONS                                                 */
/************************************************************************/

#define F_CPU						CONF_SOURCE_FREQ*CONF_FREQ_MULTIPLICATION
#define CONF_SOURCE_FREQ			32000000
#define CONF_FREQ_MULTIPLICATION	1

/* Define source for PLL if used. Uncomment the right one */
//#define CONF_PLL_SOURCE			OSC_PLLSRC_XOSC_gc
//#define CONF_PLL_SOURCE			OSC_PLLSRC_RC2M_gc
//#define CONF_PLL_SOURCE			OSC_PLLSRC_RC32M_gc


/************************************************************************/
/* HARDWARE OPTIONS                                                     */
/************************************************************************/

	/*===== PORTA ==========================================================
	 *======================================================================*/
	#define	ADC0_PIN					PIN0_bp
	#define	ADC1_PIN					PIN1_bp
	#define	ADC2_PIN					PIN2_bp
	#define	ADC3_PIN					PIN3_bp
	#define	ADC4_PIN					PIN4_bp
	#define	ADC5_PIN					PIN5_bp
	#define	ADC6_PIN					PIN6_bp
	#define	ADC7_PIN					PIN7_bp
	
	/*===== PORTB ==========================================================
	 * Extension port
	 *======================================================================*/
	#define LED_PORT					PORTB
	
	#define	NTC_TEMP_PIN				ADC_CH_MUXPOS_PIN0_gc|(8<<3)
	#define	LIGHT_SENS_PIN				ADC_CH_MUXPOS_PIN1_gc|(8<<3)
	#define	ADC10_PIN					PIN2_bp
	#define	ADC11_PIN					PIN3_bp
	#define	ADC12_PIN					PIN4_bp
	#define	ADC13_PIN					PIN5_bp
	#define	ADC14_PIN					PIN6_bp
	#define	ADC15_PIN					PIN7_bp
	
	/*===== PORTC ==========================================================
	 * Extension port
	 *======================================================================*/	
	#define	PORTC0						PIN0_bp
	#define	PORTC1						PIN1_bp
	#define	PORTC2						PIN2_bp
	#define	PORTC3						PIN3_bp
	#define	PORTC4						PIN4_bp
	#define	PORTC5						PIN5_bp
	#define	PORTC6						PIN6_bp
	#define	PORTC7						PIN7_bp
	
	/*===== PORTD ==========================================================
	 *======================================================================*/
	#define PWM_PORT					PORTD		/* PWM port */
	#define PWM_TIMER					TCD0		/* PWM timer */
	#define FTDI_PORT					PORTD		/* FTDI port */
	#define FTDI_USART					USARTD0		/* FTDI USART */
	#define SD_PORT						PORTD		/* SD card port */
	#define SD_SPI						SPID		/* SD card SPI */
	
	#define	PWM_ILLU_PIN				PIN0_bp
	#define	PORTD1						PIN1_bp
	#define	FTDI_TX_PIN					PIN2_bp
	#define	FTDI_RX_PIN					PIN3_bp
	#define	SD_CS_PIN					PIN4_bp
	#define	SD_DI_PIN					PIN5_bp
	#define	SD_DO_PIN					PIN6_bp
	#define	SD_CLK_PIN					PIN7_bp
	
	/*===== PORTE ==========================================================
	 *======================================================================*/
	#define BUT_PORT					PORTE		/* Buttons port */
	#define DISP_CMD_PORT				PORTE		/* Command port */
	
	#define	BUT_1_PIN					PIN0_bp
	#define	BUT_2_PIN					PIN1_bp
	#define	BUT_3_PIN					PIN2_bp
	#define	BUT_4_PIN					PIN3_bp
	#define	DISP_XCS_PIN				PIN4_bp		/* Chip select signal (LOW=selected, HIGH=not selected) */
	#define	DISP_XWR_PIN				PIN5_bp		/* Writing control signal (write on rising edge) */
	#define	DISP_A0_PIN					PIN6_bp		/* Data command select signal (Data=HIGH, Command=LOW) */
	#define	DISP_XRES_PIN				PIN7_bp		/* Reset. Initialized when set to LOW */
	
	/*===== PORTF ==========================================================
	 *======================================================================*/
	#define DISP_DATA_PORT				PORTF		/* Data port */
	
	#define	DISP_D0_PIN					PIN0_bp
	#define	DISP_D1_PIN					PIN1_bp
	#define	DISP_D2_PIN					PIN2_bp
	#define	DISP_D3_PIN					PIN3_bp
	#define	DISP_D4_PIN					PIN4_bp
	#define	DISP_D5_PIN					PIN5_bp
	#define	DISP_D6_PIN					PIN6_bp
	#define	DISP_D7_PIN					PIN7_bp
	
	/*===== DISPLAY ========================================================
	 * Set orientation of display
	 * VERTICAL SCREEN
	 * MADCTL_P == 0x00		- writing to screen begins in TOP LEFT corner
	 * MADCTL_P == 0x40		- writing to screen begins in TOP RIGHT corner 
	 * MADCTL_P == 0x80		- writing to screen begins in BOTTOM LEFT corner 
	 * MADCTL_P == 0xC0		- writing to screen begins in BOTTOM RIGHT corner
	 * HORIZONTAL SCREEN
	 * MADCTL_P == 0x20		- writing to screen begins in TOP LEFT corner 
	 * MADCTL_P == 0x60		- writing to screen begins in TOP RIGHT corner
	 * MADCTL_P == 0xA0		- writing to screen begins in BOTTOM LEFT corner
	 * MADCTL_P == 0xE0		- writing to screen begins in BOTTOM RIGHT corner
	 *======================================================================*/
	#define MADCTL_P 0xA0
	
	
	/*===== BUTTONS ========================================================
	 *======================================================================*/
	#define NUM_OF_BUTTONS		4
	#define BUT_PORT_INT0_vect	PORTE_INT0_vect
	/* Buttons allocation to pins											*/
	enum BUTTONS_enum {
		BUT_1,
		BUT_2,
		BUT_3,
		BUT_4,
	};	
	
	
	/*===== PWM ============================================================
	 * Setting PWM for display illumination									
	 *======================================================================*/
	#define PWM_FRQ_HZ					200
	
	
	/*===== SD CARD ========================================================
	 *======================================================================*/
	#define SD_BAUD						16000000


	/*===== FTDI ===========================================================
	 * FTDI settings														
	 *======================================================================*/
	#define FTDI_BAUD					1000000
	#define FTDI_RECEIVE_vect			USARTD0_RXC_vect
	#define FTDI_RECEIVE_TIMEOUT_MS		20					/* Max. time to receive data */
	




/************************************************************************/
/* COMPILATION OPTIONS                                                  */
/************************************************************************/
	/*===== COMPILE ========================================================
	 * Uncomment if needed
	 *======================================================================*/
	#define COMPILE_BUTTON_RESET_FEATURE		/* After several presses application gets restarted if app get stuck */
	//#define COMPILE_DISP_PRINT_FILE			/* Print pictures from sd card */
	
	
#endif /* CONF_BOARD_H_ */