/*
 * EEPROM.h
 *
 *  Created on: Sep 7, 2025
 *      Author: aarus
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include "stm32f4xx_hal.h"

HAL_StatusTypeDef EEPROM_Write(uint16_t page, uint16_t offset, uint8_t *data, uint16_t size);
HAL_StatusTypeDef EEPROM_Read(uint16_t page, uint16_t offset, uint8_t *data, uint16_t size);
HAL_StatusTypeDef EEPROM_PageErase(uint16_t page);

#endif /* CORE_SRC_EEPROM_H_ */

