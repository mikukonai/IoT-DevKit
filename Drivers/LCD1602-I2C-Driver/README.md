# LCD1602-IIC-Driver

1602LCD的I2C转接模块的Linux用户态驱动程序。本程序适用于Linux操作系统。已经在Raspberry Pi（Raspbian）、Intel Galileo Gen1（Yocto Linux）、NanoPi 2 Fire（Debian）等单板机上测试通过。

## 兼容性说明

请根据实际情况，修改`i2c1602.h`中，关于I2C设备文件和I2C从设备地址的宏定义。

    #define I2C_DEVFILE "/dev/i2c-1"
    #define I2C_ADDR 0x3f

以及，根据所使用平台不同（例如使用STM32、8051等平台），请修改`i2c1602.c`中，函数`i2c1602_expanderWrite()`的具体实现。

## 原作者提供的信息

//YWROBOT
//last updated on 21/12/2011
//Tim Starling Fix the reset bug (Thanks Tim)
//wiki doc http://www.dfrobot.com/wiki/index.php?title=I2C/TWI_LCD1602_Module_(SKU:_DFR0063)
//Support Forum: http://www.dfrobot.com/forum/
//Compatible with the Arduino IDE 1.0
//Library version:1.1
