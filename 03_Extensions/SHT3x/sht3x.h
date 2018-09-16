/*************************************************************************
 * sht3x.h
 *
 * Created: 10.9.2018 20:26:54
 * Revised: 16.9.2018
 * Author: LeXa
 *
 * ABOUT:
 * 
 *************************************************************************/


#ifndef SHT3X_H_
#define SHT3X_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <core/drivers/twi.h>

#define SHT3X_ADDR          0x88

/************************************************************************/
/* ENUMS                                                                */
/************************************************************************/
enum SHT3X_SINGLESHOT_ACCURACY {
    SHT3X_SINGLESHOT_ACCURACY_HIGH = 0x06,
    SHT3X_SINGLESHOT_ACCURACY_MEDIUM = 0x0D,
    SHT3X_SINGLESHOT_ACCURACY_LOW = 0x10,
};

/************************************************************************/
/* CLASS                                                                */
/************************************************************************/
class SHT3X
{
    private:
        TWI_t *m_psTwi;
        float m_fTempC;
        float m_fHum;
        
    public:
        /**
         * \brief Constructor
         * 
         * \param psTwi         - pointer TWI module
         * \param unFBaud       - TWI baud rate
         * \param unFCpu        - cpu frequency
         * 
         * \return 
         */
        SHT3X(TWI_t *psTwi, uint32_t unFBaud, uint32_t unFCpu);
        
        /**
         * \brief Start Single shot measure
         * 
         * \param eAccuracy     - measure accuracy
         * 
         * \return void
         */
        void SingleShotMeasure(SHT3X_SINGLESHOT_ACCURACY eAccuracy);
        
        /**
         * \brief Get temperature (°C)
         * 
         * 
         * \return float
         */
        float GetTempC() 
        {
            return m_fTempC;
        }
        
        /**
         * \brief Get relative humidity
         * 
         * 
         * \return float
         */
        float GetHum()
        {
            return m_fHum;
        }
        
};


#endif /* SHT3X_H_ */