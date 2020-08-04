/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "stdio.h"

// Include BMI160 Library and PSOC HAL
#include "../Library/BMI160/bmi160.h"
#include "../Library/BMI160/bmi160_psoc.h"

struct bmi160_dev sensor;
struct bmi160_sensor_data accel;
struct bmi160_sensor_data gyro;
uint16_t step_count = 0;//stores the step counter value


int8_t imu_bmi160_init(void);
int8_t imu_bmi160_config(void);
int8_t imu_bmi160_enable_step_counter(void);
int8_t imu_bmi160_read_acc_gyo(void);
int8_t imu_bmi160_read_steps(void);

// USBUART
#define USBFS_DEVICE    (0u)
#define USBUART_BUFFER_SIZE (256u)
uint16 count;
uint8 buffer[USBUART_BUFFER_SIZE];
void USBUART_user_check_init(void);
void USBUART_user_echo(void);

// Testing Function
void print_imu_via_usbuart(void);

// System clock
uint32 sys_clock_cur_ms = 0;
float sys_clock_cur_us_in_ms = 0;
void sys_clock_ms_callback(void); // 1ms callback interrupt function


int main(void)
{
    uint8_t led_test = 0;
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    // USBUART Init
    USBUART_Start(USBFS_DEVICE, USBUART_5V_OPERATION);
    USBUART_CDC_Init();
    
    // I2C Init
    I2C_1_Start();
    
    // IMU BMI160 Init
    imu_bmi160_init();
    imu_bmi160_config();
    imu_bmi160_enable_step_counter();
    
    // PWM Block Init
    PWM_LED_Start();
   
    /* Turn off LEDs */
    Led_Red_Write(0);
    Led_Green_Write(0);
    Led_Blue_Write(0);
    Led_Key_Write(1);
    
    CyDelay(100);
    
    // Start system 1ms tick
    CySysTickStart();
    CySysTickSetCallback(0, sys_clock_ms_callback);
    CySysTickEnableInterrupt();
       
    for(;;)
    {

        imu_bmi160_read_acc_gyo();
        imu_bmi160_read_steps();
        USBUART_user_check_init();
        //USBUART_user_echo();
        print_imu_via_usbuart();
        
        led_test++;
        
        Led_Red_Write((led_test >> 0) & 0x01);
        Led_Green_Write((led_test >> 1) & 0x01);
        Led_Blue_Write((led_test >> 2) & 0x01);   

        if (Led_Key_Read() == 0)
        {
            led_test = 0;
        }
          
        
        CyDelay(10);       
    }
}

int8_t imu_bmi160_init(void)
{
    /* IMU init */
    sensor.id = BMI160_I2C_ADDR;
    sensor.interface = BMI160_I2C_INTF;
    sensor.read = bmi160_psoc_i2c_read;
    sensor.write = bmi160_psoc_i2c_write;
    sensor.delay_ms = bmi160_psoc_delay_ms;
    
    int8_t rslt = BMI160_OK;
    rslt = bmi160_soft_reset(&sensor);
    
    rslt = BMI160_OK;
    rslt = bmi160_init(&sensor);
    /*
    // After the above function call, 
    // accel and gyro parameters in the device structure 
    // are set with default values, 
    // found in the datasheet of the sensor
    */

   return rslt;
}

int8_t imu_bmi160_config(void)
{
    int8_t rslt = BMI160_OK;

    /* Select the Output data rate, range of accelerometer sensor */
    // sensor.accel_cfg.odr = BMI160_ACCEL_ODR_1600HZ;
    sensor.accel_cfg.odr = BMI160_ACCEL_ODR_800HZ;
    sensor.accel_cfg.range = BMI160_ACCEL_RANGE_4G;
    sensor.accel_cfg.bw = BMI160_ACCEL_BW_NORMAL_AVG4;
    // sensor.accel_cfg.bw = BMI160_ACCEL_BW_RES_AVG8;
    #define IMU_ACC_SCALE 4

    /* Select the power mode of accelerometer sensor */
    sensor.accel_cfg.power = BMI160_ACCEL_NORMAL_MODE;

    /* Select the Output data rate, range of Gyroscope sensor */
    sensor.gyro_cfg.odr = BMI160_GYRO_ODR_3200HZ;
    //sensor.gyro_cfg.range = BMI160_GYRO_RANGE_1000_DPS;
    sensor.gyro_cfg.range = BMI160_GYRO_RANGE_500_DPS;
    sensor.gyro_cfg.bw = BMI160_GYRO_BW_NORMAL_MODE;
    // sensor.gyro_cfg.bw = BMI160_GYRO_BW_OSR2_MODE;
    #define IMU_GYO_SCALE 500

    /* Select the power mode of Gyroscope sensor */
    sensor.gyro_cfg.power = BMI160_GYRO_NORMAL_MODE; 

    /* Set the sensor configuration */
    rslt = bmi160_set_sens_conf(&sensor);    

    return rslt;
}

int8_t imu_bmi160_enable_step_counter(void)
{
    int8_t rslt = BMI160_OK;
    uint8_t step_enable = 1;//enable the step counter

    rslt = bmi160_set_step_counter(step_enable,  &sensor);

    return rslt;

}

int8_t imu_bmi160_read_steps(void)
{
    int8_t rslt = BMI160_OK;
    rslt = bmi160_read_step_counter(&step_count,  &sensor);
    return rslt;
}

int8_t imu_bmi160_read_acc_gyo(void)
{
    int8_t rslt = BMI160_OK;

    /* To read only Accel data */
    //rslt = bmi160_get_sensor_data(BMI160_ACCEL_SEL, &accel, NULL, &sensor);

    /* To read only Gyro data */
    //rslt = bmi160_get_sensor_data(BMI160_GYRO_SEL, NULL, &gyro, &sensor);

    /* To read both Accel and Gyro data */
    //bmi160_get_sensor_data((BMI160_ACCEL_SEL | BMI160_GYRO_SEL), &accel, &gyro, &sensor);

    /* To read Accel data along with time */
    //rslt = bmi160_get_sensor_data((BMI160_ACCEL_SEL | BMI160_TIME_SEL) , &accel, NULL, &sensor);

    /* To read Gyro data along with time */
    //rslt = bmi160_get_sensor_data((BMI160_GYRO_SEL | BMI160_TIME_SEL), NULL, &gyro, &sensor);

    /* To read both Accel and Gyro data along with time*/
    bmi160_get_sensor_data((BMI160_ACCEL_SEL | BMI160_GYRO_SEL | BMI160_TIME_SEL), &accel, &gyro, &sensor);

    return rslt;
}

/// USBUART Routin
void USBUART_user_check_init(void) {
    /* Host can send double SET_INTERFACE request. */
    if (0u != USBUART_IsConfigurationChanged())
    {
        /* Initialize IN endpoints when device is configured. */
        if (0u != USBUART_GetConfiguration())
        {
            /* Enumeration is done, enable OUT endpoint to receive data 
             * from host. */
            USBUART_CDC_Init();
        }
    }
}


void USBUART_user_echo(void) {
    /* Service USB CDC when device is configured. */
    if (0u != USBUART_GetConfiguration())
    {
        /* Check for input data from host. */
        if (0u != USBUART_DataIsReady())
        {
            /* Read received data and re-enable OUT endpoint. */
            count = USBUART_GetAll(buffer);

            if (0u != count)
            {
                /* Wait until component is ready to send data to host. */
                while (0u == USBUART_CDCIsReady())
                {
                }

                /* Send data back to host. */
                USBUART_PutData(buffer, count);

                /* If the last sent packet is exactly the maximum packet 
                *  size, it is followed by a zero-length packet to assure
                *  that the end of the segment is properly identified by 
                *  the terminal.
                */
                if (USBUART_BUFFER_SIZE == count)
                {
                    /* Wait until component is ready to send data to PC. */
                    while (0u == USBUART_CDCIsReady())
                    {
                    }

                    /* Send zero-length packet to PC. */
                    USBUART_PutData(NULL, 0u);
                }
            }
        }
    }
}

void print_imu_via_usbuart(void)
{   
    //int32_t gyro_offset = 50000;
    sys_clock_cur_us_in_ms = (float)CySysTickGetValue() * (1/(float)cydelay_freq_hz);

    while (0u == USBUART_CDCIsReady())
    {
    }

    // sprintf((char *)buffer, "%d\t%d\t%d\t%d\t%ld\t%ld\t%ld\r\n", step_count, accel.x, accel.y, accel.z, (long)(gyro.x + gyro_offset), (long)(gyro.y + gyro_offset), (long)(gyro.z + gyro_offset));
    sprintf((char *)buffer, "%lu\t%d\t%d\t%d\t%d\t%d\t%d\r\n", sys_clock_cur_ms, accel.x, accel.y, accel.z, gyro.x, gyro.y, gyro.z);
    //sprintf((char *)buffer, "%f\t%f\t%f\t%f\t%f\t%f\t%f\r\n", (float)sys_clock_cur_ms/1000 + sys_clock_cur_us_in_ms, 
    //                            (float)accel.x/32768*IMU_ACC_SCALE*9.80665, (float)accel.y/32768*IMU_ACC_SCALE*9.80665,(float)accel.z/32768*IMU_ACC_SCALE*9.80665, 
    //                            (float)gyro.x/32768*IMU_GYO_SCALE, (float)gyro.y/32768*IMU_GYO_SCALE, (float)gyro.z/32768*IMU_GYO_SCALE);


    //count = sizeof(buffer);
    /* Send data back to host. */
    //USBUART_PutData(buffer, count);
    USBUART_PutString((char8 *)buffer);
}

// 1ms system tick callback interrupt function
void sys_clock_ms_callback(void){
    sys_clock_cur_ms ++; // increment ms counter by 1
}

/* [] END OF FILE */
