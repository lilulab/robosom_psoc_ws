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

/* ========================================
 *
 * ADC Part 1/3 - Params
 *
 * ========================================
*/
int16 adc_val_int16[4] = {0, 0, 0, 0};
float32 adc_mv_f32[4] = {0, 0, 0, 0};
float32 adc_mv_mean[4] = {0, 0, 0, 0};

/* ========================================
 * ADC Part 1/3 Ends here
 * ========================================
*/



// USBUART
#define USBFS_DEVICE    (0u)
#define USBUART_BUFFER_SIZE (256u)
uint16 count;
uint8 buffer[USBUART_BUFFER_SIZE];
void USBUART_user_check_init(void);
void USBUART_user_echo(void);
void print_adc_via_usbuart(void);

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
    
    /* ========================================
     *
     * ADC Part 2/3 - Init Sequence
     *
     * ========================================
    */
    
    /* Start the ADC components */
    ADC_SAR_Seq_1_Start();
    
    /* Start the ADC conversion */
    ADC_SAR_Seq_1_StartConvert();
    
    /* ========================================
     * ADC Part 2/3 Ends here
     * ========================================
    */
    
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

        USBUART_user_check_init();
        //USBUART_user_echo();
        
        /* Use the GetResult16 API to get an 8 bit unsigned result in
         * single ended mode.  The API CountsTo_mVolts is then used
         * to convert the ADC counts into mV before transmitting via
         * the UART.  See the datasheet API description for more
         * details */
        
        /* ========================================
         *
         * ADC Part 3/3 - Sensor Update 
         *
         * ========================================
        */
            
        // wait for ADC update
        ADC_SAR_Seq_1_IsEndConversion(ADC_SAR_Seq_1_WAIT_FOR_RESULT);
        
        // Get ADC raw values
        for (uint8_t i = 0; i<4; i++)
        {

            // Get bit value
            adc_val_int16[i] = ADC_SAR_Seq_1_GetResult16(i);
            
            // Convert to volt and shift about mean
            adc_mv_f32[i] = ADC_SAR_Seq_1_CountsTo_Volts(adc_val_int16[i]) - adc_mv_mean[i];
            // adc_mv_f32[i] = (i+1) * 1.1; // [DEBUG] test function
            
        }
        
        /* ========================================
         * ADC Part 3/3 Ends here
         * ========================================
        */
        
//        adc_val_int16[0] = ADC_SAR_Seq_1_GetResult16(0u);
//        adc_val_int16[1] = ADC_SAR_Seq_1_GetResult16(1u);
//        adc_val_int16[2] = ADC_SAR_Seq_1_GetResult16(2u);
//        adc_val_int16[3] = ADC_SAR_Seq_1_GetResult16(3u);
//        
//        adc_mv_f32[0] = ADC_SAR_Seq_1_CountsTo_Volts(adc_val_int16[0]) - adc_mv_mean[0];
//        adc_mv_f32[1] = ADC_SAR_Seq_1_CountsTo_Volts(adc_val_int16[1]) - adc_mv_mean[1];
//        adc_mv_f32[2] = ADC_SAR_Seq_1_CountsTo_Volts(adc_val_int16[2]) - adc_mv_mean[2];
//        adc_mv_f32[3] = ADC_SAR_Seq_1_CountsTo_Volts(adc_val_int16[3]) - adc_mv_mean[3];
        

        
        // print result to USBUART
        print_adc_via_usbuart();
                
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

void print_adc_via_usbuart(void)
{   
    //int32_t gyro_offset = 50000;
    sys_clock_cur_us_in_ms = (float)CySysTickGetValue() * (1/(float)cydelay_freq_hz);

    while (0u == USBUART_CDCIsReady())
    {
    }

    // sprintf((char *)buffer, "%f\t%f\r\n", (float)sys_clock_cur_ms/1000 + sys_clock_cur_us_in_ms, adc_mv_f32);
    sprintf((char *)buffer, "0\t%f\t%f\t%f\t%f\r\n", adc_mv_f32[0], adc_mv_f32[1], adc_mv_f32[2], adc_mv_f32[3]);

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
