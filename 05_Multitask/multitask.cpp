/*****************************************************************************
 * multitask.cpp
 *
 * Created: 4.12.2015 15:15:18
 * Revised: 9.11.2017
 * Author: LeXa
 * BOARD: 
 * ABOUT:
 *
 *****************************************************************************/

#include "multitask.h"

TASK_struct									m_sTask[TASK_BUFFER_SIZE] =	TASK_BUFFER_INIT;
#define SysTimeOverflow						m_nSysTime < 0
#define isTaskActive(pos)					(m_sTask[pos].nTimeMatch >= 0)
#define isTaskReadyToRun(pos)				!m_sTask[pos].bSuspend && m_sTask[pos].nTimeMatch <= m_nSysTime && m_sTask[pos].unPriority >= m_unHighestPrio
#define setTaskInactive(pos)				m_sTask[pos].nTimeMatch = -1


void inline MTASK::Init(uint32_t unCpuFreq, uint32_t unTicksPerSecond)
{
	MULTITASK_TIMER.CTRLA = 0<<TC0_CLKSEL3_bp|1<<TC0_CLKSEL2_bp|0<<TC0_CLKSEL1_bp|0<<TC0_CLKSEL0_bp;	/* Clock divided by 8 (0100) */
	MULTITASK_TIMER.CTRLB = 0<<TC0_WGMODE2_bp|0<<TC0_WGMODE1_bp|0<<TC0_WGMODE0_bp;						/* Normal counter */
	MULTITASK_TIMER.PER = unCpuFreq/8/unTicksPerSecond;													/* Set period for ticks */
	MULTITASK_TIMER.INTCTRLA = TC_OVFINTLVL_MED_gc;														/* Set interrupt level */
}

void inline MTASK::TickElapsed()
{
	/* Increment system time */
	m_nSysTime++;
	
	/* When system time overflows, most significant bit of every time match needs to be cleared */
	if (SysTimeOverflow)
	{
		m_nSysTime = 0;
		for (uint8_t i=0; i<TASK_BUFFER_SIZE; i++)
		{
			if(isTaskActive(i)) {m_sTask[i].nTimeMatch &= 0x7FFFFFFF;}
		}
	}
	
	/* Increment time match if task is active and suspended */
	for (uint8_t i=0; i<TASK_BUFFER_SIZE; i++)
	{
		if ((isTaskActive(i)) && m_sTask[i].bSuspend) {m_sTask[i].nTimeMatch++;}
	}
}

void inline MTASK::Schedule()
{
	/* Clear to default values */
	m_unHighestPrio = 0;
	m_unActiveTasks = 0;
	m_unCurrentTask = TASK_IDLE;	/* value TASK_IDLE means also no task to run */
	
	/* Find task with highest priority */
	for (uint8_t i=0; i<TASK_BUFFER_SIZE; i++)
	{
		if (isTaskActive(i))
		{
			m_unActiveTasks++;
			if (isTaskReadyToRun(i)) {m_unCurrentTask = i; m_unHighestPrio = m_sTask[i].unPriority;}
		}
	}
	
	/* Run task if available */
	if (m_unCurrentTask != TASK_IDLE)
	{
		void (*pvRunTask)() = m_sTask[m_unCurrentTask].pvTaskFunc;
		if (m_sTask[m_unCurrentTask].bRepeat) {m_sTask[m_unCurrentTask].nTimeMatch = m_nSysTime + m_sTask[m_unCurrentTask].unTimeOut;}
		else {setTaskInactive(m_unCurrentTask);}		/* This task buffer position is inactive and free for another task */
		pvRunTask();									/* Run task */
	}
	
			
	/* If no active task available, CPU is going to POWER SAVE 
	 * sleep mode, else to IDLE sleep mode.
	 * Wait for next interrupt (tick or some else interrupt) */
	else if (DEEP_SLEEP && !m_unActiveTasks)
	{
		SLEEP.CTRL = SLEEP_SMODE_PSAVE_gc|SLEEP_SEN_bm;
		asm("sleep");
		SLEEP.CTRL = 0;
	}
	else
	{
		SLEEP.CTRL = SLEEP_SMODE_IDLE_gc|SLEEP_SEN_bm;
		asm("sleep");
		SLEEP.CTRL = 0;
	}
}


static inline uint8_t unFreeOrRunPos(void vTaskFunc())
{
	uint8_t unBufPos = TASK_IDLE;
	for (uint8_t i=0; i<TASK_BUFFER_SIZE; i++)
	{
		if (isTaskActive(i) && m_sTask[i].pvTaskFunc == vTaskFunc) {return i;}
		else if (!isTaskActive(i)) {unBufPos = i;}
	}
	return unBufPos;
}

static inline uint8_t unBufferPos(void vTaskFunc())
{
	for (uint8_t i=0; i<TASK_BUFFER_SIZE; i++)
	{
		if (isTaskActive(i) && m_sTask[i].pvTaskFunc == vTaskFunc) {return i;}
	}
	return TASK_IDLE;
}


void MTASK::Run()
{
	/* Return if idle, or application is not running in thread mode, but in interrupt */
	if (m_unCurrentTask == TASK_IDLE || PMIC.STATUS) {return;}
	m_sTask[m_unCurrentTask].nTimeMatch = m_nSysTime;
	m_sTask[m_unCurrentTask].bSuspend = false;
}


void MTASK::Run(void vTaskFunc())
{
	uint8_t unBufPos = unFreeOrRunPos(vTaskFunc);
	if (unBufPos == TASK_IDLE) {return;}
	m_sTask[unBufPos].pvTaskFunc = vTaskFunc;
	m_sTask[unBufPos].nTimeMatch = m_nSysTime;
	m_sTask[unBufPos].bSuspend = false;
}


void MTASK::Delay(uint16_t unTimeout)
{
	/* Return if idle, or application is not running in thread mode, but in interrupt */
	if (m_unCurrentTask == TASK_IDLE || PMIC.STATUS) {return;}
	m_sTask[m_unCurrentTask].nTimeMatch = m_nSysTime + unTimeout;
	m_sTask[m_unCurrentTask].bRepeat = false;
	m_sTask[m_unCurrentTask].bSuspend = false;
}


void MTASK::Delay(void vTaskFunc(), uint16_t unTimeout)
{
	uint8_t unBufPos = unFreeOrRunPos(vTaskFunc);
	if (unBufPos == TASK_IDLE) {return;}
	m_sTask[unBufPos].pvTaskFunc = vTaskFunc;
	m_sTask[unBufPos].nTimeMatch = m_nSysTime + unTimeout;
	m_sTask[unBufPos].bRepeat = false;
	m_sTask[unBufPos].bSuspend = false;
}


void MTASK::Repeat(uint16_t unTimeout)
{
	/* Return if idle, or application is not running in thread mode, but in interrupt */
	if (m_unCurrentTask == TASK_IDLE || PMIC.STATUS) {return;}
	m_sTask[m_unCurrentTask].nTimeMatch = m_nSysTime + unTimeout;
	m_sTask[m_unCurrentTask].bRepeat = true;
	m_sTask[m_unCurrentTask].bSuspend = false;
}


void MTASK::Repeat(void vTaskFunc(), uint16_t unTimeout)
{
	uint8_t unBufPos = unFreeOrRunPos(vTaskFunc);
	if (unBufPos == TASK_IDLE) {return;}
	m_sTask[unBufPos].pvTaskFunc = vTaskFunc;
	m_sTask[unBufPos].nTimeMatch = m_nSysTime + unTimeout;
	m_sTask[unBufPos].unTimeOut = unTimeout;
	m_sTask[unBufPos].bRepeat = true;
	m_sTask[unBufPos].bSuspend = false;
}


void MTASK::Stop()
{
	/* Return if idle, or application is not running in thread mode, but in interrupt */
	if (m_unCurrentTask == TASK_IDLE || PMIC.STATUS) {return;}
	setTaskInactive(m_unCurrentTask);
}


void MTASK::Stop(void vTaskFunc())
{
	uint8_t unBufPos = unBufferPos(vTaskFunc);
	if (unBufPos == TASK_IDLE) {return;}
	setTaskInactive(unBufPos);
}


void MTASK::Suspend()
{
	/* Return if idle, or application is not running in thread mode, but in interrupt */
	if (m_unCurrentTask == TASK_IDLE || PMIC.STATUS) {return;}
	m_sTask[m_unCurrentTask].bSuspend = true;
}


void MTASK::Suspend(void vTaskFunc())
{
	uint8_t unBufPos = unBufferPos(vTaskFunc);
	if (unBufPos == TASK_IDLE) {return;}
	m_sTask[unBufPos].bSuspend = true;
}


void MTASK::Resume()
{
	/* Return if idle, or application is not running in thread mode, but in interrupt */
	if (m_unCurrentTask == TASK_IDLE || PMIC.STATUS) {return;}
	m_sTask[m_unCurrentTask].bSuspend = false;
}


void MTASK::Resume(void vTaskFunc())
{
	uint8_t unBufPos = unBufferPos(vTaskFunc);
	if (unBufPos == TASK_IDLE) {return;}
	m_sTask[m_unCurrentTask].bSuspend = false;
}


void MTASK::SetPriority(uint8_t unPriority)
{
	/* Return if idle, or application is not running in thread mode, but in interrupt */
	if (m_unCurrentTask == TASK_IDLE || PMIC.STATUS) {return;}
	m_sTask[m_unCurrentTask].unPriority = unPriority;
}


void MTASK::SetPriority(void vTaskFunc(), uint8_t unPriority)
{
	uint8_t unBufPos = unBufferPos(vTaskFunc);
	if (unBufPos == TASK_IDLE) {return;}
	m_sTask[unBufPos].unPriority = unPriority;
}


void MTASK::Replace(void vTaskOrigin(), void vTaskReplace())
{
	uint8_t unBufPos = unBufferPos(vTaskOrigin);
	if (unBufPos == TASK_IDLE) {return;}
	m_sTask[unBufPos].pvTaskFunc = vTaskReplace;
}


/************************************************************************/
/* RUN APPLICATION                                                      */
/************************************************************************/
#if (BOOT_LOADER == true)
	typedef void (*AppPtr_t)() __attribute__((noreturn));
	const AppPtr_t RunApp = (AppPtr_t) APP_RESET_VECTOR_ADDR;
	
	void Task_RunApp()
	{
		cli();
		/* Multitask timer must be switched off, because application can use another timer for multitasking */
		MULTITASK_TIMER.CTRLA = 0;
		RunApp();
	}
#endif


/************************************************************************/
/* INTERRUPT HANDLER                                                    */
/************************************************************************/
MTASK cMTask;

SIGNAL(MULTITASK_INT_vect)
{
	cMTask.TickElapsed();
}


/************************************************************************/
/* MAIN PROGRAM                                                         */
/************************************************************************/
int main(void)
{
	/* Multitask initialization */
	cMTask.Init(F_CPU,TICKS_PER_SECOND);
	
	/* Enable HIGH, MEDIUM and LOW level interrupt */
	CCP = CCP_IOREG_gc;		/* IVSEL is configuration change protected */
#if BOOT_LOADER == true
	PMIC.CTRL = PMIC_IVSEL_bm|PMIC_HILVLEN_bm|PMIC_MEDLVLEN_bm|PMIC_LOLVLEN_bm;
#else	
	PMIC.CTRL = PMIC_HILVLEN_bm|PMIC_MEDLVLEN_bm|PMIC_LOLVLEN_bm;
#endif	
	sei();
	
	/* MAIN LOOP */
	while (1)
	{
		cMTask.Schedule();
	}
}