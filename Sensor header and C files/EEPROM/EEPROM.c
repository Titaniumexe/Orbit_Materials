/*
 * EEPROM.c
 *
 *  Created on: Sep 7, 2025
 *      Author: aarus
 */
#include "EEPROM.h"
#include <string.h>
#define EEPROM_I2C &hi2c1
#define EEPROM_ADDR 0xA0
#define PAGE_SIZE 256

extern I2C_HandleTypeDef hi2c1;

HAL_StatusTypeDef EEPROM_Write(uint16_t page, uint16_t offset, uint8_t *data, uint16_t size) {
    int paddrposition = 8; // log2(PAGE_SIZE)
    uint16_t startPage = page;
    uint16_t endPage = page + ((size + offset) / PAGE_SIZE);
    uint16_t numofpages = (endPage - startPage) + 1;
    uint16_t pos = 0;

    for(int i = 0; i < numofpages; i++) {
        uint16_t MemAddress = (startPage << paddrposition) | offset;
        uint16_t bytesremaining = (size > (PAGE_SIZE - offset)) ? (PAGE_SIZE - offset) : size;
        HAL_StatusTypeDef res = HAL_I2C_Mem_Write(EEPROM_I2C, EEPROM_ADDR, MemAddress, 2, &data[pos], bytesremaining, 1000);
        if(res != HAL_OK) return res;
        startPage += 1;
        offset = 0; size -= bytesremaining; pos += bytesremaining;
        HAL_Delay(5); // write cycle time in ms
    }
    return HAL_OK;
}

HAL_StatusTypeDef EEPROM_Read(uint16_t page, uint16_t offset, uint8_t *data, uint16_t size) {
    int paddrposition = 8;
    uint16_t startPage = page;
    uint16_t endPage = page + ((size + offset) / PAGE_SIZE);
    uint16_t numofpages = (endPage - startPage) + 1;
    uint16_t pos = 0;

    for(int i = 0; i < numofpages; i++) {
        uint16_t MemAddress = (startPage << paddrposition) | offset;
        uint16_t bytesremaining = (size > (PAGE_SIZE - offset)) ? (PAGE_SIZE - offset) : size;
        HAL_StatusTypeDef res = HAL_I2C_Mem_Read(EEPROM_I2C, EEPROM_ADDR, MemAddress, 2, &data[pos], bytesremaining, 1000);
        if(res != HAL_OK) return res;
        startPage += 1; offset = 0; size -= bytesremaining; pos += bytesremaining;
    }
    return HAL_OK;
}

HAL_StatusTypeDef EEPROM_WriteFloat(uint16_t mem_addr, float value) {
    uint8_t buf[10];
    memcpy(buf, &value, 4);
    return HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDR, mem_addr, 2, buf, 4, 1000);
}
HAL_StatusTypeDef EEPROM_ReadFloat(uint16_t mem_addr, float *value) {
    uint8_t buf[10];
    HAL_StatusTypeDef res = HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDR, mem_addr, 2, buf, 4, 1000);
    if (res == HAL_OK) memcpy(value, buf, 4);
    return res;
}
HAL_StatusTypeDef EEPROM_PageErase(uint16_t page) {
    int paddrposition = 8;
    uint16_t MemAddress = (page << paddrposition);
    uint8_t data[PAGE_SIZE];
    memset(data, 0xFF, PAGE_SIZE);
    return HAL_I2C_Mem_Write(EEPROM_I2C, EEPROM_ADDR, MemAddress, 2, data, PAGE_SIZE, 1000);
}



