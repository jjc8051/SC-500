/**
 * \file
 *
 * \brief ADC Basic driver implementation.
 *
 (c) 2020 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms,you may use this software and
    any derivatives exclusively with Microchip products.It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

/**
 * \defgroup doc_driver_adc_basic ADC Basic Driver
 * \ingroup doc_driver_adc
 *
 * \section doc_driver_adc_rev Revision History
 * - v0.0.0.1 Initial Commit
 *
 *@{
 */

#include <adc.h>

//=================================================================================================
/**
 * \brief Initialize ADC interface
 * If module is configured to disabled state, the clock to the ADC is disabled
 * if this is supported by the device's clock system.
 *
 * \return Initialization status.
 * \retval 0 the ADC init was successful
 * \retval 1 the ADC init was not successful
 */
//=================================================================================================
#ifdef ADC_SET							// ADC 사용 설정
int8_t ADC_init()
{

	// ADC0.CTRLB = ADC_SAMPNUM_NONE_gc; /* No accumulation */

	ADC0.CTRLC = ADC_PRESC_DIV24_gc; /* CLK_PER divided by 24 */

	// ADC0.CTRLD = 0x0 << ADC_SAMPDLY_gp /* Sampling Delay Selection: 0x0 */
	//		 | ADC_INITDLY_DLY0_gc; /* Delay 0 CLK_ADC cycles */

	// ADC0.CTRLE = ADC_WINCM_NONE_gc; /* No Window Comparison */

	// ADC0.DBGCTRL = 0 << ADC_DBGRUN_bp; /* Debug run: disabled */

	// ADC0.EVCTRL = 0 << ADC_STARTEI_bp; /* Start Event Input Enable: disabled */

	// ADC0.INTCTRL = 0 << ADC_RESRDY_bp /* Result Ready Interrupt Enable: disabled */
	//		 | 0 << ADC_WCMP_bp; /* Window Comparator Interrupt Enable: disabled */

	// ADC0.MUXPOS = ADC_MUXPOS_AIN0_gc; /* ADC input pin 0 */

	// ADC0.SAMPCTRL = 0x0 << ADC_SAMPLEN_gp; /* Sample length: 0x0 */

	// ADC0.WINHT = 0x0; /* Window Comparator High Threshold: 0x0 */

	// ADC0.WINLT = 0x0; /* Window Comparator Low Threshold: 0x0 */

	ADC0.CTRLA = 1 << ADC_ENABLE_bp    /* ADC Enable: enabled */
	           | 0 << ADC_FREERUN_bp   /* ADC Freerun mode: disabled */
	           | ADC_RESSEL_12BIT_gc   /* 12-bit mode */
	           | 0 << ADC_RUNSTBY_bp   /* Run standby mode: disabled */
	           | 0 << ADC_LEFTADJ_bp   /* Left Adjust Result: disabled */
	           | 0 << ADC_CONVMODE_bp; /* Differential Mode Conversion: disabled */
	return 0;
}
#endif

//=================================================================================================
/**
 * \brief Enable ADC
 * 1. If supported by the clock system, enables the clock to the ADC
 * 2. Enables the ADC module by setting the enable-bit in the ADC control register
 *
 * \return Nothing
 */
//=================================================================================================
#ifdef ADC_SET							// ADC 사용 설정
void ADC_enable()
{
	ADC0.CTRLA |= ADC_ENABLE_bm;
}
#endif

//=================================================================================================
/**
 * \brief Disable ADC
 * 1. Disables the ADC module by clearing the enable-bit in the ADC control register
 * 2. If supported by the clock system, disables the clock to the ADC
 *
 * \return Nothing
 */
//=================================================================================================
#ifdef ADC_SET							// ADC 사용 설정
void ADC_disable()
{
	ADC0.CTRLA &= ~ADC_ENABLE_bm;
}
#endif

//=================================================================================================
/**
 * \brief Start a conversion on ADC
 *
 * \param[in] channel The ADC channel to start conversion on
 *
 * \return Nothing
 */
//=================================================================================================
#ifdef ADC_SET								// ADC 사용 설정
void ADC_start_conversion(adc_channel_t channel)
{
	ADC0.CTRLA  &= ~ADC_CONVMODE_bm;
	ADC0.MUXPOS  = channel;
	ADC0.COMMAND = ADC_STCONV_bm;
}
#endif

//=================================================================================================
/**
 * \brief Start a differential conversion on ADC
 *
 * \param[in] channel,channel1 The ADC channels to start conversion on
 *
 * \return Nothing
 */
//=================================================================================================
// #ifdef ADC_SET							// ADC 사용 설정
// void ADC_start_diff_conversion(adc_0_channel_t channel, adc_0_muxneg_channel_t channel1)
// {
// 	ADC0.CTRLA |= ADC_CONVMODE_bm;
// 	ADC0.MUXPOS  = channel;
// 	ADC0.MUXNEG  = channel1;
// 	ADC0.COMMAND = ADC_STCONV_bm;
// }
// #endif

//=================================================================================================
/**
 * \brief Stop a conversion on ADC
 *
 * \return Nothing
 */
//=================================================================================================
// #ifdef ADC_SET							// ADC 사용 설정
// void ADC_stop_conversion()
// {
// 	ADC0.COMMAND = ADC_SPCONV_bm;
// }
// #endif

//=================================================================================================
/**
   * \brief Check if the ADC conversion is done
   *
   * \return The status of ADC converison done check
   * \retval true The ADC conversion is done
   * \retval false The ADC converison is not done
   */
//=================================================================================================
#ifdef ADC_SET								// ADC 사용 설정
bool ADC_is_conversion_done()
{
	return (ADC0.INTFLAGS & ADC_RESRDY_bm);
}
#endif

//=================================================================================================
/**
 * \brief Read a conversion result from ADC
 *
 * \return Conversion result read from the ADC ADC module
 */
//=================================================================================================
#ifdef ADC_SET								// ADC 사용 설정
adc_result_t ADC_get_conversion_result(void)
{
	return (ADC0.RES);
}
#endif

//=================================================================================================
/**
 * \brief Start a conversion, wait until ready, and return the conversion result
 *
 * \return Conversion result read from the ADC ADC module
 */
//=================================================================================================
#ifdef ADC_SET								// ADC 사용 설정
adc_result_t ADC_get_conversion(adc_channel_t channel)
{
	adc_result_t res;

	ADC_start_conversion(channel);
	while (!ADC_is_conversion_done());
	res = ADC_get_conversion_result();
	ADC0.INTFLAGS |= ADC_RESRDY_bm;
	return res;
}
#endif

//=================================================================================================
/**
 * \brief Start a differential conversion, wait until ready, and return the conversion result
 *
 * \return Conversion result read from the ADC ADC module
 */
//=================================================================================================
// #ifdef ADC_SET							// ADC 사용 설정
// diff_adc_result_t ADC_get_diff_conversion(adc_0_channel_t channel, adc_0_muxneg_channel_t channel1)
// {
// 	diff_adc_result_t res;
// 
// 	ADC_start_diff_conversion(channel, channel1);
// 	while (!ADC_is_conversion_done())
// 		;
// 	res = ADC_get_conversion_result();
// 	ADC0.INTFLAGS |= ADC_RESRDY_bm;
// 	return res;
// }
// #endif

//=================================================================================================
/**
   * \brief Return the number of bits in the ADC conversion result
   *
   * \return The number of bits in the ADC conversion result
   */
//=================================================================================================
// #ifdef ADC_SET							// ADC 사용 설정
// uint8_t ADC_get_resolution()
// {
// 	return (ADC0.CTRLA & ADC_RESSEL0_bm) ? 10 : 12;
// }
// #endif

//=================================================================================================
#ifdef ADC_SET								// ADC 사용 설정
void VREF_ADC_init(void)
{
	VREF.ADC0REF = VREF_REFSEL_VDD_gc;				// VDD as reference
	_delay_us(50);
}
#endif

//=================================================================================================
