/***********************************************
 * BME280环境传感器 API（C语言）
 * - Mikukonai @ GitHub
 * - 改写自SparkFun提供的Arduino版本
 * - 适用于Raspberry、Intel Edison/Galileo等Linux开发板
 * - 2017.6.13
 * - 需要根据系统实际情况进行修改的地方：
 * -- 宏 I2C_DEVFILE 修改为实际的I2C设备文件路径
 ***********************************************/

/******************************************************************************
SparkFunBME280.h
BME280 Arduino and Teensy Driver
Marshall Taylor @ SparkFun Electronics
May 20, 2015
https://github.com/sparkfun/BME280_Breakout

Resources:
Uses Wire.h for i2c operation
Uses SPI.h for SPI operation

Development environment specifics:
Arduino IDE 1.6.4
Teensy loader 1.23

This code is released under the [MIT License](http://opensource.org/licenses/MIT).
Please review the LICENSE.md file included with this example. If you have any questions 
or concerns with licensing, please contact techsupport@sparkfun.com.
Distributed as-is; no warranty is given.
******************************************************************************/

#ifndef __BME280_H__
#define __BME280_H__

#include "stdint.h"
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <stdint.h> 
#include <sys/ioctl.h> 
#include <linux/i2c.h> 

//Register names:
#define BME280_DIG_T1_LSB_REG			0x88
#define BME280_DIG_T1_MSB_REG			0x89
#define BME280_DIG_T2_LSB_REG			0x8A
#define BME280_DIG_T2_MSB_REG			0x8B
#define BME280_DIG_T3_LSB_REG			0x8C
#define BME280_DIG_T3_MSB_REG			0x8D
#define BME280_DIG_P1_LSB_REG			0x8E
#define BME280_DIG_P1_MSB_REG			0x8F
#define BME280_DIG_P2_LSB_REG			0x90
#define BME280_DIG_P2_MSB_REG			0x91
#define BME280_DIG_P3_LSB_REG			0x92
#define BME280_DIG_P3_MSB_REG			0x93
#define BME280_DIG_P4_LSB_REG			0x94
#define BME280_DIG_P4_MSB_REG			0x95
#define BME280_DIG_P5_LSB_REG			0x96
#define BME280_DIG_P5_MSB_REG			0x97
#define BME280_DIG_P6_LSB_REG			0x98
#define BME280_DIG_P6_MSB_REG			0x99
#define BME280_DIG_P7_LSB_REG			0x9A
#define BME280_DIG_P7_MSB_REG			0x9B
#define BME280_DIG_P8_LSB_REG			0x9C
#define BME280_DIG_P8_MSB_REG			0x9D
#define BME280_DIG_P9_LSB_REG			0x9E
#define BME280_DIG_P9_MSB_REG			0x9F
#define BME280_DIG_H1_REG				0xA1
#define BME280_CHIP_ID_REG				0xD0 //Chip ID
#define BME280_RST_REG					0xE0 //Softreset Reg
#define BME280_DIG_H2_LSB_REG			0xE1
#define BME280_DIG_H2_MSB_REG			0xE2
#define BME280_DIG_H3_REG				0xE3
#define BME280_DIG_H4_MSB_REG			0xE4
#define BME280_DIG_H4_LSB_REG			0xE5
#define BME280_DIG_H5_MSB_REG			0xE6
#define BME280_DIG_H6_REG				0xE7
#define BME280_CTRL_HUMIDITY_REG		0xF2 //Ctrl Humidity Reg
#define BME280_STAT_REG					0xF3 //Status Reg
#define BME280_CTRL_MEAS_REG			0xF4 //Ctrl Measure Reg
#define BME280_CONFIG_REG				0xF5 //Configuration Reg
#define BME280_PRESSURE_MSB_REG			0xF7 //Pressure MSB
#define BME280_PRESSURE_LSB_REG			0xF8 //Pressure LSB
#define BME280_PRESSURE_XLSB_REG		0xF9 //Pressure XLSB
#define BME280_TEMPERATURE_MSB_REG		0xFA //Temperature MSB
#define BME280_TEMPERATURE_LSB_REG		0xFB //Temperature LSB
#define BME280_TEMPERATURE_XLSB_REG		0xFC //Temperature XLSB
#define BME280_HUMIDITY_MSB_REG			0xFD //Humidity MSB
#define BME280_HUMIDITY_LSB_REG			0xFE //Humidity LSB

#define I2C_DEVFILE						"/dev/i2c-0"

//Class SensorSettings.
//This object is used to hold settings data.  The application
//uses this classes' data directly.  The settings are adopted and sent to the sensor
//at special times, such as .begin.  Some are used for doing math.
//
//This is a kind of bloated way to do this.  The trade-off is that the user doesn't
//need to deal with #defines or enums with bizarre names.
//
//A power user would strip out SensorSettings entirely, and send specific read and
//write command directly to the IC. (ST #defines below)
//
typedef struct
{
	//Main Interface and mode settings
	uint8_t commInterface;
	uint8_t I2CAddress;
	int i2c_fd;
	
	uint8_t runMode;
	uint8_t tStandby;
	uint8_t filter;
	uint8_t tempOverSample;
	uint8_t pressOverSample;
	uint8_t humidOverSample;
} SensorSettings;

//Used to hold the calibration constants.  These are used
//by the driver as measurements are being taking
typedef struct
{
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;
	
	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;
	
	uint8_t dig_H1;
	int16_t dig_H2;
	uint8_t dig_H3;
	int16_t dig_H4;
	int16_t dig_H5;
	uint8_t dig_H6;
} SensorCalibration;

SensorSettings* settings;
SensorCalibration* calibration;
int32_t t_fine;

uint8_t bme280_init( void );
void bme280_reset( void );

float bme280_read_pressure( void );
float bme280_read_alt_meters( void );
float bme280_read_humidity( void );
float bme280_read_temp( void );

//utilities
void readRegisterRegion(uint8_t*, uint8_t, uint8_t );
uint8_t readRegister(uint8_t);
int16_t readRegisterInt16( uint8_t offset );
void writeRegister(uint8_t, uint8_t);

uint8_t bme280_init()
{
	uint8_t dataToWrite = 0;  //Temporary variable

	if( (settings->i2c_fd = open(I2C_DEVFILE, O_RDWR)) < 0 )
	{
		printf("BME280 Error : open i2c device file\n");
	}

	if(ioctl(settings->i2c_fd, I2C_SLAVE, settings->I2CAddress) < 0) {
		printf("BME280 Error : ioctl\n");
		return 0;
	}

	//Reading all compensation data, range 0x88:A1, 0xE1:E7
	
	calibration->dig_T1 = ((uint16_t)((readRegister(BME280_DIG_T1_MSB_REG) << 8) + readRegister(BME280_DIG_T1_LSB_REG)));
	calibration->dig_T2 = ((int16_t)((readRegister(BME280_DIG_T2_MSB_REG) << 8) + readRegister(BME280_DIG_T2_LSB_REG)));
	calibration->dig_T3 = ((int16_t)((readRegister(BME280_DIG_T3_MSB_REG) << 8) + readRegister(BME280_DIG_T3_LSB_REG)));

	calibration->dig_P1 = ((uint16_t)((readRegister(BME280_DIG_P1_MSB_REG) << 8) + readRegister(BME280_DIG_P1_LSB_REG)));
	calibration->dig_P2 = ((int16_t)((readRegister(BME280_DIG_P2_MSB_REG) << 8) + readRegister(BME280_DIG_P2_LSB_REG)));
	calibration->dig_P3 = ((int16_t)((readRegister(BME280_DIG_P3_MSB_REG) << 8) + readRegister(BME280_DIG_P3_LSB_REG)));
	calibration->dig_P4 = ((int16_t)((readRegister(BME280_DIG_P4_MSB_REG) << 8) + readRegister(BME280_DIG_P4_LSB_REG)));
	calibration->dig_P5 = ((int16_t)((readRegister(BME280_DIG_P5_MSB_REG) << 8) + readRegister(BME280_DIG_P5_LSB_REG)));
	calibration->dig_P6 = ((int16_t)((readRegister(BME280_DIG_P6_MSB_REG) << 8) + readRegister(BME280_DIG_P6_LSB_REG)));
	calibration->dig_P7 = ((int16_t)((readRegister(BME280_DIG_P7_MSB_REG) << 8) + readRegister(BME280_DIG_P7_LSB_REG)));
	calibration->dig_P8 = ((int16_t)((readRegister(BME280_DIG_P8_MSB_REG) << 8) + readRegister(BME280_DIG_P8_LSB_REG)));
	calibration->dig_P9 = ((int16_t)((readRegister(BME280_DIG_P9_MSB_REG) << 8) + readRegister(BME280_DIG_P9_LSB_REG)));

	calibration->dig_H1 = ((uint8_t)(readRegister(BME280_DIG_H1_REG)));
	calibration->dig_H2 = ((int16_t)((readRegister(BME280_DIG_H2_MSB_REG) << 8) + readRegister(BME280_DIG_H2_LSB_REG)));
	calibration->dig_H3 = ((uint8_t)(readRegister(BME280_DIG_H3_REG)));
	calibration->dig_H4 = ((int16_t)((readRegister(BME280_DIG_H4_MSB_REG) << 4) + (readRegister(BME280_DIG_H4_LSB_REG) & 0x0F)));
	calibration->dig_H5 = ((int16_t)((readRegister(BME280_DIG_H5_MSB_REG) << 4) + ((readRegister(BME280_DIG_H4_LSB_REG) >> 4) & 0x0F)));
	calibration->dig_H6 = ((uint8_t)readRegister(BME280_DIG_H6_REG));

	//Set the oversampling control words.
	//config will only be writeable in sleep mode, so first insure that.
	writeRegister(BME280_CTRL_MEAS_REG, 0x00);
	
	//Set the config word
	dataToWrite = (settings->tStandby << 0x5) & 0xE0;
	dataToWrite |= (settings->filter << 0x02) & 0x1C;
	writeRegister(BME280_CONFIG_REG, dataToWrite);
	
	//Set ctrl_hum first, then ctrl_meas to activate ctrl_hum
	dataToWrite = settings->humidOverSample & 0x07; //all other bits can be ignored
	writeRegister(BME280_CTRL_HUMIDITY_REG, dataToWrite);
	
	//set ctrl_meas
	//First, set temp oversampling
	dataToWrite = (settings->tempOverSample << 0x5) & 0xE0;
	//Next, pressure oversampling
	dataToWrite |= (settings->pressOverSample << 0x02) & 0x1C;
	//Last, set mode
	dataToWrite |= (settings->runMode) & 0x03;
	//Load the byte
	writeRegister(BME280_CTRL_MEAS_REG, dataToWrite);
	
	return readRegister(0xD0);
}

void bme280_reset( void )
{
	writeRegister(BME280_RST_REG, 0xB6);
	
}

float bme280_read_pressure( void )
{

	// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
	// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
	int32_t adc_P = ((uint32_t)readRegister(BME280_PRESSURE_MSB_REG) << 12) | ((uint32_t)readRegister(BME280_PRESSURE_LSB_REG) << 4) | ((readRegister(BME280_PRESSURE_XLSB_REG) >> 4) & 0x0F);
	
	int64_t var1, var2, p_acc;
	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)calibration->dig_P6;
	var2 = var2 + ((var1 * (int64_t)calibration->dig_P5)<<17);
	var2 = var2 + (((int64_t)calibration->dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)calibration->dig_P3)>>8) + ((var1 * (int64_t)calibration->dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)calibration->dig_P1)>>33;
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p_acc = 1048576 - adc_P;
	p_acc = (((p_acc<<31) - var2)*3125)/var1;
	var1 = (((int64_t)calibration->dig_P9) * (p_acc>>13) * (p_acc>>13)) >> 25;
	var2 = (((int64_t)calibration->dig_P8) * p_acc) >> 19;
	p_acc = ((p_acc + var1 + var2) >> 8) + (((int64_t)calibration->dig_P7)<<4);
	
	return (float)p_acc / 256.0;
}

float bme280_read_alt_meters( void )
{
	float heightOutput = 0;
	
	heightOutput = ((float)-45846.2)*(pow(((float)bme280_read_pressure()/(float)101325), 0.190263) - (float)1);
	return heightOutput;
}

float bme280_read_humidity( void )
{
	
	// Returns humidity in %RH as unsigned 32 bit integer in Q22. 10 format (22 integer and 10 fractional bits).
	// Output value of “47445” represents 47445/1024 = 46. 333 %RH
	int32_t adc_H = ((uint32_t)readRegister(BME280_HUMIDITY_MSB_REG) << 8) | ((uint32_t)readRegister(BME280_HUMIDITY_LSB_REG));
	
	int32_t var1;
	var1 = (t_fine - ((int32_t)76800));
	var1 = (((((adc_H << 14) - (((int32_t)calibration->dig_H4) << 20) - (((int32_t)calibration->dig_H5) * var1)) +
	((int32_t)16384)) >> 15) * (((((((var1 * ((int32_t)calibration->dig_H6)) >> 10) * (((var1 * ((int32_t)calibration->dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
	((int32_t)calibration->dig_H2) + 8192) >> 14));
	var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t)calibration->dig_H1)) >> 4));
	var1 = (var1 < 0 ? 0 : var1);
	var1 = (var1 > 419430400 ? 419430400 : var1);

	return (float)(var1>>12) / 1024.0;
}

float bme280_read_temp( void )
{
	// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
	// t_fine carries fine temperature as global value

	//get the reading (adc_T);
	int32_t adc_T = ((uint32_t)readRegister(BME280_TEMPERATURE_MSB_REG) << 12) | ((uint32_t)readRegister(BME280_TEMPERATURE_LSB_REG) << 4) | ((readRegister(BME280_TEMPERATURE_XLSB_REG) >> 4) & 0x0F);

	//By datasheet, calibrate
	int64_t var1, var2;

	var1 = ((((adc_T>>3) - ((int32_t)calibration->dig_T1<<1))) * ((int32_t)calibration->dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((int32_t)calibration->dig_T1)) * ((adc_T>>4) - ((int32_t)calibration->dig_T1))) >> 12) *
	((int32_t)calibration->dig_T3)) >> 14;
	t_fine = var1 + var2;
	float output = (t_fine * 5 + 128) >> 8;

	output = output / 100;
	
	return output;
}

void readRegisterRegion(uint8_t *outputPointer , uint8_t offset, uint8_t length)
{
	uint8_t err = 0;
	uint8_t r_addr[1] = {offset};
	if( (err = write(settings->i2c_fd, r_addr, 1)) != 1) { 
		printf("Failed to reset the read address : %d\n", settings->I2CAddress); 
		return; 
	} 
	if( (err = read(settings->i2c_fd, outputPointer, length)) != length){ 
		printf("Failed to read in the buffer\n"); 
		return; 
	}
}

uint8_t readRegister(uint8_t offset)
{
	//Return value
	uint8_t result;
	uint8_t err = 0;
	uint8_t r_addr[1] = {offset};
	if( (err = write(settings->i2c_fd, r_addr, 1)) != 1) { 
		printf("Failed to reset the read address : %d\n", settings->I2CAddress); 
		return 1; 
	}
	uint8_t arr[1] = {result};
	if( (err = read(settings->i2c_fd, arr, 1)) != 1){ 
		printf("Failed to read in the buffer\n"); 
		return 1; 
	}
	return arr[0];
}

int16_t readRegisterInt16( uint8_t offset )
{
	uint8_t myBuffer[2];
	readRegisterRegion(myBuffer, offset, 2);  //Does memory transfer
	int16_t output = (int16_t)myBuffer[0] | (int16_t)(myBuffer[1] << 8);
	
	return output;
}

void writeRegister(uint8_t offset, uint8_t dataToWrite)
{
	uint8_t err = 0;
	uint8_t addr_and_data[2] = {offset, dataToWrite};
	if( (err = write(settings->i2c_fd, addr_and_data, 2)) != 2) { 
		printf("Failed to reset the read address : %d\n", settings->I2CAddress); 
		return; 
	}
}

#endif
