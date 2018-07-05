/*************************************************************************
 * clock.h
 *
 * Created: 18.1.2015 20:04:52
 * Revised: 2.6.2017
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 * Setting system clock
 *************************************************************************/


#ifndef CLOCK_H_
#define CLOCK_H_

class CLOCK
{
    private:
    
    public:
        /**
         * \brief Set CPU frequency to 32MHz (RC oscillator)
         * 
         * 
         * \return 
         */
        CLOCK()
        {
            SetRC32M();
        }
        
        /**
         * \brief Set crystal oscillator frequency
         * 
         * \param unCrystalFreq     - crystal frequency
         * 
         * \return 
         */
        CLOCK(uint32_t unCrystalFreq)
        {
            SetXTAL(unCrystalFreq);
        }
        
        /**
         * \brief Set PPL oscillator
         * 
         * \param eSource           - source oscillator
         * \param unSourceFreq      - source frequency
         * \param unMultiplication  - multiplication of frequency
         * 
         * \return 
         */
        CLOCK(OSC_PLLSRC_enum eSource, uint32_t unSourceFreq, uint8_t unMultiplication)
        {
            SetPLL(eSource,unSourceFreq,unMultiplication);
        }
        
        /**
         * \brief Set CPU frequency to 2MHz (RC oscillator)
         * 
         * 
         * \return 
         */
        void SetRC2M()
        {
            /* ENABLE RC 2MHZ OSCILLATOR */
            OSC.CTRL = OSC_RC2MEN_bm;
            while (!(OSC.STATUS & OSC_RC2MRDY_bm));
            CCP = CCP_IOREG_gc;
            CLK.CTRL = CLK_SCLKSEL_RC2M_gc;
        }
        
        /**
         * \brief Set CPU frequency to 32MHz (RC oscillator)
         * 
         * 
         * \return 
         */
        void SetRC32M()
        {
            /* ENABLE RC 32MHZ OSCILLATOR */
            OSC.CTRL = OSC_RC32MEN_bm;
            while (!(OSC.STATUS & OSC_RC32MRDY_bm));
            CCP = CCP_IOREG_gc;
            CLK.CTRL = CLK_SCLKSEL_RC32M_gc;
        }
        
        /**
         * \brief Set crystal oscillator frequency
         * 
         * \param unCrystalFreq     - crystal frequency
         * 
         * \return 
         */
        void SetXTAL(uint32_t unCrystalFreq)
        {
            /* SETUP FREQUENCY RANGE */
            if (unCrystalFreq < 2000000) {OSC.XOSCCTRL = OSC_FRQRANGE_04TO2_gc|OSC_XOSCSEL_XTAL_1KCLK_gc;}
            else if (unCrystalFreq < 9000000) {OSC.XOSCCTRL = OSC_FRQRANGE_2TO9_gc|OSC_XOSCSEL_XTAL_1KCLK_gc;}
            else if (unCrystalFreq < 12000000) {OSC.XOSCCTRL = OSC_FRQRANGE_9TO12_gc|OSC_XOSCSEL_XTAL_1KCLK_gc;}
            else {OSC.XOSCCTRL = OSC_FRQRANGE_12TO16_gc|OSC_XOSCSEL_XTAL_1KCLK_gc;}
            
            /* ENABLE CRYSTAL OSCILATOR */
            OSC.CTRL = OSC_XOSCEN_bm;
            while (!(OSC.STATUS & OSC_XOSCRDY_bm));
            CCP = CCP_IOREG_gc;
            CLK.CTRL = CLK_SCLKSEL_XOSC_gc;
        }
        
        /**
         * \brief Set PPL oscillator
         * 
         * \param eSource           - source oscillator
         * \param unSourceFreq      - source frequency
         * \param unMultiplication  - multiplication of frequency
         * 
         * \return 
         */
        void SetPLL(OSC_PLLSRC_enum eSource, uint32_t unSourceFreq, uint8_t unMultiplication)
        {
            if (eSource == OSC_PLLSRC_XOSC_gc)
            {
                /* SETUP FREQUENCY RANGE */
                if (unSourceFreq < 2000000) {OSC.XOSCCTRL = OSC_FRQRANGE_04TO2_gc|OSC_XOSCSEL_XTAL_1KCLK_gc;}
                else if (unSourceFreq < 9000000) {OSC.XOSCCTRL = OSC_FRQRANGE_2TO9_gc|OSC_XOSCSEL_XTAL_1KCLK_gc;}
                else if (unSourceFreq < 12000000) {OSC.XOSCCTRL = OSC_FRQRANGE_9TO12_gc|OSC_XOSCSEL_XTAL_1KCLK_gc;}
                else {OSC.XOSCCTRL = OSC_FRQRANGE_12TO16_gc|OSC_XOSCSEL_XTAL_1KCLK_gc;}
                
                /* ENABLE CRYSTAL OSCILLATOR */
                OSC.CTRL |= OSC_XOSCEN_bm;
                while (!(OSC.STATUS & OSC_XOSCRDY_bm));
            }
            
            else if (eSource == OSC_PLLSRC_RC2M_gc)
            {
                /* ENABLE RC 2MHZ OSCILLATOR */
                OSC.CTRL = OSC_RC2MEN_bm;
                while (!(OSC.STATUS & OSC_RC2MRDY_bm));
            }
            
            else if (eSource == OSC_PLLSRC_RC32M_gc)
            {
                /* ENABLE RC 32MHZ OSCILLATOR */
                OSC.CTRL = OSC_RC32MEN_bm;
                while (!(OSC.STATUS & OSC_RC32MRDY_bm));
            }
            
            /* ENABLE PLL */
            OSC.PLLCTRL = eSource|unMultiplication<<OSC_PLLFAC_gp;
            OSC.CTRL |= OSC_PLLEN_bm;
            while (!(OSC.STATUS & OSC_PLLRDY_bm));
            CCP = CCP_IOREG_gc;
            CLK.CTRL = CLK_SCLKSEL_PLL_gc;
        }       
};

#endif /* CLOCK_H_ */