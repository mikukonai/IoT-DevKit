/***********************************************
 * BME280环境传感器 驱动代码（C语言）
 * - Mikukonai @ GitHub
 * - 改写自SparkFun提供的Arduino版本
 * - 适用于Raspberry、Intel Edison/Galileo等Linux开发板
 * - 2017.6.13
 ***********************************************/

#include <stdlib.h>
#include "bme280.h"

int main(int argc, char** argv) {
	settings = (SensorSettings*)malloc(sizeof(int) * 9);
	calibration = (SensorCalibration*)malloc(sizeof(int) * 18);

	settings->commInterface = 0;
	settings->i2c_fd = 0;
	settings->I2CAddress = 0x76;
	//renMode can be:
	//  0, Sleep mode
	//  1 or 2, Forced mode
	//  3, Normal mode
	settings->runMode = 3; //Normal mode

	//tStandby can be:
	//  0, 0.5ms
	//  1, 62.5ms
	//  2, 125ms
	//  3, 250ms
	//  4, 500ms
	//  5, 1000ms
	//  6, 10ms
	//  7, 20ms
	settings->tStandby = 0;

	//filter can be off or number of FIR coefficients to use:
	//  0, filter off
	//  1, coefficients = 2
	//  2, coefficients = 4
	//  3, coefficients = 8
	//  4, coefficients = 16
	settings->filter = 0;

	//tempOverSample can be:
	//  0, skipped
	//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
	settings->tempOverSample = 1;

	//pressOverSample can be:
	//  0, skipped
	//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
	settings->pressOverSample = 1;

	//humidOverSample can be:
	//  0, skipped
	//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
	settings->humidOverSample = 1;

	printf("BME280 Digital Environment Sensor\n");
	printf("Check: 0x%x\n", bme280_init());
	float temp = bme280_read_temp();
	printf("Temperature: %.2f°C\n", temp);
	float press = bme280_read_pressure() / 100;
	printf("Pressure: %.2fhPa\n", press);
	float humid = bme280_read_humidity();
	printf("Humidity: %.2f%%RH\n", humid);

	return 0;
}
