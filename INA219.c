
#include "INA219.h"
#include "TWI0_Util.h"


static volatile uint8_t iic_address;
uint8_t ina219_buf[3];

void INA219_Initialise(uint8_t addr) {
    iic_address = addr;
    TWI0_write2ByteRegister(iic_address, INA219_CFG, INA219_DEFAULT_CFG);
    TWI0_write2ByteRegister(iic_address, INA219_CAL, 0x1000);
}

uint16_t get_shunt_voltage_raw() {
    return TWI0_read2ByteRegister(iic_address, INA219_SHUNT_VOLTAGE);
}

uint16_t get_bus_voltage_raw() {
    uint16_t reading =  TWI0_read2ByteRegister(iic_address, INA219_BUS_VOLTAGE);
    return (int16_t)((reading >> 3) * 4);
}

uint16_t get_current_raw() {
    TWI0_write2ByteRegister(iic_address, INA219_CAL, 0x1000);
    return TWI0_read2ByteRegister(iic_address, INA219_CURRENT);
   
}

uint16_t get_power_raw() {
    TWI0_write2ByteRegister(iic_address, INA219_CAL, 0x1000);
    return TWI0_read2ByteRegister(iic_address, INA219_POWER);
   
}

struct ina219_data INA219_getReadings() {
    struct ina219_data readings;

    uint16_t reading;

    reading = get_shunt_voltage_raw();
    readings.shunt_voltage = reading * 0.01;

    reading = get_bus_voltage_raw();
    readings.bus_voltage = reading * 0.001;

    reading = get_current_raw();
    readings.current = (float)(reading / 10) ;
    
    reading = get_power_raw();
    readings.power = (float)(reading * 2);

    return readings;
}

