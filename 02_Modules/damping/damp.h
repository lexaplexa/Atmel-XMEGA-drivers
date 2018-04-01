/*****************************************************************************
 * damp.h
 *
 * Created: 22.7.2016 11:21:45
 * Revised: 31.3.2018
 * Author: LeXa
 * BOARD:
 * ABOUT:
 *
 *****************************************************************************/

#ifndef FILTER_H_
#define FILTER_H_

#include <avr/io.h>

/**
 * \brief   Damping changes of value
 * 
 * \param unCurrentVal          - Current value
 * \param unLastVal             - Last value
 * \param unDampCoefficient     - Damping coefficient
 * 
 * \return uint8_t
 */
uint8_t DampValue(uint8_t unCurrentVal, uint8_t unLastVal, uint8_t unDampCoefficient);

/**
 * \brief   Damping changes of value
 * 
 * \param unCurrentVal          - Current value
 * \param unLastVal             - Last value
 * \param unDampCoefficient     - Damping coefficient
 * 
 * \return int8_t
 */
int8_t DampValue(int8_t unCurrentVal, int8_t unLastVal, uint8_t unDampCoefficient);

/**
 * \brief   Damping changes of value
 * 
 * \param unCurrentVal          - Current value
 * \param unLastVal             - Last value
 * \param unDampCoefficient     - Damping coefficient
 * 
 * \return uint16_t
 */
uint16_t DampValue(uint16_t unCurrentVal, uint16_t unLastVal, uint8_t unDampCoefficient);

/**
 * \brief   Damping changes of value
 * 
 * \param unCurrentVal          - Current value
 * \param unLastVal             - Last value
 * \param unDampCoefficient     - Damping coefficient
 * 
 * \return int16_t
 */
int16_t DampValue(int16_t unCurrentVal, int16_t unLastVal, uint8_t unDampCoefficient);


#endif /* FILTER_H_ */