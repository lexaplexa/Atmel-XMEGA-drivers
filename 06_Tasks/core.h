/*************************************************************************
 * core.h
 *
 * Created: 24.6.2018 09:57:04
 * Revised: 
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 *
 *************************************************************************/


#ifndef CORE_H_
#define CORE_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#include <core/multitask/multitask.h>

#include <conf/conf_board.h>
#include <conf/conf_multitask.h>

#include <core/drivers/clock.h>


/************************************************************************/
/* EXTERNAL CLASSES                                                     */
/************************************************************************/
extern class MTASK cMTask;              /* core/multitask/multitask.cpp */


/************************************************************************/
/* TASK PROTOTYPES                                                      */
/*----------------------------------------------------------------------*/
/* Every task function is named void task<name of task>();              */
/* For example:                                                         */
/* void taskStartUp();                                                  */
/*                                                                      */
/* File names with source code are named task_<name of task>.cpp        */
/* For example:                                                         */
/* task_startup.cpp                                                     */
/*                                                                      */
/* Every task source code must include core.h                           */
/************************************************************************/


#endif /* CORE_H_ */