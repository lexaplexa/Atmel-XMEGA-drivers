/*************************************************************************
 * tasks.h
 *
 * Created: 11.10.2017 14:07:04
 * Revised: 23.2.2018
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 *
 *************************************************************************/


#ifndef TASKS_H_
#define TASKS_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#include <multitask/multitask.h>

#include <conf/conf_board.h>
#include <conf/conf_multitask.h>

#include <drivers/clock.h>

extern class MTASK cMTask;			/* multitask/multitask.cpp */


/************************************************************************/
/* TASK PROTOTYPES                                                      */
/*----------------------------------------------------------------------*/
/* Every task function is named void task<name of task>();				*/
/* For example:															*/
/* void taskStartUp();													*/
/*																		*/
/* File names with source code are named task_<name of task>.cpp		*/
/* For example:															*/
/* task_startup.cpp														*/
/*																		*/
/* Every task source code must include tasks.h							*/
/************************************************************************/


#endif /* TASKS_H_ */