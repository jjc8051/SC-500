/**
 * \file
 *
 * \brief USART init driver.
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

//=================================================================================================
#ifndef USART_H_INCLUDED
#define USART_H_INCLUDED

#include <stdint.h>
#include <compiler.h>
#include <main.h>

#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------------------------------
#ifdef Usart_0
/* Normal Mode, Baud register value */
	#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)
	int8_t USART_0_init();
	int USART0_Write(char data);
	void USART0_SendString(const char *str);
	void Rx0_Buff_Clear(void);
#endif

//-------------------------------------------------------------------------------------------------
#ifdef Usart_1
/* Normal Mode, Baud register value */
	#define USART1_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)
	int8_t USART_1_init();
	int USART1_Write(char data);
	void USART1_SendString(const char *str);
	void Rx1_Buff_Clear(void);
#endif

//-------------------------------------------------------------------------------------------------
#ifdef Usart_2
/* Normal Mode, Baud register value */
	#define USART2_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)
	int8_t USART_2_init();
	int USART2_Write(char data);
	void USART2_SendString(const char *str);
	void Rx2_Buff_Clear(void);
#endif

//-------------------------------------------------------------------------------------------------
#ifdef Usart_3
/* Normal Mode, Baud register value */
	#define USART3_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)
	int8_t USART_3_init();
	int USART3_Write(char data);
	void USART3_SendString(const char *str);
	void Rx3_Buff_Clear(void);
#endif

//-------------------------------------------------------------------------------------------------
#ifdef Usart_4
/* Normal Mode, Baud register value */
	#define USART4_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)
	int8_t USART_4_init();
	int USART4_Write(char data);
	void USART4_SendString(const char *str);
	void Rx4_Buff_Clear(void);
#endif

//-------------------------------------------------------------------------------------------------
#ifdef Usart_5
/* Normal Mode, Baud register value */
	#define USART5_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)
	int8_t USART_5_init();
	int USART5_Write(char data);
	void USART5_SendString(const char *str);
	void Rx5_Buff_Clear(void);
#endif

//-------------------------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif

#endif /* USART_H_INCLUDED */