/*************************************************************************
 * bmp180.h
 *
 * Created: 31.7.2014 20:38:26
 * Revised: 1.4.2018
 * Author: LeXa
 *
 * ABOUT:
 * Library for temperature and pressure sensor BMP180
 *
 * How to measure temperature
 * -------------------------------------------------------
 * 1) Start ADC for temperature
 *      WriteReg(BMP180_REG_CTRL_MEAS,COMM_TEMP_MEAS);
 * 2) Wait 4.5ms
 * 3) Calculate temperature from ADC result and calibration data
 *      CalcTemp();
 * -------------------------------------------------------
 *
 * How to measure pressure
 * -------------------------------------------------------
 * 1) Start ADC for pressure. Depends on accuracy you have choose
 *      WriteReg(BMP180_REG_CTRL_MEAS,COMM_PRESS_MEAS(SAMPLE_ACCURACY_LOW));
 *      WriteReg(BMP180_REG_CTRL_MEAS,COMM_PRESS_MEAS(SAMPLE_ACCURACY_STANDARD));
 *      WriteReg(BMP180_REG_CTRL_MEAS,COMM_PRESS_MEAS(SAMPLE_ACCURACY_HIGH));
 *      WriteReg(BMP180_REG_CTRL_MEAS,COMM_PRESS_MEAS(SAMPLE_ACCURACY_ULTRA_HIGH));
 * 2) Wait depends on accuracy you have choose
 *      SAMPLE_ACCURACY_LOW         - wait 4.5ms
 *      SAMPLE_ACCURACY_STANDARD    - wait 7.5ms
 *      SAMPLE_ACCURACY_HIGH        - wait 13.5ms
 *      SAMPLE_ACCURACY_ULTRA_HIGH  - wait 25.5ms
 * 3) Calculate pressure from ADC result and calibration data. Depends on accuracy you have choose
 *      CalcPress(SAMPLE_ACCURACY_LOW);
 *      CalcPress(SAMPLE_ACCURACY_STANDARD);
 *      CalcPress(SAMPLE_ACCURACY_HIGH);
 *      CalcPress(SAMPLE_ACCURACY_ULTRA_HIGH);
 * -------------------------------------------------------
 *************************************************************************/


#ifndef BMP180_H_
#define BMP180_H_

#include <avr/io.h>

//#define CALIB_DATA_DEFAULT        /* if uncommented, use calibration data from datasheet */

#define BMP180_ADDR         0xEE    /* BMP180 I2C identifier */

/************************************************************************/
/* REGISTER ADDRESSES                                                   */
/************************************************************************/
enum BMP180_REG_enum {
    BMP180_REG_CAL_DATA             = 0xAA,
    BMP180_REG_ID                   = 0xD0,
    BMP180_REG_SOFT_RESET           = 0xE0,
    BMP180_REG_CTRL_MEAS            = 0xF4,
    BMP180_REG_ADC                  = 0xF6,
};

/************************************************************************/
/* COMMANDS                                                             */
/************************************************************************/
#define COMM_SOFT_RESET             0xB6
#define COMM_TEMP_MEAS              0x2E
#define COMM_PRESS_MEAS(accuracy)   0x34|(accuracy<<6)

/************************************************************************/
/* SAMPLING ACCURACY                                                    */
/************************************************************************/
enum SAMPLE_ACCURACY_enum {
    SAMPLE_ACCURACY_LOW,
    SAMPLE_ACCURACY_STANDARD,
    SAMPLE_ACCURACY_HIGH,
    SAMPLE_ACCURACY_ULTRA_HIGH,
    };

/************************************************************************/
/* CALIBRATION DATA                                                     */
/************************************************************************/
typedef struct {
    int16_t     AC1;
    int16_t     AC2;
    int16_t     AC3;
    uint16_t    AC4;
    uint16_t    AC5;
    uint16_t    AC6;
    int16_t     B1;
    int16_t     B2;
    int16_t     MB;
    int16_t     MC;
    int16_t     MD;
}BMP180_CAL_DATA_t;

/************************************************************************/
/* CLASS                                                                */
/************************************************************************/
class BMP180
{
    private:
        TWI_t *m_psTwi;
        BMP180_CAL_DATA_t m_sCaldata;       /* Calibration data */
        int32_t x1, x2, x3, b3, b5, b6, p;
        uint32_t b4, b7, bmp180_adc;
            
    public:
        int16_t m_nTemp;
        int32_t m_nPress;
        BMP180(TWI_t *psTwi, uint32_t unFcpu, uint32_t unFtwi);
        
        /* Write data to register */
        void WriteReg(BMP180_REG_enum eReg, uint8_t byte);
        
        /* Read data from register */
        void ReadReg(BMP180_REG_enum eReg, uint8_t *data, uint8_t num);
        
        /* Calculate temperature from ADC result and calibration data */
        int16_t CalcTemp();
        
        /* Calculate pressure from ADC result and calibration data */
        int32_t CalcPress(SAMPLE_ACCURACY_enum eSmplAccy);
};

#endif /* BMP180_H_ */