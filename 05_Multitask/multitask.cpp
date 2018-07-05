/*****************************************************************************
 * multitask.cpp
 *
 * Created: 4.12.2015 15:15:18
 * Revised: 24.6.2018
 * Author: LeXa
 * BOARD: 
 * ABOUT:
 *
 *****************************************************************************/

#include "multitask.h"

#define SysTimeOverflow             m_nSysTime < 0
#define isTaskActive(pos)           (m_sTask[pos].ptaskFunc)
#define isTaskReadyToRun(pos)       !m_sTask[pos].bSuspend && m_sTask[pos].nTimeMatch <= m_nSysTime && m_sTask[pos].unPriority >= m_unHighestPrio
#define setTaskInactive(pos)        m_sTask[pos].nTimeMatch = -1; m_sTask[pos].unPriority = 0; m_sTask[pos].ptaskFunc = 0

void inline MTASK::Init(uint32_t unCpuFreq, uint32_t unTicksPerSecond)
{
    MULTITASK_TIMER.CTRLA = 0<<TC0_CLKSEL3_bp|1<<TC0_CLKSEL2_bp|0<<TC0_CLKSEL1_bp|0<<TC0_CLKSEL0_bp;    /* Clock divided by 8 (0100) */
    MULTITASK_TIMER.CTRLB = 0<<TC0_WGMODE2_bp|0<<TC0_WGMODE1_bp|0<<TC0_WGMODE0_bp;                      /* Normal counter */
    MULTITASK_TIMER.PER = unCpuFreq/8/unTicksPerSecond;                                                 /* Set period for ticks */
    MULTITASK_TIMER.INTCTRLA = TC_OVFINTLVL_MED_gc;                                                     /* Set interrupt level */
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
    m_unCurrentTask = TASK_IDLE;    /* value TASK_IDLE means also no task to run */
    
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
        FuncPtr_t pvRunTask = (FuncPtr_t)m_sTask[m_unCurrentTask].ptaskFunc;
        if (m_sTask[m_unCurrentTask].bRepeat) {m_sTask[m_unCurrentTask].nTimeMatch = m_nSysTime + m_sTask[m_unCurrentTask].unTimeOut;}
        else {setTaskInactive(m_unCurrentTask);}        /* This task buffer position is inactive and free for another task */
        pvRunTask();                                    /* Run task */
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

inline uint8_t MTASK::unFreeOrRunPos(void taskFunc())
{
    uint8_t unBufPos = TASK_BUFFER_FULL;
    for (uint8_t i=0; i<TASK_BUFFER_SIZE; i++)
    {
        if (m_sTask[i].ptaskFunc == taskFunc) {return i;}
        else if (!isTaskActive(i)) {unBufPos = i;}
    }
    if (unBufPos == TASK_BUFFER_FULL && peventFunc[TASK_EVENT_TYPE_TaskBufferOverflow]) 
    {
        FuncPtr_t CallBack = (FuncPtr_t)peventFunc[TASK_EVENT_TYPE_TaskBufferOverflow]; 
        CallBack();
    }
    return unBufPos;
}

inline uint8_t MTASK::unBufferPos(void taskFunc())
{
    for (uint8_t i=0; i<TASK_BUFFER_SIZE; i++)
    {
        if (m_sTask[i].ptaskFunc == taskFunc) {return i;}
    }
    if (peventFunc[TASK_EVENT_TYPE_TaskBufferOverflow]) 
    {
        FuncPtr_t CallBack = (FuncPtr_t)peventFunc[TASK_EVENT_TYPE_TaskBufferOverflow]; 
        CallBack();
    }
    return TASK_BUFFER_FULL;
}

void MTASK::Run(void taskFunc())
{
    uint8_t unBufPos = unFreeOrRunPos(taskFunc);
    if (unBufPos == TASK_BUFFER_FULL) {return;}
    m_sTask[unBufPos].ptaskFunc = (void*)taskFunc;
    m_sTask[unBufPos].nTimeMatch = m_nSysTime;
    m_sTask[unBufPos].bSuspend = false;
    m_sTask[unBufPos].bRepeat = false;
}

void MTASK::Run(void taskFunc(), uint8_t unPriority)
{
    uint8_t unBufPos = unFreeOrRunPos(taskFunc);
    if (unBufPos == TASK_BUFFER_FULL) {return;}
    m_sTask[unBufPos].ptaskFunc = (void*)taskFunc;
    m_sTask[unBufPos].nTimeMatch = m_nSysTime;
    m_sTask[unBufPos].bSuspend = false;
    m_sTask[unBufPos].bRepeat = false;
    m_sTask[unBufPos].unPriority = unPriority;
}

void MTASK::Delay(void taskFunc(), uint16_t unTimeout)
{
    uint8_t unBufPos = unFreeOrRunPos(taskFunc);
    if (unBufPos == TASK_BUFFER_FULL) {return;}
    m_sTask[unBufPos].ptaskFunc = (void*)taskFunc;
    m_sTask[unBufPos].nTimeMatch = m_nSysTime + unTimeout;
    m_sTask[unBufPos].bRepeat = false;
    m_sTask[unBufPos].bSuspend = false;
}


void MTASK::Delay(void taskFunc(), uint16_t unTimeout, uint8_t unPriority)
{
    uint8_t unBufPos = unFreeOrRunPos(taskFunc);
    if (unBufPos == TASK_BUFFER_FULL) {return;}
    m_sTask[unBufPos].ptaskFunc = (void*)taskFunc;
    m_sTask[unBufPos].nTimeMatch = m_nSysTime + unTimeout;
    m_sTask[unBufPos].bRepeat = false;
    m_sTask[unBufPos].bSuspend = false;
    m_sTask[unBufPos].unPriority = unPriority;
}

void MTASK::Repeat(void taskFunc(), uint16_t unTimeout)
{
    uint8_t unBufPos = unFreeOrRunPos(taskFunc);
    if (unBufPos == TASK_BUFFER_FULL) {return;}
    m_sTask[unBufPos].ptaskFunc = (void*)taskFunc;
    m_sTask[unBufPos].nTimeMatch = m_nSysTime + unTimeout;
    m_sTask[unBufPos].unTimeOut = unTimeout;
    m_sTask[unBufPos].bRepeat = true;
    m_sTask[unBufPos].bSuspend = false;
}


void MTASK::Repeat(void taskFunc(), uint16_t unTimeout, uint8_t unPriority)
{
    uint8_t unBufPos = unFreeOrRunPos(taskFunc);
    if (unBufPos == TASK_BUFFER_FULL) {return;}
    m_sTask[unBufPos].ptaskFunc = (void*)taskFunc;
    m_sTask[unBufPos].nTimeMatch = m_nSysTime + unTimeout;
    m_sTask[unBufPos].unTimeOut = unTimeout;
    m_sTask[unBufPos].bRepeat = true;
    m_sTask[unBufPos].bSuspend = false;
    m_sTask[unBufPos].unPriority = unPriority;
}

void MTASK::Stop(void taskFunc())
{
    uint8_t unBufPos = unBufferPos(taskFunc);   
    if (unBufPos == TASK_IDLE) {return;}
    setTaskInactive(unBufPos);
    m_sTask[unBufPos].bSuspend = false;
    m_sTask[unBufPos].bRepeat = false;
}

void MTASK::Suspend(void taskFunc())
{
    uint8_t unBufPos = unBufferPos(taskFunc);
    if (unBufPos == TASK_IDLE) {return;}
    m_sTask[unBufPos].bSuspend = true;
}

void MTASK::Resume(void taskFunc())
{
    uint8_t unBufPos = unBufferPos(taskFunc);
    if (unBufPos == TASK_IDLE) {return;}
    m_sTask[m_unCurrentTask].bSuspend = false;
}

void MTASK::SetPriority(void taskFunc(), uint8_t unPriority)
{
    uint8_t unBufPos = unBufferPos(taskFunc);
    if (unBufPos == TASK_IDLE) {return;}
    m_sTask[unBufPos].unPriority = unPriority;
}


void MTASK::Replace(void taskOrigin(), void taskReplace())
{
    uint8_t unBufPos = unBufferPos(taskOrigin);
    if (unBufPos == TASK_IDLE) {return;}
    m_sTask[unBufPos].ptaskFunc = (void*)taskReplace;
}


void MTASK::SetEvent(TASK_EVENT_TYPE_enum eEventType, void vCallBack())
{
    if (eEventType >= TASK_EVENT_TYPE_sum) {return;}
    peventFunc[eEventType] = (void*)vCallBack;
}


/************************************************************************/
/* INTERRUPT HANDLER                                                    */
/************************************************************************/
MTASK cMTask;

SIGNAL(MULTITASK_INT_vect)
{
    cMTask.TickElapsed();
}


/************************************************************************/
/* RUN APPLICATION                                                      */
/************************************************************************/
#if (BOOT_LOADER == true)
    typedef void (*AppPtr_t)() __attribute__((noreturn));
    const AppPtr_t RunApp = (AppPtr_t) APP_RESET_VECTOR_ADDR;
    
    void taskRunApp()
    {
        cli();
        /* Multitask timer must be switched off, because application can use another timer for multitasking */
        MULTITASK_TIMER.CTRLA = 0;
        RunApp();
    }
#endif


/************************************************************************/
/* MAIN PROGRAM                                                         */
/************************************************************************/
int main(void)
{
    /* Multitask initialization */
    cMTask.Init(F_CPU,TICKS_PER_SECOND);
    cMTask.Delay(taskStartUp,TASK_TOUT_MS(STARTUP_TIMEOUT));

    /* Enable HIGH, MEDIUM and LOW level interrupt */
    CCP = CCP_IOREG_gc;     /* IVSEL is configuration change protected */
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
