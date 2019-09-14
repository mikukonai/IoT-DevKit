/*
 * SSD1306 I2C Driver Test
 * Mikukonai 2017
 *
 */
#include "SSD1306.h"

int main()
{
	SSD1306_init();
	SSD1306_clearDisplay();
  SSD1306_setBrightness(255);

  SSD1306_setPageMode();

  FILE *fd = fopen("/home/root/ssd1306/screen.txt","r");
  int i = 0;
  int count = 0;
  char str[90];
  char c;

  memset(str, 0, sizeof(char)*90);
  
  do{
    c = fgetc(fd);
    if(c != '\n' && c != '\r' && c != EOF)
      str[i++] = c;
    else if(c == '\r') {
      continue;
    }
    else {
      str[i] = 0;
      i = 0;
      printf("%u : %s\n", count, str);
      SSD1306_setTextXY(count, 0);
      SSD1306_putString(str);
      count++;
    }
  }while(c != EOF);

  fclose(fd);
	close(ssd1306_i2c_devfd);
	return 0;
}
