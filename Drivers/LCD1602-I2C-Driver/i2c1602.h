// LCD1602 I2C 接口模块驱动
// mikukonai - 2017.2
//  适合于Raspberry Pi、Intel Galileo、NanoPi等Linux开发板
//  只测试了i2c1602_printstr(const char *)函数
//    只供本人个人用途使用，本人不对代码负责
//  以下是原作者提供的信息

//YWROBOT
//last updated on 21/12/2011
//Tim Starling Fix the reset bug (Thanks Tim)
//wiki doc http://www.dfrobot.com/wiki/index.php?title=I2C/TWI_LCD1602_Module_(SKU:_DFR0063)
//Support Forum: http://www.dfrobot.com/forum/
//Compatible with the Arduino IDE 1.0
//Library version:1.1

#ifndef __I2C_1602_H__
#define __I2C_1602_H__

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <errno.h>

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x04  // Enable bit
#define Rw 0x02  // Read/Write bit
#define Rs 0x01  // Register select bit

///////////////////////////////////////////////
#define I2C_DEVFILE "/dev/i2c-1"
#define I2C_ADDR 0x3f
///////////////////////////////////////////////
#define delayMicroseconds(x) usleep(x)
#define delay(x) usleep((x)*1000)
///////////////////////////////////////////////

// Global variables
uint8_t _Addr;
uint8_t _displayfunction;
uint8_t _displaycontrol;
uint8_t _displaymode;
uint8_t _numlines;
uint8_t _cols;
uint8_t _rows;
uint8_t _backlightval;

int i2cdev_fd = 0;

void printIIC(uint8_t args);
void i2c1602_write(uint8_t value);
void i2c1602_var_init(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows);
void i2c1602_begin(uint8_t cols, uint8_t rows, uint8_t charsize);
void i2c1602_close();
void i2c1602_clear();
void i2c1602_home();
void i2c1602_noDisplay();
void i2c1602_display();
void i2c1602_noBlink();
void i2c1602_blink();
void i2c1602_noCursor();
void i2c1602_cursor();
void i2c1602_scrollDisplayLeft();
void i2c1602_scrollDisplayRight();
void i2c1602_printLeft();
void i2c1602_printRight();
void i2c1602_leftToRight();
void i2c1602_rightToLeft();
void i2c1602_shiftIncrement();
void i2c1602_shiftDecrement();
void i2c1602_noBacklight();
void i2c1602_backlight();
void i2c1602_autoscroll();
void i2c1602_noAutoscroll(); 
void i2c1602_createChar(uint8_t, uint8_t[]);
void i2c1602_setCursor(uint8_t, uint8_t); 

void i2c1602_command(uint8_t);
void i2c1602_init();

void i2c1602_blink_on();						// alias for blink()
void i2c1602_blink_off();       					// alias for noBlink()
void i2c1602_cursor_on();      	 					// alias for cursor()
void i2c1602_cursor_off();      					// alias for noCursor()
void i2c1602_setBacklight(uint8_t new_val);				// alias for backlight() and nobacklight()
void i2c1602_load_custom_character(uint8_t char_num, uint8_t *rows);	// alias for createChar()
void i2c1602_printstr(const char*);

void i2c1602_send(uint8_t, uint8_t);
void i2c1602_write4bits(uint8_t);
void i2c1602_expanderWrite(uint8_t);
void i2c1602_pulseEnable(uint8_t);

#endif

