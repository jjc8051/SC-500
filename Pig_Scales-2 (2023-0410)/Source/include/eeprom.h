/**
 * @file eeprom.h
 * @author LJY (ejok9194@gmail.com)
 * @brief AVRxxxDx Series eeprom 함수 펌웨어 헤더
 * @version 1.0
 * @date 2022-01-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include <ccp.h> // AVR128dxxx Register 쓰기 함수 정의 헤더
#include <stdint.h>

//=================================================================================================
//	structure
//=================================================================================================
typedef struct DATA_MERGE_T
{
	union {
		struct
		{
			uint8_t u8d0;
			uint8_t u8d1;
			uint8_t u8d2;
			uint8_t u8d3;
		};

		struct
		{
			uint16_t u16d0;
			uint16_t u16d1;
		};

		struct
		{
			uint32_t u32d0;
		};
	};
} DATA_MERGE_T;

//=================================================================================================
//	Function Definition
//=================================================================================================
uint8_t	 ee_read_byte(uint16_t eeprom_adr);
uint16_t ee_read_word(uint16_t eeprom_adr);
uint32_t ee_read_dword(uint16_t eeprom_adr);
uint8_t	 ee_write_byte(uint16_t eeprom_adr, uint8_t data);
uint8_t	 ee_write_word(uint16_t eeprom_adr, uint16_t data);
uint8_t	 ee_write_dword(uint16_t eeprom_adr, uint32_t data);
uint8_t	 ee_update_byte(uint16_t eeprom_adr, uint8_t data);
uint8_t	 ee_update_word(uint16_t eeprom_adr, uint16_t data);
uint8_t	 ee_update_dword(uint16_t eeprom_adr, uint32_t data);

//=================================================================================================
#endif /* EEPROM_H_ */
