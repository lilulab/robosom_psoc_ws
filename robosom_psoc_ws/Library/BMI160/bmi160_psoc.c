/**
 * @file bmi160_psoc.c
 * @author Lu Li (lilulab@github)
 * @brief PSOC HAL for BMI160 IMU
 * @version 0.1
 * @date 2020-02-20
 * 
 * @copyright Copyright (c) 2020
 * 
 */

// PSOC related header files
#include "project.h"
#include "stdio.h"

// BMI160 official driver header file
#include "bmi160.h"

/* type definitions */
typedef int8_t (*bmi160_com_fptr_t)(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
typedef void (*bmi160_delay_fptr_t)(uint32_t period); 