/*****************************************************************************
 * bme280.h
 *
 * Created: 21.7.2016 8:21:24
 * Revised: 24.6.2018
 * Author: LeXa
 * 
 * ABOUT:
 * BME280 sensor for measuring temperature, atmospheric pressure and humidity.
 *
 * Typical measurement time:
 * --------------------------------------------------------------------------- 
 * t = 1 + (2*OverSampling_T) + (2*OverSampling_P+0.5) + (2*OverSampling_H+0.5) [ms]
 * Oversampling = 0 means not measured
 * ---------------------------------------------------------------------------
 *
 *****************************************************************************/


#ifndef BME280_H_
#define BME280_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <core/drivers/twi.h>

#define BME280_ADDR         0xEC    /* BME280 I2C identifier */

/************************************************************************/
/* REGISTERS                                                            */
/************************************************************************/
enum REG_enum {
    REG_CALIB_TEMP      = 0x88,
    REG_CALIB_PRESS     = 0x8E,
    REG_CALIB_HUM_1     = 0xA1,
    REG_CALIB_HUM_2     = 0xE1,
    REG_CTRL_HUM        = 0xF2,
    REG_STATUS          = 0xF3,
    REG_CTRL_MEAS       = 0xF4,
    REG_CONFIG          = 0xF5,
    REG_PRESS_MSB       = 0xF7,
    REG_TEMP_MSB        = 0xFA,
    REG_HUM_MSB         = 0xFD,
};

/************************************************************************/
/* COMMANDS                                                             */
/************************************************************************/
#define COMM_SOFT_RESET             0xB6


/************************************************************************/
/* OVERSAMPLING                                                         */
/************************************************************************/
enum OVERSAMPLING_enum {
    OVERSAMPLING_OFF,
    OVERSAMPLING_1x,
    OVERSAMPLING_2x,
    OVERSAMPLING_4x,
    OVERSAMPLING_8x,
    OVERSAMPLING_16x,
};

enum OS_TEMP_enum {
    OS_TEMP_OFF_gc  = OVERSAMPLING_OFF<<5,
    OS_TEMP_1x_gc   = OVERSAMPLING_1x <<5,
    OS_TEMP_2x_gc   = OVERSAMPLING_2x <<5,
    OS_TEMP_4x_gc   = OVERSAMPLING_4x <<5,
    OS_TEMP_8x_gc   = OVERSAMPLING_8x <<5,
    OS_TEMP_16x_gc  = OVERSAMPLING_16x<<5,
};

enum OS_PRESS_enum {
    OS_PRESS_OFF_gc = OVERSAMPLING_OFF<<2,
    OS_PRESS_1x_gc  = OVERSAMPLING_1x <<2,
    OS_PRESS_2x_gc  = OVERSAMPLING_2x <<2,
    OS_PRESS_4x_gc  = OVERSAMPLING_4x <<2,
    OS_PRESS_8x_gc  = OVERSAMPLING_8x <<2,
    OS_PRESS_16x_gc = OVERSAMPLING_16x<<2,
};

enum OS_HUM_enum {
    OS_HUM_OFF_gc   = OVERSAMPLING_OFF<<0,
    OS_HUM_1x_gc    = OVERSAMPLING_1x <<0,
    OS_HUM_2x_gc    = OVERSAMPLING_2x <<0,
    OS_HUM_4x_gc    = OVERSAMPLING_4x <<0,
    OS_HUM_8x_gc    = OVERSAMPLING_8x <<0,
    OS_HUM_16x_gc   = OVERSAMPLING_16x<<0,
};

/************************************************************************/
/* MODE                                                                 */
/************************************************************************/
enum MODE_enum {
    MODE_SLEEP,
    MODE_FORCED,
    MODE_NORMAL,
};

/************************************************************************/
/* FILTER COEFICIENT                                                    */
/************************************************************************/
enum FILTER_enum {
    FILTER_OFF_gc       = 0x00<<2,
    FILTER_COEF_2_gc    = 0x01<<2,
    FILTER_COEF_4_gc    = 0x02<<2,
    FILTER_COEF_8_gc    = 0x03<<2,
    FILTER_COEF_16_gc   = 0x04<<2,
};


/************************************************************************/
/* COMPENSATION DATA                                                    */
/************************************************************************/
union sCompData{
    struct {
        uint16_t dig_T1;
        int16_t dig_T2;
        int16_t dig_T3;
        uint16_t dig_P1;
        int16_t dig_P2;
        int16_t dig_P3;
        int16_t dig_P4;
        int16_t dig_P5;
        int16_t dig_P6;
        int16_t dig_P7;
        int16_t dig_P8;
        int16_t dig_P9;
        uint8_t dig_H1;
        int16_t dig_H2;
        uint8_t dig_H3;
        int16_t dig_H4;
        int16_t dig_H5;
        int8_t dig_H6;
        };
    uint8_t reg[33];
};


/************************************************************************/
/* CLASS                                                                */
/************************************************************************/
class BME280 {
    private:
        sCompData m_sCompData;
        TWI_t *m_psTwi;
        int32_t adc_T, adc_P, adc_H, t_fine, var1, var2, v_x1_u32r;
        
    public:
        int32_t m_nTemp;
        uint32_t m_unPress;
        uint32_t m_unHum;
            
        /**
         * \brief    BME280 constructor
         * 
         * \param psTwi          - pointer to TWI
         * \param unFcpu         - CPU frequency	
         * \param unFtwi         - TWI baud rate
         * 
         * \return 
         */
        BME280(TWI_t *psTwi, uint32_t unFcpu, uint32_t unFtwi)
        {
            m_psTwi = psTwi;
            
            TWI_MASTER_BAUD(m_psTwi, unFcpu, unFtwi);
            TWI_MASTER_ENABLE(m_psTwi);
            TWI_MASTER_SET_BUS_IDLE(m_psTwi);
            
            ReadReg(REG_CALIB_TEMP,&m_sCompData.reg[0],6);              /* Read Temp compensation data */
            ReadReg(REG_CALIB_PRESS,&m_sCompData.reg[6],18);            /* Read Pressure compensation data */
            ReadReg(REG_CALIB_HUM_1,&m_sCompData.reg[24],1);            /* Read Humidity compensation data */
            ReadReg(REG_CALIB_HUM_2,&m_sCompData.reg[25],8);            /* Read Humidity compensation data */
            m_sCompData.dig_H4 = (int16_t)m_sCompData.reg[28]<<4|m_sCompData.reg[29]&0x0F;
            m_sCompData.dig_H5 = m_sCompData.reg[30]>>4|(int16_t)m_sCompData.reg[31]<<4;
        }
        
    
        /**
         * \brief    Write data to register
         * 
         * \param eReg           - Register address
         * \param byte           - Byte to write
         * 
         * \return void
         */
        void WriteReg(REG_enum eReg, uint8_t byte)
        {
            cli();
            TWI_MASTER_WAIT_BUSY(m_psTwi);
            
            m_psTwi->MASTER.ADDR = BME280_ADDR & 0xFE;                  /* Address for writing */
            TWI_MASTER_WAIT_WRITE_DONE(m_psTwi);                        /* Wait until address is sent */
            TWI_MASTER_CLEAR_WRITE_FLAG(m_psTwi);                       /* Clear flag */
            
            if (TWI_MASTER_ARBLOST_FLAG(m_psTwi)) {return;}             /* Arbitration lost */
            TWI_MASTER_CLEAR_WRITE_FLAG(m_psTwi);                       /* Clear flag */
            
            m_psTwi->MASTER.DATA = eReg;                                /* Register address */
            TWI_MASTER_WAIT_WRITE_DONE(m_psTwi);                        /* Wait until byte is sent */
            TWI_MASTER_CLEAR_WRITE_FLAG(m_psTwi);                       /* Clear flag */
            
            m_psTwi->MASTER.DATA = byte;                                /* Write data to register */
            TWI_MASTER_WAIT_WRITE_DONE(m_psTwi);                        /* Wait until byte is sent */
            TWI_MASTER_CLEAR_WRITE_FLAG(m_psTwi);                       /* Clear flag */
            
            TWI_MASTER_STOP(m_psTwi);                                   /* Stop communication */
            sei();
        }
        
    
        /**
         * \brief    Read data from register
         * 
         * \param eReg       Register address
         * \param data       Data pointer
         * \param num        Number of data
         * 
         * \return void
         */
        void ReadReg(REG_enum eReg, uint8_t *data, uint8_t num)
        {
            cli();
            TWI_MASTER_WAIT_BUSY(m_psTwi);
            
            m_psTwi->MASTER.ADDR = BME280_ADDR & 0xFE;                  /* Write address */
            TWI_MASTER_WAIT_WRITE_DONE(m_psTwi);                        /* Wait until address is sent */
            
            if (TWI_MASTER_ARBLOST_FLAG(m_psTwi)) {return;}             /* Arbitration lost */
            TWI_MASTER_CLEAR_WRITE_FLAG(m_psTwi);                       /* Clear flag */
            
            m_psTwi->MASTER.DATA = eReg;                                /* Register address */
            TWI_MASTER_WAIT_WRITE_DONE(m_psTwi);                        /* Wait until byte is sent */
            TWI_MASTER_CLEAR_WRITE_FLAG(m_psTwi);                       /* Clear flag */
            
            m_psTwi->MASTER.ADDR = (BME280_ADDR & 0xFE)|0x01;           /* Address for reading */
            
            for (uint8_t i=0; i<num; i++)
            {
                TWI_MASTER_WAIT_READ_DONE(m_psTwi);	                    /* Wait until data received */
                TWI_MASTER_CLEAR_READ_FLAG(m_psTwi);                    /* Clear flag */
                data[i] = m_psTwi->MASTER.DATA;                         /* Read data from register */
                TWI_MASTER_ACKM(m_psTwi);                               /* Acknowledge */
            }
            
            TWI_MASTER_STOP(m_psTwi);                                   /* Stop communication */
            sei();
        }
        
        
        /**
         * \brief    Calculate temperature from ADC result and calibration data
         * 
         * 
         * \return int16_t       - Temperature (1bit = 0.01 °C)
         */
        int16_t CalcTemp()
        {
            uint8_t data[3];
            ReadReg(REG_TEMP_MSB,data,3);
            adc_T = (uint32_t)data[0]<<12|(uint32_t)data[1]<<4|data[2]>>4;
            
            var1 = ((((adc_T>>3) - ((int32_t)m_sCompData.dig_T1<<1))) * ((int32_t)m_sCompData.dig_T2)) >> 11;
            var2 = (((((adc_T>>4) - ((int32_t)m_sCompData.dig_T1)) * ((adc_T>>4) - ((int32_t)m_sCompData.dig_T1))) >> 12) *	((int32_t)m_sCompData.dig_T3)) >> 14;
            t_fine = var1 + var2;
            m_nTemp = (int32_t)(t_fine * 5 + 128) >> 8;
            return (uint16_t)m_nTemp;
        }
        
    
        /**
         * \brief    Calculate pressure from ADC result and calibration data
         * 
         * 
         * \return int32_t       - Pressure (1bit = 1 Pa)
         */
        uint32_t CalcPress()
        {
            uint8_t data[3];
            ReadReg(REG_PRESS_MSB,data,3);
            adc_P = (uint32_t)data[0]<<12|(uint32_t)data[1]<<4|data[2]>>4;
            
            var1 = (((int32_t)t_fine)>>1) - (int32_t)64000;
            var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((int32_t)m_sCompData.dig_P6);
            var2 = var2 + ((var1*((int32_t)m_sCompData.dig_P5))<<1);
            var2 = (var2>>2)+(((int32_t)m_sCompData.dig_P4)<<16);
            var1 = (((m_sCompData.dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((int32_t)m_sCompData.dig_P2) * var1)>>1))>>18;
            var1 =((((32768+var1))*((int32_t)m_sCompData.dig_P1))>>15);
            if (var1 == 0)
            {
                return 0; // avoid exception caused by division by zero
            }
            m_unPress = (((uint32_t)(((int32_t)1048576)-adc_P)-(var2>>12)))*3125;
            if (m_unPress < 0x80000000)
            {
                m_unPress = (m_unPress<< 1) / ((uint32_t)var1);
            }
            else
            {
                m_unPress = (m_unPress/ (uint32_t)var1) * 2;
            }
            var1 = (((int32_t)m_sCompData.dig_P9) * ((int32_t)(((m_unPress>>3) * (m_unPress>>3))>>13)))>>12;
            var2 = (((int32_t)(m_unPress>>2)) * ((int32_t)m_sCompData.dig_P8))>>13;
            m_unPress = (uint32_t)((int32_t)m_unPress + ((var1 + var2 + m_sCompData.dig_P7) >> 4));
            return m_unPress;
        }
        
        
        /**
         * \brief    Calculate humidity from ADC result and calibration data
         * 
         * 
         * \return uint32_t      - Humidity (1bit = 1/1024 %)
         */
        uint32_t CalcHum()
        {
            uint8_t data[2];
            ReadReg(REG_HUM_MSB,data,2);
            adc_H = (uint32_t)data[0]<<8|(uint32_t)data[1];
            
            v_x1_u32r = t_fine-76800;
            v_x1_u32r = (((((adc_H << 14) - (((int32_t)m_sCompData.dig_H4) << 20) - (((int32_t)m_sCompData.dig_H5) * v_x1_u32r)) + (16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)m_sCompData.dig_H6)) >> 10) * (((v_x1_u32r * ((int32_t)m_sCompData.dig_H3)) >> 11) + (32768))) >> 10) + (2097152)) * ((int32_t)m_sCompData.dig_H2) + 8192) >> 14));
            v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)m_sCompData.dig_H1)) >> 4));
            v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
            v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
            m_unHum = (uint32_t)(v_x1_u32r>>12);
            return m_unHum;
            
            /*var_H = (((double)t_fine) - 76800.0);
            var_H = (adc_H - (((double)m_sCompData.dig_H4) * 64.0 + ((double)m_sCompData.dig_H5) / 16384.0 * var_H)) *
            (((double)m_sCompData.dig_H2) / 65536.0 * (1.0 + ((double)m_sCompData.dig_H6) / 67108864.0 * var_H *
            (1.0 + ((double)m_sCompData.dig_H3) / 67108864.0 * var_H)));
            var_H = var_H * (1.0 - ((double)m_sCompData.dig_H1) * var_H / 524288.0);
            if (var_H > 100.0) {var_H = 100.0;}
            else if (var_H < 0.0) {var_H = 0.0;}
            return var_H;*/
        }
    
    };

#endif /* BME280_H_ */