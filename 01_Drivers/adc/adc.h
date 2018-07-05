/*************************************************************************
 * adc.h
 *
 * Created: 8.10.2014 11:38:24
 * Revised: 1.7.2016
 * Author: LeXa
 * BOARD: 
 *
 * ABOUT:
 *
 *************************************************************************/


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

#define ADC_REF_SOURCE                      0x00

/* Set reference voltage */
#if (ADC_REF_SOURCE == 0x20)                /* ADC_REFSEL_AREFA_gc */
#define ADC_REF_VOLTAGE                     2.8
#elif (ADC_REF_SOURCE == 0x30)              /* ADC_REFSEL_AREFB_gc */
#define ADC_REF_VOLTAGE                     2.8
#elif (ADC_REF_SOURCE == 0x00)              /* ADC_REFSEL_INT1V_gc */
#define ADC_REF_VOLTAGE                     1
#elif (ADC_REF_SOURCE == 0x10)              /* ADC_REFSEL_VCC_gc */
#define ADC_REF_VOLTAGE                     3.3/1.5
#endif

/* Calculate voltage from ADC */
#define ADC_TO_VOLT(x)                      (x*ADC_REF_VOLTAGE/2048)
#define ADC_TO_VOLT_GAIN(x,gain)            (x*ADC_REF_VOLTAGE/2048/gain)

/* ADC routines */
#define adc_init(adc,pscale,res)            adc.CTRLA = ADC_ENABLE_bm;          \
                                            adc.CTRLB = 1<<ADC_CONMODE_bp|res;  \
                                            adc.PRESCALER = pscale;             \
                                            adc.REFCTRL = ADC_REF_SOURCE

#define adc_read(adc,pos,result)            adc.CH0.MUXCTRL = pos;                                                              \
                                            adc.CH0.CTRL = ADC_CH_START_bm|ADC_CH_GAIN_1X_gc|ADC_CH_INPUTMODE_SINGLEENDED_gc;   \
                                            while (!(adc.INTFLAGS & ADC_CH_CHIF_bm));                                           \
                                            adc.INTFLAGS |= ADC_CH_CHIF_bm;                                                     \
                                            result = adc.CH0RES
                                            

/************************************************************************/
/* CLASS                                                                */
/************************************************************************/
class ADC
{
    private:
        int16_t m_nADCresult;
        ADC_t *m_psADC;
        
    public:
        ADC(ADC_t *psADC, ADC_RESOLUTION_enum eRes, ADC_PRESCALER_enum ePrescale)
        {
            m_psADC = psADC;
            m_psADC->CTRLA = ADC_ENABLE_bm;
            m_psADC->CTRLB = 1<<ADC_CONMODE_bp|eRes;
            m_psADC->PRESCALER = ePrescale;
            m_psADC->REFCTRL = ADC_REF_SOURCE;
        }
        
        uint16_t Read(ADC_CH_MUXPOS_enum eMuxPos, ADC_CH_GAIN_enum eGain)
        {
            m_psADC->CH0.MUXCTRL = eMuxPos|ADC_CH_MUXNEG_INTGND_MODE3_gc;                               /* Select analog pin */
            m_psADC->CH0.CTRL = ADC_CH_START_bm|eGain|ADC_CH_INPUTMODE_DIFFWGAIN_gc;
            while (!(m_psADC->INTFLAGS & ADC_CH_CHIF_bm));              /* Wait for conversion result */
            m_psADC->INTFLAGS |= ADC_CH_CHIF_bm;                        /* Clear flag */
            return (m_nADCresult = m_psADC->CH0RES);
        }


        uint16_t Read(ADC_CH_MUXPOS_enum eMuxPos)
        {
            m_psADC->CH0.MUXCTRL = eMuxPos;                             /* Select analog pin */
            m_psADC->CH0.CTRL = ADC_CH_START_bm|ADC_CH_GAIN_1X_gc|ADC_CH_INPUTMODE_SINGLEENDED_gc;
            while (!(m_psADC->INTFLAGS & ADC_CH_CHIF_bm));              /* Wait for conversion result */
            m_psADC->INTFLAGS |= ADC_CH_CHIF_bm;                        /* Clear flag */
            return (m_nADCresult = m_psADC->CH0RES);
        }
};

#endif /* ADC_H_ */