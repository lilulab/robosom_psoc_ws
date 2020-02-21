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

#include "bmi160_psoc.h"

/* BMI160 bmi160_dev struct type definitions */
// typedef int8_t (*bmi160_com_fptr_t)(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
// typedef void (*bmi160_delay_fptr_t)(uint32_t period); 

/**
 * @brief PSOC I2C Read HAL Function
 * 
 * @param dev_addr 
 * @param reg_addr 
 * @param data 
 * @param len 
 * @return int8_t 
 */
int8_t bmi160_psoc_i2c_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    
    int status;
    int i;

    /* Write control byte without stop */
    status = I2C_1_MasterSendStart(dev_addr, I2C_1_WRITE_XFER_MODE);

    if(status == I2C_1_MSTR_NO_ERROR) /* Check if transfer completed without errors */
    // if(1) /* Cheating */
    {
        /* Write control byte */
        status = I2C_1_MasterWriteByte(reg_addr);
    }

    /* Read data array */
    status = I2C_1_MasterSendRestart(dev_addr, I2C_1_READ_XFER_MODE);

    if(I2C_1_MSTR_NO_ERROR == status) /* Check if transfer completed without errors */
    // if(1) /* Cheating */
    {
        /* Read array of 5 bytes */
        for(i=0; i<len; i++)
        {
            if(i < len-1)
            {
                data[i] = I2C_1_MasterReadByte(I2C_1_ACK_DATA); /* non-last byte send ACK */
            }
            else
            {
                data[i] = I2C_1_MasterReadByte(I2C_1_NAK_DATA); /* last byte send NACK */
            }
        }
    }

    I2C_1_MasterSendStop(); /* Send Stop */

    return status;
    // return 0; /* Cheating */
}

/**
 * @brief PSOC I2C Write HAL Function
 * 
 * @param dev_addr 
 * @param reg_addr 
 * @param data 
 * @param len 
 * @return int8_t 
 */
int8_t bmi160_psoc_i2c_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    int status;
    int i;

    status = I2C_1_MasterSendStart(dev_addr, I2C_1_WRITE_XFER_MODE);

    if(status == I2C_1_MSTR_NO_ERROR) /* Check if transfer completed without errors */
    // if(1) /* Cheating */
    {
        /* Send reg_addr as the first byte */
        status = I2C_1_MasterWriteByte(reg_addr);

        /* Only if no error, then proceed */
        if(status == I2C_1_MSTR_NO_ERROR)
        // if(1) /* Cheating */
        {
            /* Send data array of len bytes */
            for(i=0; i<len; i++)
            {
                /* send data array */
                status = I2C_1_MasterWriteByte(data[i]);

                /* if has error then break the loop */
                if(status != I2C_1_MSTR_NO_ERROR) 
                // if(0) /* Cheating */
                {
                    break;
                }
            }
        }

    }
    I2C_1_MasterSendStop(); /* Send Stop */

    return status;
    // return 0; /* Cheating */
}


/**
 * @brief PSOC HAL for the ms delay function
 * 
 * @param period 
 */
void bmi160_psoc_delay_ms(uint32_t period)
{
    // typedef void (*bmi160_delay_fptr_t)(uint32_t period); 
    CyDelay(period);
}