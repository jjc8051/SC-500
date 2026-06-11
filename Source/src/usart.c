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

/**
 * \defgroup doc_driver_usart_init USART Init
 * \ingroup doc_driver_usart
 *
 * \section doc_driver_usart_rev Revision History
 * - v0.0.0.1 Initial Commit
 *
 *@{
 */

//=================================================================================================
#include <main.h>
#include <clock_config.h>
#include <usart.h>
#include <string.h>

//=================================================================================================
#ifdef Usart_0
int8_t USART_0_init()
{
	USART0.BAUD = (uint16_t)USART0_BAUD_RATE(Usart0_BAUD);		/* set baud rate register */

	USART0.CTRLA = 1 << USART_ABEIE_bp				/* Auto-baud Error Interrupt Enable: disabled */
			| 0 << USART_DREIE_bp				/* Data Register Empty Interrupt Enable: disabled */
			| 0 << USART_LBME_bp				/* Loop-back Mode Enable: disabled */
			| USART_RS485_DISABLE_gc			/* RS485 Mode disabled */
			| 1 << USART_RXCIE_bp				/* Receive Complete Interrupt Enable: enabled */
			| 1 << USART_RXSIE_bp				/* Receiver Start Frame Interrupt Enable: enabled */
			| 0 << USART_TXCIE_bp;				/* Transmit Complete Interrupt Enable: disabled */

	USART0.CTRLB = 0 << USART_MPCM_bp				/* Multi-processor Communication Mode: disabled */
			| 0 << USART_ODME_bp				/* Open Drain Mode Enable: disabled */
			| 1 << USART_RXEN_bp				/* Receiver Enable: enabled */
			| USART_RXMODE_NORMAL_gc			/* Normal mode */
			| 0 << USART_SFDEN_bp				/* Start Frame Detection Enable: disabled */
			| 1 << USART_TXEN_bp;				/* Transmitter Enable: enabled */

//	USART0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc			/* Asynchronous Mode */
//			| USART_CHSIZE_8BIT_gc				/* Character size: 8 bit */
//			| USART_PMODE_DISABLED_gc			/* No Parity */
//			| USART_SBMODE_1BIT_gc;				/* 1 stop bit */

//	USART0.DBGCTRL = 0 << USART_DBGRUN_bp;				/* Debug Run: disabled */
//	USART0.EVCTRL = 0 << USART_IREI_bp;				/* IrDA Event Input Enable: disabled */
//	USART0.RXPLCTRL = 0x0 << USART_RXPL_gp;				/* Receiver Pulse Lenght: 0x0 */
//	USART0.TXPLCTRL = 0x0 << USART_TXPL_gp;				/* Transmit pulse length: 0x0 */

	return 0;
}

//-------------------------------------------------------------------------------------------------
int USART0_Write(char data)
{
	while(!(USART0.STATUS & USART_DREIF_bm));
	USART0.TXDATAL = data;
	return 1;
}

//-------------------------------------------------------------------------------------------------
void USART0_SendString(const char *str)
{
	for(size_t i = 0; i < strlen(str); i++) {USART0_Write(str[i]);}
}

//-------------------------------------------------------------------------------------------------
void Rx0_Buff_Clear(void)						// Rx0 Buffer Clear
{
	memset(Rx0_Buff, 0, RX0_SIZE);					// Rx0_Buff 를 Rx0_SIZE(250)만큼 0으로 초기화
	Rx0_Cnt = 0;							// Rx0 Data Count 초기화
}
#endif

//=================================================================================================
#ifdef Usart_1
int8_t USART_1_init()
{
	USART1.BAUD = (uint16_t)USART1_BAUD_RATE(Usart1_BAUD);		/* set baud rate register */

	USART1.CTRLA = 1 << USART_ABEIE_bp				/* Auto-baud Error Interrupt Enable: disabled */
			| 0 << USART_DREIE_bp				/* Data Register Empty Interrupt Enable: disabled */
			| 0 << USART_LBME_bp				/* Loop-back Mode Enable: disabled */
			| USART_RS485_DISABLE_gc			/* RS485 Mode disabled */
			| 1 << USART_RXCIE_bp				/* Receive Complete Interrupt Enable: enabled */
			| 1 << USART_RXSIE_bp				/* Receiver Start Frame Interrupt Enable: enabled */
			| 0 << USART_TXCIE_bp;				/* Transmit Complete Interrupt Enable: disabled */

	USART1.CTRLB = 0 << USART_MPCM_bp				/* Multi-processor Communication Mode: disabled */
			| 0 << USART_ODME_bp				/* Open Drain Mode Enable: disabled */
			| 1 << USART_RXEN_bp				/* Receiver Enable: enabled */
			| USART_RXMODE_NORMAL_gc			/* Normal mode */
			| 0 << USART_SFDEN_bp				/* Start Frame Detection Enable: disabled */
			| 1 << USART_TXEN_bp;				/* Transmitter Enable: enabled */

//	USART1.CTRLC = USART_CMODE_ASYNCHRONOUS_gc			/* Asynchronous Mode */
//			| USART_CHSIZE_8BIT_gc				/* Character size: 8 bit */
//			| USART_PMODE_DISABLED_gc			/* No Parity */
//			| USART_SBMODE_1BIT_gc;				/* 1 stop bit */

//	USART1.DBGCTRL = 0 << USART_DBGRUN_bp;				/* Debug Run: disabled */
//	USART1.EVCTRL = 0 << USART_IREI_bp;				/* IrDA Event Input Enable: disabled */
//	USART1.RXPLCTRL = 0x0 << USART_RXPL_gp;				/* Receiver Pulse Lenght: 0x0 */
//	USART1.TXPLCTRL = 0x0 << USART_TXPL_gp;				/* Transmit pulse length: 0x0 */

	return 0;
}

//-------------------------------------------------------------------------------------------------
int USART1_Write(char data)
{
	while(!(USART1.STATUS & USART_DREIF_bm));
	USART1.TXDATAL = data;
	return 1;
}

//-------------------------------------------------------------------------------------------------
void USART1_SendString(const char *str)
{
	for(size_t i = 0; i < strlen(str); i++) {USART1_Write(str[i]);}
}

//-------------------------------------------------------------------------------------------------
void Rx1_Buff_Clear(void)						// Rx1 Buffer Clear
{
	memset(Rx1_Buff, 0, RX1_SIZE);					// Rx1_Buff 를 Rx1_SIZE(250)만큼 0으로 초기화
	Rx1_Cnt = 0;							// Rx1 Data Count 초기화
}

#endif

//=================================================================================================
#ifdef Usart_2
int8_t USART_2_init()
{
	USART2.BAUD = (uint16_t)USART2_BAUD_RATE(Usart2_BAUD);		/* set baud rate register */

	USART2.CTRLA = 1 << USART_ABEIE_bp				/* Auto-baud Error Interrupt Enable: disabled */
			| 0 << USART_DREIE_bp				/* Data Register Empty Interrupt Enable: disabled */
			| 0 << USART_LBME_bp				/* Loop-back Mode Enable: disabled */
			| USART_RS485_DISABLE_gc			/* RS485 Mode disabled */
			| 1 << USART_RXCIE_bp				/* Receive Complete Interrupt Enable: enabled */
			| 1 << USART_RXSIE_bp				/* Receiver Start Frame Interrupt Enable: enabled */
			| 0 << USART_TXCIE_bp;				/* Transmit Complete Interrupt Enable: disabled */

	USART2.CTRLB = 0 << USART_MPCM_bp				/* Multi-processor Communication Mode: disabled */
			| 0 << USART_ODME_bp				/* Open Drain Mode Enable: disabled */
			| 1 << USART_RXEN_bp				/* Receiver Enable: enabled */
			| USART_RXMODE_NORMAL_gc			/* Normal mode */
			| 0 << USART_SFDEN_bp				/* Start Frame Detection Enable: disabled */
			| 1 << USART_TXEN_bp;				/* Transmitter Enable: enabled */

//	USART2.CTRLC = USART_CMODE_ASYNCHRONOUS_gc			/* Asynchronous Mode */
//			| USART_CHSIZE_8BIT_gc				/* Character size: 8 bit */
//			| USART_PMODE_DISABLED_gc			/* No Parity */
//			| USART_SBMODE_1BIT_gc;				/* 1 stop bit */

//	USART2.DBGCTRL = 0 << USART_DBGRUN_bp;				/* Debug Run: disabled */
//	USART2.EVCTRL = 0 << USART_IREI_bp;				/* IrDA Event Input Enable: disabled */
//	USART2.RXPLCTRL = 0x0 << USART_RXPL_gp;				/* Receiver Pulse Lenght: 0x0 */
//	USART2.TXPLCTRL = 0x0 << USART_TXPL_gp;				/* Transmit pulse length: 0x0 */

	return 0;
}

//-------------------------------------------------------------------------------------------------
int USART2_Write(char data)
{
	while(!(USART2.STATUS & USART_DREIF_bm));
	USART2.TXDATAL = data;
	return 1;
}

//-------------------------------------------------------------------------------------------------
void USART2_SendString(const char *str)
{
	for(size_t i = 0; i < strlen(str); i++) {USART2_Write(str[i]);}
}

//-------------------------------------------------------------------------------------------------
void Rx2_Buff_Clear(void)						// Rx2 Buffer Clear
{
	memset(Rx2_Buff, 0, RX2_SIZE);					// Rx2_Buff 를 Rx2_SIZE(250)만큼 0으로 초기화
	Rx2_Cnt = 0;							// Rx2 Data Count 초기화
}

#endif

//=================================================================================================
#ifdef Usart_3
int8_t USART_3_init()
{
	USART3.BAUD = (uint16_t)USART3_BAUD_RATE(Usart3_BAUD);		/* set baud rate register */

	USART3.CTRLA = 1 << USART_ABEIE_bp				/* Auto-baud Error Interrupt Enable: disabled */
			| 0 << USART_DREIE_bp				/* Data Register Empty Interrupt Enable: disabled */
			| 0 << USART_LBME_bp				/* Loop-back Mode Enable: disabled */
			| USART_RS485_DISABLE_gc			/* RS485 Mode disabled */
			| 1 << USART_RXCIE_bp				/* Receive Complete Interrupt Enable: enabled */
			| 1 << USART_RXSIE_bp				/* Receiver Start Frame Interrupt Enable: enabled */
			| 0 << USART_TXCIE_bp;				/* Transmit Complete Interrupt Enable: disabled */

	USART3.CTRLB = 0 << USART_MPCM_bp				/* Multi-processor Communication Mode: disabled */
			| 0 << USART_ODME_bp				/* Open Drain Mode Enable: disabled */
			| 1 << USART_RXEN_bp				/* Receiver Enable: enabled */
			| USART_RXMODE_NORMAL_gc			/* Normal mode */
			| 0 << USART_SFDEN_bp				/* Start Frame Detection Enable: disabled */
			| 1 << USART_TXEN_bp;				/* Transmitter Enable: enabled */

//	USART3.CTRLC = USART_CMODE_ASYNCHRONOUS_gc			/* Asynchronous Mode */
//			| USART_CHSIZE_8BIT_gc				/* Character size: 8 bit */
//			| USART_PMODE_DISABLED_gc			/* No Parity */
//			| USART_SBMODE_1BIT_gc;				/* 1 stop bit */

//	USART3.DBGCTRL = 0 << USART_DBGRUN_bp;				/* Debug Run: disabled */
//	USART3.EVCTRL = 0 << USART_IREI_bp;				/* IrDA Event Input Enable: disabled */
//	USART3.RXPLCTRL = 0x0 << USART_RXPL_gp;				/* Receiver Pulse Lenght: 0x0 */
//	USART3.TXPLCTRL = 0x0 << USART_TXPL_gp;				/* Transmit pulse length: 0x0 */

	return 0;
}

//-------------------------------------------------------------------------------------------------
int USART3_Write(char data)
{
	while(!(USART3.STATUS & USART_DREIF_bm)) ;
	USART3.TXDATAL = data;
	return 1;
}

//-------------------------------------------------------------------------------------------------
void USART3_SendString(const char *str)
{
	for(size_t i = 0; i < strlen(str); i++) {USART3_Write(str[i]);}
}

//-------------------------------------------------------------------------------------------------
void Rx3_Buff_Clear(void)						// Rx3 Buffer Clear
{
	memset(Rx3_Buff, 0, RX3_SIZE);					// Rx3_Buff 를 Rx3_SIZE(250)만큼 0으로 초기화
	Rx3_Cnt = 0;							// Rx3 Data Count 초기화
}

#endif

//=================================================================================================
#ifdef Usart_4
int8_t USART_4_init()
{
	USART4.BAUD = (uint16_t)USART4_BAUD_RATE(Usart4_BAUD);		/* set baud rate register */

	USART4.CTRLA = 1 << USART_ABEIE_bp				/* Auto-baud Error Interrupt Enable: disabled */
			| 0 << USART_DREIE_bp				/* Data Register Empty Interrupt Enable: disabled */
			| 0 << USART_LBME_bp				/* Loop-back Mode Enable: disabled */
			| USART_RS485_DISABLE_gc			/* RS485 Mode disabled */
			| 1 << USART_RXCIE_bp				/* Receive Complete Interrupt Enable: enabled */
			| 1 << USART_RXSIE_bp				/* Receiver Start Frame Interrupt Enable: enabled */
			| 0 << USART_TXCIE_bp;				/* Transmit Complete Interrupt Enable: disabled */

	USART4.CTRLB = 0 << USART_MPCM_bp				/* Multi-processor Communication Mode: disabled */
			| 0 << USART_ODME_bp				/* Open Drain Mode Enable: disabled */
			| 1 << USART_RXEN_bp				/* Receiver Enable: enabled */
			| USART_RXMODE_NORMAL_gc			/* Normal mode */
			| 0 << USART_SFDEN_bp				/* Start Frame Detection Enable: disabled */
			| 1 << USART_TXEN_bp;				/* Transmitter Enable: enabled */

//	USART4.CTRLC = USART_CMODE_ASYNCHRONOUS_gc			/* Asynchronous Mode */
//			| USART_CHSIZE_8BIT_gc				/* Character size: 8 bit */
//			| USART_PMODE_DISABLED_gc			/* No Parity */
//			| USART_SBMODE_1BIT_gc;				/* 1 stop bit */

//	USART4.DBGCTRL = 0 << USART_DBGRUN_bp;				/* Debug Run: disabled */
//	USART4.EVCTRL = 0 << USART_IREI_bp;				/* IrDA Event Input Enable: disabled */
//	USART4.RXPLCTRL = 0x0 << USART_RXPL_gp;				/* Receiver Pulse Lenght: 0x0 */
//	USART4.TXPLCTRL = 0x0 << USART_TXPL_gp;				/* Transmit pulse length: 0x0 */

	return 0;
}

//-------------------------------------------------------------------------------------------------
int USART4_Write(char data)
{
	while(!(USART4.STATUS & USART_DREIF_bm));
	USART4.TXDATAL = data;
	return 1;
}

//-------------------------------------------------------------------------------------------------
void USART4_SendString(const char *str)
{
	for(size_t i = 0; i < strlen(str); i++) {USART4_Write(str[i]);}
}

//-------------------------------------------------------------------------------------------------
void Rx4_Buff_Clear(void)						// Rx4 Buffer Clear
{
	memset(Rx4_Buff, 0, RX4_SIZE);					// Rx4_Buff 를 Rx4_SIZE(250)만큼 0으로 초기화
	Rx4_Cnt = 0;							// Rx4 Data Count 초기화
}

#endif

//=================================================================================================
#ifdef Usart_5
int8_t USART_5_init()
{
	USART5.BAUD = (uint16_t)USART5_BAUD_RATE(Usart5_BAUD);		/* set baud rate register */

	USART5.CTRLA = 1 << USART_ABEIE_bp				/* Auto-baud Error Interrupt Enable: disabled */
			| 0 << USART_DREIE_bp				/* Data Register Empty Interrupt Enable: disabled */
			| 0 << USART_LBME_bp				/* Loop-back Mode Enable: disabled */
			| USART_RS485_DISABLE_gc			/* RS485 Mode disabled */
			| 1 << USART_RXCIE_bp				/* Receive Complete Interrupt Enable: enabled */
			| 1 << USART_RXSIE_bp				/* Receiver Start Frame Interrupt Enable: enabled */
			| 0 << USART_TXCIE_bp;				/* Transmit Complete Interrupt Enable: disabled */

	USART5.CTRLB = 0 << USART_MPCM_bp				/* Multi-processor Communication Mode: disabled */
			| 0 << USART_ODME_bp				/* Open Drain Mode Enable: disabled */
			| 1 << USART_RXEN_bp				/* Receiver Enable: enabled */
			| USART_RXMODE_NORMAL_gc			/* Normal mode */
			| 0 << USART_SFDEN_bp				/* Start Frame Detection Enable: disabled */
			| 1 << USART_TXEN_bp;				/* Transmitter Enable: enabled */

//	USART5.CTRLC = USART_CMODE_ASYNCHRONOUS_gc			/* Asynchronous Mode */
//			| USART_CHSIZE_8BIT_gc				/* Character size: 8 bit */
//			| USART_PMODE_DISABLED_gc			/* No Parity */
//			| USART_SBMODE_1BIT_gc;				/* 1 stop bit */

//	USART5.DBGCTRL = 0 << USART_DBGRUN_bp;				/* Debug Run: disabled */
//	USART5.EVCTRL = 0 << USART_IREI_bp;				/* IrDA Event Input Enable: disabled */
//	USART5.RXPLCTRL = 0x0 << USART_RXPL_gp;				/* Receiver Pulse Lenght: 0x0 */
//	USART5.TXPLCTRL = 0x0 << USART_TXPL_gp;				/* Transmit pulse length: 0x0 */

	return 0;
}

//-------------------------------------------------------------------------------------------------
int USART5_Write(char data)
{
	while(!(USART5.STATUS & USART_DREIF_bm));
	USART5.TXDATAL = data;
	return 1;
}

//-------------------------------------------------------------------------------------------------
void USART5_SendString(const char *str)
{
	for(size_t i = 0; i < strlen(str); i++) {USART5_Write(str[i]);}
}

//-------------------------------------------------------------------------------------------------
void Rx5_Buff_Clear(void)						// Rx5 Buffer Clear
{
	memset(Rx5_Buff, 0, RX5_SIZE);					// Rx5_Buff 를 Rx5_SIZE(250)만큼 0으로 초기화
	Rx5_Cnt = 0;							// Rx5 Data Count 초기화
}

#endif

//=================================================================================================
