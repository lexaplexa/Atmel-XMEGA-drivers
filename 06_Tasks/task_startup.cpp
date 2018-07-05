/*************************************************************************
 * task_startup.cpp
 *
 * Created: 11.10.2017 14:06:20
 * Revised: 24.6.2018
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 *
 *************************************************************************/

#include <core/core.h>

CLOCK cClock;

void taskStartUp()
{
    cMTask.Delay(taskStartUpApp,TASK_TOUT_MS(1));
}