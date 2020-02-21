

#ifndef BMI160_PSOC_H_
#define BMI160_PSOC_H_

// PSOC related header files
#include "project.h"
#include "stdio.h"

// BMI160 official driver header file
#include "bmi160.h"

#endif /* BMI160_PSOC_H_ */

/**
 * @brief PSOC I2C Read HAL Function
 * 
 * @param dev_addr 
 * @param reg_addr 
 * @param data 
 * @param len 
 * @return int8_t 
 */
int8_t bmi160_psoc_i2c_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);

/**
 * @brief  PSOC I2C Write HAL Function
 * 
 * @param dev_addr 
 * @param reg_addr 
 * @param data 
 * @param len 
 * @return int8_t 
 */
int8_t bmi160_psoc_i2c_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);

/**
 * @brief PSOC HAL for the ms delay function
 * 
 * @param period 
 */
void bmi160_psoc_delay_ms(uint32_t period);