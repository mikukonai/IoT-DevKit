// LCD1602 I2C 接口模块驱动 - 测试文件
// mikukonai - 2017.2
//  适合于Raspberry Pi、Intel Galileo、NanoPi等Linux开发板
//  只测试了i2c1602_printstr(const char *)函数
//    只供本人个人用途使用，本人不对代码负责

#include "i2c1602.h"

int main(int argc, char **argv)
{
	i2c1602_init();
	i2c1602_backlight();
	i2c1602_clear();

	              //0123456789abcde0123456789abcde
	char str[33] = "Hello, World!  By Mikukonai   ";

	i2c1602_printstr(str);

	i2c1602_close();
	return 0;
}