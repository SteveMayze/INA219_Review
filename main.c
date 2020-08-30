/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"
#include "util/delay.h"
#include <stdio.h>
#include <string.h>
#include "INA219.h"
#include "IS31FL3637.h"

/*
    Main application
*/

static uint8_t rx[USART0_RX_BUFFER_SIZE];


uint8_t readString(bool block){
    uint8_t i = 0;
    memset(rx, 0, sizeof(USART0_RX_BUFFER_SIZE));
    if (USART0_IsRxReady()||block) {
        for (i = 0; i < USART0_RX_BUFFER_SIZE; i++) {
            rx[i] = USART0_Read(); // Blocks until character is available
            if( rx[i] == '\n' || rx[i] == '\r' ) {
                rx[i] = 0x00;
                break;
            }
        }
    }
    return i;
}

#define _INA219_CHECK


#ifdef _INA219_CHECK

#define INA219_ADDRESS INA219_ADDR_GND_GND
#define DISPLAY_ADDR IS31FL3637_Addr7_GND_GND

int main(void)
{
    /* Initialises MCU, drivers and middle-ware */
    SYSTEM_Initialize();

    INA219_Initialise(INA219_ADDRESS);
    IS31FL3637_Initialise(DISPLAY_ADDR);

    struct ina219_data reading;
    char sbuf[80];
    while (1){
        reading = INA219_getReadings();
        
        printf("READINGS\n");
        sprintf(sbuf, "  Bus Voltage: %f mV\n", reading.bus_voltage);
        printf(sbuf);
        sprintf(sbuf, "Shunt Voltage: %f V\n", reading.shunt_voltage);
        printf(sbuf);
        sprintf(sbuf, "      Current: %f A\n", reading.current);
        printf(sbuf);
        sprintf(sbuf, "        Power: %f W\n", reading.power);
        printf(sbuf);
        sprintf(sbuf, "         Test: %f \n", 25.3);
        printf(sbuf);
        LED_Toggle();
        _delay_ms(10000);
    }
}
#endif

#ifdef _IS31FL3637_CHECK
#include "IS31FL3637.h"

#define DISPLAY_ADDR IS31FL3637_Addr7_GND_GND
int main(void)
{
    /* Initialises MCU, drivers and middle-ware */
    SYSTEM_Initialize();

    IS31FL3637_Initialise(DISPLAY_ADDR);
    printf("> ");
   // sei();
    while (1){
        uint8_t read = readString(false);
        if( read > 0){
            printf("%s - %d\n", rx, read);
            if( strncmp("on", (char*)rx, strlen("on")) != 0) {
                LED_SetHigh();
            } 
            if( strncmp("off", (char*)rx, strlen("off")) != 0) {
                LED_SetLow();
            } 
            printf("> ");
        }
        LED_Toggle();
        _delay_ms(1000);

        //set_sleep_mode(SLEEP_MODE_STANDBY);
        //sleep_mode();

    }
}
#endif

/**
    End of File
*/