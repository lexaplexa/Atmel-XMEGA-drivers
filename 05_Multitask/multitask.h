/*****************************************************************************
 * multitask.h
 *
 * Created: 4.12.2015 14:40:09
 * Revised:	9.11.2017
 * Author: LeXa
 * BOARD: 
 * ABOUT:
 *
 *****************************************************************************/


#ifndef MULTITASK_H_
#define MULTITASK_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <conf/conf_multitask.h>
#include <conf/conf_board.h>


/************************************************************************/
/* TICKS_PER_SECOND                                                     */
/* Set number of ticks per second										*/
/************************************************************************/
#ifndef TICKS_PER_SECOND
	#define TICKS_PER_SECOND			1000
#endif

#define TIMEOUT_MS(x)					(uint16_t)(((uint32_t)x*TICKS_PER_SECOND)/1000)

/************************************************************************/
/* START UP TIMEOUT                                                     */
/* Function StartUp is always first running function. It can be delayed */
/* after start.															*/
/************************************************************************/
#ifndef STARTUP_TIMEOUT
	#define STARTUP_TIMEOUT				10
#endif

/************************************************************************/
/* DEEP SLEEP                                                           */
/* Controller can be put under deep sleep if no tasks are active.		*/
/* Multitask counter is not running. There are limited wake-up sources.	*/
/************************************************************************/
#ifndef DEEP_SLEEP
	#define DEEP_SLEEP					false
#endif

/************************************************************************/
/* BOOT LOADER                                                          */
/* If program starts in boot loader section, interrupt vectors has to	*/
/* be moved to boot loader section										*/
/************************************************************************/
#ifndef BOOT_LOADER
	#define BOOT_LOADER					false
#endif

#ifndef APP_BASE_ADDR
	#define APP_BASE_ADDR				0x0000
#endif

#ifndef APP_RESET_VECTOR_ADDR
	#define APP_RESET_VECTOR_ADDR		APP_BASE_ADDR		/* Only for compatibility with Atmel SAM uC */
#endif

/************************************************************************/
/* TASK BUFFER		                                                    */
/* Tasks can be predefined												*/
/************************************************************************/
#ifndef TASK_BUFFER_SIZE
	#define TASK_BUFFER_SIZE			2
#endif

#define TASK_IDLE						TASK_BUFFER_SIZE

#ifndef TASK_BUFFER_INIT
	#define TASK_BUFFER_INIT			{{Task_StartUp	,TIMEOUT_MS(STARTUP_TIMEOUT)	,0,0,false,false}}
#endif


/************************************************************************/
/* TASK STRUCTURE                                                       */
/************************************************************************/
struct TASK_struct {
	void (*pvTaskFunc)();			/* Pointer to task function */
	int32_t nTimeMatch;				/* Time point when task is ready to run. No time match = -1 */
	uint32_t unTimeOut:16;			/* Timeout for task */
	uint32_t unPriority:3;			/* Task priority (0 - lowest, 7 - highest) */
	uint32_t bSuspend:1;			/* Task is suspended */
	uint32_t bRepeat:1;				/* Task is repeated */
};


/************************************************************************/
/* FUNCTION DECLARATION                                                 */
/************************************************************************/

class MTASK
{
	private:
		
		int32_t			m_nSysTime;					/* System time in ticks */
		uint8_t			m_unCurrentTask;			/* Current running task, if == TASK_IDLE, then no task is running */
		uint8_t			m_unActiveTasks;			/* Number of active tasks */
		uint8_t			m_unHighestPrio;			/* Highest priority in a schedule loop */
		
	public:
		/**
		 * \brief Multi task initialization. After that global interrupt must be enabled.
		 *
		 * \param unCpuFreq			- CPU frequency
		 * \param unTicksPerSecond	- ticks per second
		 *
		 * \return void
		 */
		 void inline Init(uint32_t unCpuFreq, uint32_t unTicksPerSecond);
		 
		 
		 /**
		 * \brief	Tick has been elapsed. System time is incremented.
		 *			Handle this function in a interrupt routine, which is periodically generated.
		 *
		 * \return void
		 */
		 void inline TickElapsed();
		 
		 
		 /**
		 * \brief  Task Scheduler decides which task will run as next. Should be
		 *		   located on beginning of main loop.
		 *
		 * \return void
		 */
		 void inline Schedule();
		 
		 
		 /**
		 * \brief	Run immediately current running task function in next scheduler loop (depends also on priority)
		 *			Do not use this function during interrupt execution!
		 *
		 *
		 * \return void
		 */
		 void Run();
		 
		 
		 /**
		 * \brief	Run immediately task function in next scheduler loop (depends also on priority)
		 *
		 * \param vTaskFunc		- task function
		 *
		 * \return void
		 */
		 void Run(void vTaskFunc());
		 
		 
		 /**
		 * \brief	Run current running task function after time out
		 *			Do not use this function during interrupt execution!
		 *
		 * \param unTimeout		- time out in ticks
		 *
		 * \return void
		 */
		 void Delay(uint16_t unTimeout);
		 
		 
		 /**
		 * \brief	Run task function after time out
		 *
		 * \param vTaskFunc		- task function
		 * \param unTimeout		- time out in ticks
		 *
		 * \return void
		 */
		 void Delay(void vTaskFunc(), uint16_t unTimeout);
		 
		 
		 /**
		 * \brief	Repeat current running task function after time out
		 *			Do not use this function during interrupt execution!
		 *
		 * \param unTimeout		- time out in ticks
		 *
		 * \return void
		 */
		 void Repeat(uint16_t unTimeout);
		 
		 
		 /**
		 * \brief	Repeat task function after time out
		 *
		 * \param vTaskFunc		- task function
		 * \param unTimeout		- time out in ticks
		 *
		 * \return void
		 */
		 void Repeat(void vTaskFunc(), uint16_t unTimeout);
		 
		 
		 /**
		 * \brief	Stop current running task function
		 *			Do not use this function during interrupt execution!
		 *
		 *
		 * \return void
		 */
		 void Stop();
		 
		 
		 /**
		 * \brief	Stop task function vTaskFunc
		 *
		 * \param vTaskFunc		- task function
		 *
		 * \return void
		 */
		 void Stop(void vTaskFunc());
		 
		 
		 /**
		 * \brief	Suspend current running task function
		 *			Do not use this function during interrupt execution!
		 *
		 *
		 * \return void
		 */
		 void Suspend();
		 
		 
		 /**
		 * \brief	Suspend task function vTaskFunc
		 *
		 * \param vTaskFunc		- task function
		 *
		 * \return void
		 */
		 void Suspend(void vTaskFunc());
		 
		 
		 /**
		 * \brief	Resume current running task function
		 *			Do not use this function during interrupt execution!
		 *
		 *
		 * \return void
		 */
		 void Resume();
		 
		 
		 /**
		 * \brief	Resume task function vTaskFunc
		 *
		 * \param vTaskFunc		- task function
		 *
		 * \return void
		 */
		 void Resume(void vTaskFunc());
		 
		 
		 /**
		 * \brief	Set priority to current running task function.
		 *			Do not use this function during interrupt execution!
		 *
		 * \param unPriority	- priority (0 -> lowest,..., 7 -> highest)
		 *
		 * \return void
		 */
		 void SetPriority(uint8_t unPriority);
		 
		 
		 /**
		 * \brief	Set priority to task function vTaskFunc
		 *
		 * \param vTaskFunc		- task function
		 * \param unPriority	- priority (0 -> lowest,..., 7 -> highest)
		 *
		 * \return void
		 */
		 void SetPriority(void vTaskFunc(), uint8_t unPriority);
		 
		 
		 /**
		 * \brief	vTaskOrigin will be replaced by vTaskReplace if vTaskOrigin found in task buffer
		 *
		 * \param vTaskOrigin	- original task function to be replaced
		 * \param vTaskReplace	- replaced by task function
		 *
		 * \return void
		 */
		 void Replace(void vTaskOrigin(), void vTaskReplace());
};

		
/**
 * \brief	Prototype to start up function
 * 
 * 
 * \return void
 */
void Task_StartUp();


/**
 * \brief	Prototype of function which jumps to application
 *			Used only in boot loader
 * 
 * 
 * \return void
 */
void Task_RunApp();

#endif /* MULTITASK_H_ */