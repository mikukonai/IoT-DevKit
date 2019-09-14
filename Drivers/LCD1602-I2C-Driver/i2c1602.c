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

#include "i2c1602.h"

void i2c1602_var_init(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows)
{
  _Addr = lcd_Addr;
  _cols = lcd_cols;
  _rows = lcd_rows;
  _backlightval = LCD_NOBACKLIGHT;
}

void i2c1602_init(){
	i2c1602_var_init(I2C_ADDR, 16, 2);
	//I2C init
	i2cdev_fd = open(I2C_DEVFILE, O_RDWR);
	if(ioctl(i2cdev_fd, I2C_SLAVE, I2C_ADDR)<0 )
	{
		printf("LCD1602 ioctl error : %s\r\n", strerror(errno));
	}
	i2c1602_begin(16, 2, 0);
}

void i2c1602_close() {
	close(i2cdev_fd);
}

void i2c1602_begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
	if (lines > 1) {
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	delay(50); 

	// Now we pull both RS and R/W low to begin commands
	i2c1602_expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	delay(1000);

  	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46

	// we start in 8bit mode, try to set 4 bit mode
	i2c1602_write4bits(0x03 << 4);
	delayMicroseconds(4500); // wait min 4.1ms

	// second try
	i2c1602_write4bits(0x03 << 4);
	delayMicroseconds(4500); // wait min 4.1ms

	// third go!
	i2c1602_write4bits(0x03 << 4); 
	delayMicroseconds(150);

	// finally, set to 4-bit interface
	i2c1602_write4bits(0x02 << 4); 

	// set # lines, font size, etc.
	i2c1602_command(LCD_FUNCTIONSET | _displayfunction);  

	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	i2c1602_display();

	// clear it off
	i2c1602_clear();

	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	// set the entry mode
	i2c1602_command(LCD_ENTRYMODESET | _displaymode);

	i2c1602_home();
}

/********** high level commands, for the user! */
void i2c1602_clear(){
	i2c1602_command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void i2c1602_home(){
	i2c1602_command(LCD_RETURNHOME);  // set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void i2c1602_setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > _numlines ) {
		row = _numlines-1;    // we count rows starting w/0
	}
	i2c1602_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void i2c1602_noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	i2c1602_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void i2c1602_display() {
	_displaycontrol |= LCD_DISPLAYON;
	i2c1602_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void i2c1602_noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	i2c1602_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void i2c1602_cursor() {
	_displaycontrol |= LCD_CURSORON;
	i2c1602_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void i2c1602_noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	i2c1602_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void i2c1602_blink() {
	_displaycontrol |= LCD_BLINKON;
	i2c1602_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void i2c1602_scrollDisplayLeft(void) {
	i2c1602_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void i2c1602_scrollDisplayRight(void) {
	i2c1602_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void i2c1602_leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	i2c1602_command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void i2c1602_rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	i2c1602_command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void i2c1602_autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	i2c1602_command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void i2c1602_noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	i2c1602_command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void i2c1602_createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	i2c1602_command(LCD_SETCGRAMADDR | (location << 3));
	int i = 0;
	for (i=0; i<8; i++) {
		i2c1602_write(charmap[i]);
	}
}

// Turn the (optional) backlight off/on
void i2c1602_noBacklight(void) {
	_backlightval=LCD_NOBACKLIGHT;
	i2c1602_expanderWrite(0);
}

void i2c1602_backlight(void) {
	_backlightval=LCD_BACKLIGHT;
	i2c1602_expanderWrite(0);
}

/*********** mid level commands, for sending data/cmds */

void i2c1602_command(uint8_t value) {
	i2c1602_send(value, 0);
}

void i2c1602_write(uint8_t value) {
	i2c1602_send(value, Rs);
}


/************ low level data pushing commands **********/

// write either command or data
// mode - Rs[P0](H:Data L:Command)
void i2c1602_send(uint8_t value, uint8_t mode) {
	uint8_t highnib = value & 0xf0;
	uint8_t lownib = (value<<4) & 0xf0;
	i2c1602_write4bits((highnib)|mode);
	i2c1602_write4bits((lownib)|mode); 
}

void i2c1602_write4bits(uint8_t value) {
	i2c1602_expanderWrite(value);
	i2c1602_pulseEnable(value);
}

void i2c1602_expanderWrite(uint8_t _data){
	//printIIC(_data | _backlightval);
	uint8_t val = _data | _backlightval;
	if( write(i2cdev_fd, &val, 1) < 0 )
	{
		printf("I2C write error\n");
	}
}

void i2c1602_pulseEnable(uint8_t _data){
	i2c1602_expanderWrite(_data | En);	// En high
	delayMicroseconds(1);		// enable pulse must be >450ns
	
	i2c1602_expanderWrite(_data & ~En);	// En low
	delayMicroseconds(50);		// commands need > 37us to settle
} 

void i2c1602_printstr(const char *str)
{
	int len = strlen(str);
	int count = 0;
	if(len > 32)
		printf("String too long\n");

	i2c1602_setCursor(0,0);
	for(count = 0; count < 16; count++) {
		if(0 == str[count]) {return;}
		i2c1602_write(str[count]);
	}
	i2c1602_setCursor(0,1);
	for(count = 16; count < 32; count++) {
		if(0 == str[count]) {return;}
		i2c1602_write(str[count]);
	}
}
