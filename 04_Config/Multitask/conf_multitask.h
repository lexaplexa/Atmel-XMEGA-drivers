/*************************************************************************
 * conf_multitask.h
 *
 * Created: 4.1.2016 14:11:06
 * Revised: 24.6.2018
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 * Configuration of multitasking
 *************************************************************************/


#ifndef CONF_MULTITASK_H_
#define CONF_MULTITASK_H_

#include <core/core.h>

/************************************************************************/
/* MULTITASK SETTINGS                                                   */
/************************************************************************/
#define MULTITASK_INT_vect          TCC0_OVF_vect
#define MULTITASK_TIMER             TCC0
#define TICKS_PER_SECOND            1000
#define STARTUP_TIMEOUT             10
#define DEEP_SLEEP                  false
#define BOOT_LOADER                 false
#define APP_BASE_ADDR               0x0000
#define APP_RESET_VECTOR_ADDR       APP_BASE_ADDR       /* Only for compatibility with Atmel SAM uC */


/************************************************************************/
/* TASK BUFFER                                                          */
/*----------------------------------------------------------------------*/
/* You can predefine task buffer size. Size number depends on how many  */
/* tasks you want to handle at the same time. Recommended minimal size  */
/* is 2.                                                                */
/************************************************************************/
#define TASK_BUFFER_SIZE            2

#endif /* CONF_MULTITASK_H_ */