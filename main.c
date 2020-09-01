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

uint8_t readString(bool block) {
    uint8_t i = 0;
    memset(rx, 0, sizeof (USART0_RX_BUFFER_SIZE));
    if (USART0_IsRxReady() || block) {
        for (i = 0; i < USART0_RX_BUFFER_SIZE; i++) {
            rx[i] = USART0_Read(); // Blocks until character is available
            if (rx[i] == '\n' || rx[i] == '\r') {
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
#define DISPLAY_A_ADDR IS31FL3637_Addr7_GND_GND
#define DISPLAY_B_ADDR IS31FL3637_Addr7_GND_SCL

void splitFloat(int *result, float value) {
    result[0] = trunc(value);
    result[1] = trunc((value - result[0]) * 1000);
}

int main(void) {
    /* Initialises MCU, drivers and middle-ware */
    SYSTEM_Initialize();

    INA219_Initialise(INA219_ADDRESS);
    // IS31FL3637_Initialise_for_rainbow(DISPLAY_A_ADDR);
    IS31FL3637_Initialise(DISPLAY_B_ADDR);

    struct ina219_data readings;
    int values[2];

    while (1) {

        readings = INA219_getReadings();

        printf("READINGS\n");
        // printf("Bus Voltage\tShunt Voltage\tCurrent mA\tPower mW\n");
        splitFloat(values, readings.bus_voltage);
        printf("  Bus Voltage: %d.%03d V\n", values[0], values[1]);
        splitFloat(values, readings.shunt_voltage);
        printf("Shunt Voltage: %d.%03d mV\n", values[0], values[1]);
        splitFloat(values, readings.current);
        printf("      Current: %d.%03d mA\n", values[0], values[1]);
        splitFloat(values, readings.power);
        printf("        Power: %d.%03d mW\n", values[0], values[1]);
        LED_Toggle();

        uint8_t red, green, blue;
        uint8_t level = 0;
        if (readings.power < 600) { // Blue
            red = 0x00;
            green = 0x00;
            blue = 0xff;
            level = 0;
        } else if (readings.power < 1200) { // Azure
            red = 0x00;
            green = 0x7F;
            blue = 0xfF;
            level = 0;
        } else if (readings.power < 1800) { // Cyan 
            red = 0x00;
            green = 0xff;
            blue = 0xff;
            level = 1;
        } else if (readings.power < 2400) { // Aquamarine
            red = 0x00;
            green = 0xff;
            blue = 0x7f;
            level = 2;
        } else if (readings.power < 3000) { // Green
            red = 0x00;
            green = 0xff;
            blue = 0x00;
            level = 3;
        } else if (readings.power < 3600) { // Chartreuse
            red = 0x7f;
            green = 0xff;
            blue = 0x00;
            level = 4;
        } else if (readings.power < 4200) { // Yellow
            red = 0xff;
            green = 0xff;
            blue = 0x00;
            level = 5;
        } else if (readings.power < 4800) { // Orange
            red = 0xff;
            green = 0x3f;
            blue = 0x00;
            level = 6;
        } else { // Red
            red = 0xff;
            green = 0x00;
            blue = 0x00;
            level = 7;
        }
        for (uint8_t row = 0; row < 4; row++) {
            for (uint8_t column = 0; column < 8; column++) {
                if (column <= level) {
                    IS31FL3637_set_LED(DISPLAY_B_ADDR, column, row, red, green, blue);
                } else {
                    IS31FL3637_set_LED(DISPLAY_B_ADDR, column, row, 0x00, 0x00, 0x00);
                }
            }
        }

        IS31FL3637_update_display(DISPLAY_B_ADDR);
        _delay_ms(1000);
    }
}
#endif

#ifdef _IS31FL3637_CHECK
#include "IS31FL3637.h"

#define DISPLAY_ADDR IS31FL3637_Addr7_GND_GND

int main(void) {
    /* Initialises MCU, drivers and middle-ware */
    SYSTEM_Initialize();

    IS31FL3637_Initialise(DISPLAY_ADDR);
    printf("> ");
    // sei();
    while (1) {
        uint8_t read = readString(false);
        if (read > 0) {
            printf("%s - %d\n", rx, read);
            if (strncmp("on", (char*) rx, strlen("on")) != 0) {
                LED_SetHigh();
            }
            if (strncmp("off", (char*) rx, strlen("off")) != 0) {
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