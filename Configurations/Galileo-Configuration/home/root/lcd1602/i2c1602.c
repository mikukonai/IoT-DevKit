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
#define I2C_DEVFILE "/dev/i2c-0"
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

int main(int argc, char **argv) {
	i2c1602_init();
	i2c1602_backlight();
	i2c1602_clear();

	FILE *fd = fopen("/home/root/lcd1602/screen.txt","r");
	int i = 0;
	int count = 0;
	char str[41];
	char c;

	memset(str, 0, sizeof(char)*40);
	
	do{
		c = fgetc(fd);
		count++;
		if(c != '\n' && c != '\r')
			str[i++] = c;
	}while(c != EOF && i <= 32 && count <= 32);
	str[i] = 0;
	printf("%s",str);
	i2c1602_printstr(str);

	fclose(fd);
	close(i2cdev_fd);
	return 0;
}