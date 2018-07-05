/*************************************************************************
 * twi.h
 *
 * Created: 31.7.2014 23:33:05
 * Revised: 23.2.2018
 * Author: LeXa
 *
 * ABOUT:
 * Two Wire Interface driver
 *************************************************************************/


#ifndef TWI_H_
#define TWI_H_

/* Setting routines */
#define TWI_MASTER_BAUD(twi_ch, f_sys, f_twi)   twi_ch->MASTER.BAUD = f_sys/(2*f_twi)-5
#define TWI_MASTER_SET_BUS_IDLE(twi_ch)         twi_ch->MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc
#define TWI_MASTER_ENABLE(twi_ch)               twi_ch->MASTER.CTRLA |= TWI_MASTER_ENABLE_bm
#define TWI_MASTER_DISABLE(twi_ch)              twi_ch->MASTER.CTRLA &= ~TWI_MASTER_ENABLE_bm

/* Control bus routines */
#define TWI_MASTER_REPEAT_START(twi_ch)         twi_ch->MASTER.CTRLC = 0<<TWI_MASTER_ACKACT_bp|TWI_MASTER_CMD_REPSTART_gc
#define TWI_MASTER_ACKM(twi_ch)                 twi_ch->MASTER.CTRLC = 0<<TWI_MASTER_ACKACT_bp|TWI_MASTER_CMD_RECVTRANS_gc
#define TWI_MASTER_NACKM(twi_ch)                twi_ch->MASTER.CTRLC = 1<<TWI_MASTER_ACKACT_bp|TWI_MASTER_CMD_RECVTRANS_gc
#define TWI_MASTER_STOP(twi_ch)                 twi_ch->MASTER.CTRLC = 1<<TWI_MASTER_ACKACT_bp|TWI_MASTER_CMD_STOP_gc

/* Wait routines */
#define TWI_MASTER_WAIT_READ_DONE(twi_ch)       while(!(twi_ch->MASTER.STATUS & TWI_MASTER_RIF_bm))
#define TWI_MASTER_WAIT_WRITE_DONE(twi_ch)      while(!(twi_ch->MASTER.STATUS & TWI_MASTER_WIF_bm))
#define TWI_MASTER_WAIT_BUSY(twi_ch)            while((twi_ch->MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) == TWI_MASTER_BUSSTATE_BUSY_gc)

/* Flag routines */
#define TWI_MASTER_READ_FLAG(twi_ch)            twi_ch->MASTER.STATUS & TWI_MASTER_RIF_bm
#define TWI_MASTER_WRITE_FLAG(twi_ch)           twi_ch->MASTER.STATUS & TWI_MASTER_WIF_bm
#define TWI_MASTER_ARBLOST_FLAG(twi_ch)         twi_ch->MASTER.STATUS & TWI_MASTER_ARBLOST_bm
#define TWI_MASTER_BUSERR_FLAG(twi_ch)          twi_ch->MASTER.STATUS & TWI_MASTER_BUSERR_bm
#define TWI_MASTER_NACK_FLAG(twi_ch)            twi_ch->MASTER.STATUS & TWI_MASTER_RXACK_bm

#define TWI_MASTER_CLEAR_READ_FLAG(twi_ch)      twi_ch->MASTER.STATUS |= TWI_MASTER_RIF_bm
#define TWI_MASTER_CLEAR_WRITE_FLAG(twi_ch)     twi_ch->MASTER.STATUS |= TWI_MASTER_WIF_bm
#define TWI_MASTER_CLEAR_ARBLOST_FLAG(twi_ch)   twi_ch->MASTER.STATUS |= TWI_MASTER_ARBLOST_bm
#define TWI_MASTER_CLEAR_BUSERR_FLAG(twi_ch)    twi_ch->MASTER.STATUS |= TWI_MASTER_BUSERR_bm

#endif /* TWI_H_ */