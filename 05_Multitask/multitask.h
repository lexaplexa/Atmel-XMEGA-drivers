/*****************************************************************************
 * multitask.h
 *
 * Created: 4.12.2015 14:40:09
 * Revised: 8.12.2018
 * Author:  LeXa
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
/* Set number of ticks per second                                       */
/************************************************************************/
#ifndef TICKS_PER_SECOND
    #define TICKS_PER_SECOND            1000
#endif

#define TASK_TOUT_MS(x)                   (uint16_t)(((uint32_t)x*TICKS_PER_SECOND)/1000)

/************************************************************************/
/* START UP TIMEOUT                                                     */
/* Function StartUp is always first running function. It can be delayed */
/* after start.                                                         */
/************************************************************************/
#ifndef STARTUP_TIMEOUT
    #define STARTUP_TIMEOUT             10
#endif

/************************************************************************/
/* BOOT LOADER                                                          */
/* If controller starts in boot loader, application start and reset     */
/* interrupt vector must be defined.                                    */
/************************************************************************/
#ifndef BOOT_LOADER
    #define BOOT_LOADER                 false
#endif

#ifndef APP_BASE_ADDR
    #define APP_BASE_ADDR               0x0000
#endif

#ifndef APP_RESET_VECTOR_ADDR
    #define APP_RESET_VECTOR_ADDR       APP_BASE_ADDR + 4       /* Only for compatibility with Atmel SAM uC */
#endif

/************************************************************************/
/* MICRO TRACE BUFFER                                                   */
/* For debugging purposes. See www.arm.com                              */
/************************************************************************/
#ifndef MICRO_TRACE_BUFFER
    #define MICRO_TRACE_BUFFER          false
#endif

/************************************************************************/
/* TASK BUFFER                                                          */
/************************************************************************/
#ifndef TASK_BUFFER_SIZE
    #define TASK_BUFFER_SIZE            2
#endif

#define TASK_IDLE                       TASK_BUFFER_SIZE
#define TASK_BUFFER_FULL                TASK_BUFFER_SIZE


/************************************************************************/
/* TASK STRUCTURE                                                       */
/************************************************************************/
typedef void (*FuncPtr_t)();

struct TASK_struct {
    void* ptaskFunc;               /* Pointer to task function */
    int32_t nTimeMatch = -1;        /* Time point when task is ready to run. -1 = task free to use */
    uint32_t unTimeOut:16;          /* Timeout for task. Used for repeated task */
    uint32_t unPriority:3;          /* Task priority (0 - lowest, 7 - highest) */
    uint32_t bSuspend:1;            /* Task is suspended */
    uint32_t bRepeat:1;             /* Task is repeated */
};


/************************************************************************/
/* ENUMS                                                                */
/************************************************************************/
enum TASK_EVENT_TYPE_enum {
    TASK_EVENT_TYPE_TaskBufferOverflow,
    TASK_EVENT_TYPE_BeforeDeepSleep,
    TASK_EVENT_TYPE_AfterWakeUp,
    TASK_EVENT_TYPE_sum             /* Sum of all events. DO NOT DELETE!! */
};


/************************************************************************/
/* DEFAULT FUNCTION PROTOTYPES                                          */
/************************************************************************/
/**
 * \brief   Prototype to start up function
 * 
 * 
 * \return void
 */
void taskStartUp();

/**
 * \brief   Prototype of function which jumps to application
 *          Used only in boot loader
 * 
 * 
 * \return void
 */
void taskRunApp();


/************************************************************************/
/* CLASS                                                                */
/************************************************************************/
class MTASK
{
    private:
        int32_t m_nSysTime;                             /* System time in ticks */
        uint8_t m_unCurrentTask;                        /* Current running task, if == TASK_IDLE, then no task is running */
        uint8_t m_unActiveTasks;                        /* Number of active tasks */
        uint8_t m_unHighestPrio;                        /* Highest priority in a schedule loop */
        bool bDeepSleepEnabled = false;                 /* Deep sleep mode */
        TASK_struct m_sTask[TASK_BUFFER_SIZE];          /* Task buffer */
        void* m_peventFunc[TASK_EVENT_TYPE_sum];          /* Pointer to event function */
        
         
        /**
         * \brief   Find free task buffer position or task position with the same active function pointer. 
         *          Position with active function has priority.
         * 
         * \param taskFunc - pointer to task function
         * 
         * \return uint8_t  - task buffer position (0 - (TASK_BUFFER_SIZE-1)). Error if returned TASK_BUFFER_FULL
         */
        inline uint8_t unFreeOrRunPos(void taskFunc());
        
        
        /**
         * \brief   Find task buffer position with active function pointer.
         * 
         * \param taskFunc - pointer to task function
         * 
         * \return uint8_t  - task buffer position (0 - (TASK_BUFFER_SIZE-1)). Not available if returned TASK_IDLE
         */
        inline uint8_t unBufferPos(void taskFunc());
        
        
    public:
        /**
        * \brief   Multi task initialization. After that global interrupt must be enabled.
        *
        * \param unCpuFreq         - processor frequency
        * \param unTicksPerSecond  - ticks per second
        *
        * \return void
        */
        void inline Init(uint32_t unCpuFreq, uint32_t unTicksPerSecond);
        
        
        /**
        * \brief   Tick has been elapsed. System time is incremented.
        *          Handle this function in a interrupt routine, which is periodically generated.
        *
        * \return void
        */
        void inline TickElapsed();
        
        
        /**
        * \brief   Task Scheduler decides which task will run as next. Must be
        *          located in main loop.
        *
        * \return void
        */
        void inline Schedule();
        
        
        /**
        * \brief   Run immediately task function in next scheduler loop (depends also on priority)
        *
        * \param taskFunc       - task function
        *
        * \return void
        */
        void Run(void taskFunc());
        
        
        /**
         * \brief   Run immediately task function in next scheduler loop with priority
         * 
         * \param taskFunc      - task function
         * \param unPriority    - priority (0 -> lowest,..., 7 -> highest)
         * 
         * \return void
         */
        void Run(void taskFunc(), uint8_t unPriority);
        
                
        /**
        * \brief   Run task function after time out
        *
        * \param taskFunc       - task function
        * \param unTimeout      - time out in ticks
        *
        * \return void
        */
        void Delay(void taskFunc(), uint16_t unTimeout);
        
        /**
         * \brief   Run task function after time out with priority
         * 
         * \param taskFunc      - task function
         * \param unTimeout     - time out in ticks
         * \param unPriority    - priority (0 -> lowest,..., 7 -> highest)
         * 
         * \return void
         */
        void Delay(void taskFunc(), uint16_t unTimeout, uint8_t unPriority);
        
        
        /**
        * \brief   Repeat task function after time out
        *
        * \param taskFunc      - task function
        * \param unTimeout     - time out in ticks
        *
        * \return void
        */
        void Repeat(void taskFunc(), uint16_t unTimeout);
        
        
        /**
         * \brief   Repeat task function after time out with priority
         * 
         * \param taskFunc      - task function
         * \param unTimeout     - time out in ticks
         * \param unPriority    - priority (0 -> lowest,..., 7 -> highest)
         * 
         * \return void
         */
        void Repeat(void taskFunc(), uint16_t unTimeout, uint8_t unPriority);
        
        
        /**
        * \brief   Stop task function taskFunc
        *
        * \param taskFunc      - task function
        *
        * \return void
        */
        void Stop(void taskFunc());
        
        
        /**
        * \brief   Suspend task function taskFunc
        *
        * \param taskFunc      - task function
        *
        * \return void
        */
        void Suspend(void taskFunc());
        
        
        /**
        * \brief   Resume task function taskFunc
        *
        * \param taskFunc      - task function
        *
        * \return void
        */
        void Resume(void taskFunc());
        
        
        /**
        * \brief    Set priority to task function taskFunc
        *
        * \param taskFunc      - task function
        * \param unPriority    - priority (0 -> lowest,..., 7 -> highest)
        *
        * \return void
        */
        void SetPriority(void taskFunc(), uint8_t unPriority);
        
        
        /**
        * \brief   vTaskOrigin will be replaced by vTaskReplace if vTaskOrigin found in task buffer
        *
        * \param vTaskOrigin   - original task function to be replaced
        * \param vTaskReplace  - replaced by task function
        *
        * \return void
        */
        void Replace(void taskOrigin(), void taskReplace());
        
        
        /**
         * \brief   Enable deep sleep
         * 
         * 
         * \return void
         */
        void DeepSleepEnable() {bDeepSleepEnabled = true;}
        
        
        /**
         * \brief   Disable deep sleep
         * 
         * 
         * \return void
         */
        void DeepSleepDisable() {bDeepSleepEnabled = false;}
        
        
        /**
         * \brief   Get system time
         * 
         * 
         * \return int32_t      - system time
         */
        int32_t unGetSysTime() {return m_nSysTime;}
        
        
        /**
         * \brief   Assign function to a event type
         * 
         * \param eventType     - type of event
         * \param vCallBack     - event function
         * 
         * \return void
         */
        void SetEvent(TASK_EVENT_TYPE_enum eEventType, void vCallBack());
};

#endif /* MULTITASK_H_ */
