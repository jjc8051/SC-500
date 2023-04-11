/**
 * @file eeprom.c
 * @author LJY (ejok9194@gmail.com)
 * @brief AVRXXXDX Series eeprom read, write and update fucntion definition.
 * @version 1.0
 * @date 2022-01-25
 *
 * @copyright Copyright (c) 2022
 *
 */
//=================================================================================================
#include "eeprom.h"
#include <ccp.h> // AVR128dxxx Register 쓰기 함수 정의 헤더

//=================================================================================================
/**
 * @brief eeprom 읽기 함수 (1byte)
 *
 * @param eeprom_adr 읽을 주소값 입력 (0~511)
 * @return uint8_t 읽은 데이터 리턴
 */
//=================================================================================================
uint8_t ee_read_byte(uint16_t eeprom_adr)
{
	// 최대 주소
	if (eeprom_adr > 511) {
		eeprom_adr = 511;
	}
	/* Read operation will be stalled by hardware if any write is in progress
	 */
	return *(uint8_t*)(EEPROM_START + eeprom_adr);
}

//=================================================================================================
/**
 * @brief eeprom 읽기 함수 (2byte)
 *
 * @param[in] eeprom_adr 읽을 주소값 입력 (0~510)
 * @return uint16_t 읽은 데이터 리턴
 */
//=================================================================================================
uint16_t ee_read_word(uint16_t eeprom_adr)
{
	// 최대 주소
	if (eeprom_adr > 511 - 1) {
		eeprom_adr = 511 - 1;
	}
	/* Read operation will be stalled by hardware if any write is in progress
	 */
	return *(uint16_t*)(EEPROM_START + eeprom_adr);
}

//=================================================================================================
/**
 * @brief eeprom 읽기 함수 (4byte)
 *
 * @param[in] eeprom_adr 읽을 주소값 입력 (0~508)
 * @return uint32_t 읽은 데이터 리턴
 */
//=================================================================================================
uint32_t ee_read_dword(uint16_t eeprom_adr)
{
	// 최대 주소
	if (eeprom_adr > 511 - 3) {
		eeprom_adr = 511 - 3;
	}
	/* Read operation will be stalled by hardware if any write is in progress
	 */
	return *(uint32_t*)(EEPROM_START + eeprom_adr);
}

//=================================================================================================
/**
 * @brief eeprom 쓰기함수 (1byte)
 *
 * @param[in] eeprom_adr 주소 입력 (0~511)
 * @param[in] data 데이터입력 (1byte)
 * @return uint8_t 성공하면 1 리턴, 실패하면 0 리턴
 */
//=================================================================================================
uint8_t ee_write_byte(uint16_t eeprom_adr, uint8_t data)
{
	if (eeprom_adr > 511) {
		eeprom_adr = 511;
		
		return 0;
	}
	/* Wait for completion of previous operation */
	while (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm))
		;
	/* Program the EEPROM with desired value(s) */
	ccp_write_spm((void*)&NVMCTRL.CTRLA, NVMCTRL_CMD_EEERWR_gc);
	/* Write byte to EEPROM */
	*(uint8_t*)(EEPROM_START + eeprom_adr) = data;
	/* Clear the current command */
	ccp_write_spm((void*)&NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);
	return 1;
}

//=================================================================================================
/**
 * @brief eeprom 쓰기함수 (2byte)
 *
 * @param[in] eeprom_adr 주소 입력 (0~510)
 * @param[in] data 데이터입력 (2byte)
 * @return uint8_t 성공하면 1 리턴, 실패하면 0 리턴
 */
//=================================================================================================
uint8_t ee_write_word(uint16_t eeprom_adr, uint16_t data)
{
	DATA_MERGE_T mergeData;

	if (eeprom_adr > 511 - 1) {
		eeprom_adr = 511 - 1;
		
		return 0;
	}
	mergeData.u32d0 = (uint32_t)data;
	ee_write_byte(eeprom_adr + 0, mergeData.u8d0);
	ee_write_byte(eeprom_adr + 1, mergeData.u8d1);

	return 1;
}

//=================================================================================================
/**
 * @brief eeprom 쓰기함수(4byte)
 *
 * @param[in] eeprom_adr 주소 입력 (0~508)
 * @param[in] data 데이터입력 (4byte)
 * @return uint8_t 성공하면 1 리턴, 실패하면 0 리턴
 */
//=================================================================================================
uint8_t ee_write_dword(uint16_t eeprom_adr, uint32_t data)
{
	DATA_MERGE_T mergeData;
	if (eeprom_adr > 511 - 3) {
		eeprom_adr = 511 - 3;
		
		return 0;
	}

	mergeData.u32d0 = (uint32_t)data;
	ee_write_byte(eeprom_adr + 0, mergeData.u8d0);
	ee_write_byte(eeprom_adr + 1, mergeData.u8d1);
	ee_write_byte(eeprom_adr + 2, mergeData.u8d2);
	ee_write_byte(eeprom_adr + 3, mergeData.u8d3);

	return 1;
}

//=================================================================================================
/**
 * @brief eeprom 갱신 함수 (1byte)
 *
 * @param[in] eeprom_adr 주소 입력 (0~511)
 * @param[in] data 데이터입력 (1byte)
 * @return uint8_t 성공하면 1 리턴, 같은 값이거나 주소범위를 초과하면 쓰지않으면 0 리턴
 */
//=================================================================================================
uint8_t ee_update_byte(uint16_t eeprom_adr, uint8_t data)
{
	DATA_MERGE_T mergeData;
	if (eeprom_adr > 511) {
		eeprom_adr = 511;
		
		return 0;
	}

	mergeData.u32d0 = (uint32_t)data;
	if (ee_read_byte(eeprom_adr) != mergeData.u8d0) {
		ee_write_byte(eeprom_adr, mergeData.u8d0);

		return 1;
	} else {
		// 이전값과 같으므로 쓰기안하고 함수종료
		return 0;
	}
}

//=================================================================================================
/**
 * @brief eeprom 갱신함수 (2byte)
 *
 * @param[in] eeprom_adr 주소 입력 (0~510)
 * @param[in] data 데이터입력 (2byte)
 * @return uint8_t 성공하면 1 리턴, 같은 값이거나 주소범위를 초과하면 쓰지않으면 0 리턴
 */
//=================================================================================================
uint8_t ee_update_word(uint16_t eeprom_adr, uint16_t data)
{
	DATA_MERGE_T mergeData;

	if (eeprom_adr > 511 - 1) {
		eeprom_adr = 511 - 1;
		
		return 0;
	}

	mergeData.u32d0 = (uint32_t)data;
	if (ee_read_word(eeprom_adr) != mergeData.u16d0) {
		ee_write_byte(eeprom_adr + 0, mergeData.u8d0);
		ee_write_byte(eeprom_adr + 1, mergeData.u8d1);

		return 1;
	} else {

		return 0;
	}
}

//=================================================================================================
/**
 * @brief eeprom 갱신함수(4byte)
 *
 * @param[in] eeprom_adr 주소 입력 (0~508)
 * @param[in] data 데이터입력 (4byte)
 * @return uint8_t 성공하면 1 리턴, 같은 값이거나 주소범위를 초과하면 쓰지않으면 0 리턴
 */
//=================================================================================================
uint8_t ee_update_dword(uint16_t eeprom_adr, uint32_t data)
{
	DATA_MERGE_T mergeData;
	if (eeprom_adr > 511 - 3) {
		eeprom_adr = 511 - 3;
		return 0;
	}

	mergeData.u32d0 = (uint32_t)data;
	if (ee_read_dword(eeprom_adr) != mergeData.u32d0) {
		ee_write_byte(eeprom_adr + 0, mergeData.u8d0);
		ee_write_byte(eeprom_adr + 1, mergeData.u8d1);
		ee_write_byte(eeprom_adr + 2, mergeData.u8d2);
		ee_write_byte(eeprom_adr + 3, mergeData.u8d3);

		return 1;
	} else {

		return 0;
	}
}

//=================================================================================================
