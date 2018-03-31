/*************************************************************************
 * conf_multitask.h
 *
 * Created: 4.1.2016 14:11:06
 * Revised: 31.3.2018
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 * Configuration of multitasking
 *************************************************************************/


#ifndef CONF_MULTITASK_H_
#define CONF_MULTITASK_H_

#include <tasks/tasks.h>

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
/* You can predefine task buffer. If you want to run task directly      */
/* after initialization, then set exact time match. In other case set   */
/* time match to -1 (free buffer position). Number of rows in           */
/* TASK_BUFFER_INIT must be same like TASK_BUFFER_SIZE.                 */
/************************************************************************/
#define TASK_BUFFER_SIZE            2

#endif /* CONF_MULTITASK_H_ */