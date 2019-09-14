/**
 * 串行方式的基于ST7920控制器的12864液晶控制程序
 *   代码是东拼西凑在网上找到的
 *   具体原理可以参考ST7920的Datasheet，代码中并没有注释
 *   2017.10.21
 **/

#ifndef uchar
#define uchar uint8_t
#endif

#ifndef uint
#define uint unsigned int
#endif

/**
 * IO引脚和操作宏定义
 *  此处应根据具体的硬件接线方式和MCU类型进行调整。
 *  此处修改一般不会影响后面的操作时序和接口逻辑。
 */

/* IO引脚定义 */

#define LCD12864_D0  11
#define LCD12864_D1  12
#define LCD12864_D2  2
#define LCD12864_D3  3
#define LCD12864_D4  4
#define LCD12864_D5  5
#define LCD12864_D6  6
#define LCD12864_D7  7

#define LCD12864_RS  8
#define LCD12864_RW  9
#define LCD12864_EN  10
//#define LCD12864_PSB 14
#define LCD12864_RST 13

/* IO赋值操作定义 */

#define LCD12864_RS_SET digitalWrite(LCD12864_RS, HIGH)
#define LCD12864_RS_CLR digitalWrite(LCD12864_RS, LOW)

#define LCD12864_RW_SET digitalWrite(LCD12864_RW, HIGH)
#define LCD12864_RW_CLR digitalWrite(LCD12864_RW, LOW)

#define LCD12864_EN_SET digitalWrite(LCD12864_EN, HIGH)
#define LCD12864_EN_CLR digitalWrite(LCD12864_EN, LOW)

#define LCD12864_PSB_SET digitalWrite(LCD12864_PSB, HIGH)
#define LCD12864_PSB_CLR digitalWrite(LCD12864_PSB, LOW)

#define LCD12864_RST_SET digitalWrite(LCD12864_RST, HIGH)
#define LCD12864_RST_CLR digitalWrite(LCD12864_RST, LOW)

/* 总线写操作定义 */

#define LCD12864_DATA_SET(val)  \
	digitalWrite(LCD12864_D0, (val)&0x01);\
	digitalWrite(LCD12864_D1, (val)&0x02);\
	digitalWrite(LCD12864_D2, (val)&0x04);\
	digitalWrite(LCD12864_D3, (val)&0x08);\
	digitalWrite(LCD12864_D4, (val)&0x10);\
	digitalWrite(LCD12864_D5, (val)&0x20);\
	digitalWrite(LCD12864_D6, (val)&0x40);\
	digitalWrite(LCD12864_D7, (val)&0x80);

/* 总线读写模式转换操作定义 */

#define LCD12864_DATA_SET_OUTPUT  \
	pinMode(LCD12864_D0, OUTPUT);\
	pinMode(LCD12864_D1, OUTPUT);\
	pinMode(LCD12864_D2, OUTPUT);\
	pinMode(LCD12864_D3, OUTPUT);\
	pinMode(LCD12864_D4, OUTPUT);\
	pinMode(LCD12864_D5, OUTPUT);\
	pinMode(LCD12864_D6, OUTPUT);\
	pinMode(LCD12864_D7, OUTPUT);

#define LCD12864_DATA_SET_INPUT  \
	pinMode(LCD12864_D0, INPUT);\
	pinMode(LCD12864_D1, INPUT);\
	pinMode(LCD12864_D2, INPUT);\
	pinMode(LCD12864_D3, INPUT);\
	pinMode(LCD12864_D4, INPUT);\
	pinMode(LCD12864_D5, INPUT);\
	pinMode(LCD12864_D6, INPUT);\
	pinMode(LCD12864_D7, INPUT);

/* 总线读操作 */
uchar LCD12864_DATA_GET() {
	uchar tmp = 0;
	tmp |= (digitalRead(LCD12864_D7) & 0x01) << 7;
	tmp |= (digitalRead(LCD12864_D6) & 0x01) << 6;
	tmp |= (digitalRead(LCD12864_D5) & 0x01) << 5;
	tmp |= (digitalRead(LCD12864_D4) & 0x01) << 4;
	tmp |= (digitalRead(LCD12864_D3) & 0x01) << 3;
	tmp |= (digitalRead(LCD12864_D2) & 0x01) << 2;
	tmp |= (digitalRead(LCD12864_D1) & 0x01) << 1;
	tmp |= (digitalRead(LCD12864_D0) & 0x01) << 0;
	return tmp;
}

/* 延时操作定义 */

void LCD12864_Delay1ms(uint c)
{
	delayMicroseconds(c);
}

/**
 * 忙检测
 *   输入：无
 *   输出：1-不忙；0-忙
 **/

uchar LCD12864_Busy(void)
{
	uchar i = 0;

	LCD12864_RS_CLR;
	LCD12864_RW_SET;
	LCD12864_EN_SET;
	LCD12864_Delay1ms(1);
	while ((LCD12864_DATA_GET() & 0x80) == 0x80)
	{
		i++;
		if (i > 100)
		{
			LCD12864_EN_CLR;
			return 0;
		}
	}
	LCD12864_EN_CLR;
	return 1;
}

/**
 * 写指令（基本操作）
 *   输入：uchar cmd
 *   输出：无
 **/
void LCD12864_WriteCmd(uchar cmd)
{
	LCD12864_EN_SET;
	LCD12864_Delay1ms(1);
	LCD12864_RS_CLR;
	LCD12864_RW_CLR;
	LCD12864_DATA_SET(cmd);
	LCD12864_Delay1ms(5);
	LCD12864_EN_CLR;
}

/**
 * 写数据（基本操作）
 *   输入：uchar dat
 *   输出：无
 **/
void LCD12864_WriteData(uchar dat)
{
	LCD12864_EN_SET;
	LCD12864_Delay1ms(1);
	LCD12864_RS_SET;
	LCD12864_RW_CLR;
	LCD12864_DATA_SET(dat);
	LCD12864_Delay1ms(5);
	LCD12864_EN_CLR;
}

/**
 * 读数据（基本操作）
 *   输入：无
 *   输出：uchar
 **/
uchar LCD12864_ReadData(void)
{
	uchar read;
	LCD12864_DATA_SET(0xff);
	LCD12864_RS_SET;
	LCD12864_RW_SET;
	LCD12864_EN_CLR;
	LCD12864_Delay1ms(1);
	LCD12864_EN_SET;
	LCD12864_Delay1ms(1);
	read = LCD12864_DATA_GET();
	LCD12864_EN_CLR;
	return read;
}

/**
 * 初始化设备
 *   输入：uchar dat
 *   输出：无
 **/
void LCD12864_Init()
{
	//LCD12864_PSB_SET;	// 选择并行输入
	LCD12864_RST_SET;	// 复位

	LCD12864_WriteCmd(0x30);	// 基本指令集
	LCD12864_WriteCmd(0x0c);	// 显示开，关光标
	LCD12864_WriteCmd(0x01);	// 清除显示
}

/**
 * 清屏
 *   输入：无
 *   输出：无
 **/
void LCD12864_ClearScreen(void)
{
	uchar i, j;
	LCD12864_WriteCmd(0x34);	// 扩展指令集
	for (i = 0; i < 32; i++)
	{
		LCD12864_WriteCmd(0x80 + i);
		LCD12864_WriteCmd(0x80);
		for (j = 0; j < 32; j++)
		{
			LCD12864_WriteData(0x00);
		}
	}
	LCD12864_WriteCmd(0x36);	// 开显示
	LCD12864_WriteCmd(0x30);	// 基本指令集
}

/**
 * 设定光标位置
 *   输入：uchar x：行数（0-3）
 *         uchar y：列数（0-15）
 *   输出：无
 **/
void LCD12864_SetCursor(uchar x, uchar y)
{
	uchar pos;
	if (x == 0)
	{
		x = 0x80;
	}
	else if (x == 1)
	{
		x = 0x90;
	}
	else if (x == 2)
	{
		x = 0x88;
	}
	else if (x == 3)
	{
		x = 0x98;
	}
	pos = x + y;
	LCD12864_WriteCmd(pos);
}

/**
 * 绘制图像
 *   输入：uchar *mat：图像矩阵
 *   输出：无
 **/
void LCD12864_DrawMatrix(uchar *mat)
{
	unsigned char i, j;
	LCD12864_ClearScreen();
	LCD12864_WriteCmd(0x34);
	for (i = 0; i < 32; i++)
	{
		LCD12864_WriteCmd(0x80 + i);
		LCD12864_WriteCmd(0x80);
		for (j = 0; j < 16; j++)
		{
			LCD12864_WriteData(*mat);
			mat++;
		}
	}
	for (i = 0; i < 32; i++)
	{
		LCD12864_WriteCmd(0x80 + i);
		LCD12864_WriteCmd(0x88);
		for (j = 0; j < 16; j++)
		{
			LCD12864_WriteData(*mat);
			mat++;
		}
	}
	LCD12864_WriteCmd(0x36);
	LCD12864_WriteCmd(0x30);
}

/**
 * 描点
 *   输入：uchar x：横坐标（0-127）
 *         uchar y：纵坐标（0-63）
 *   输出：无
 **/
void LCD12864_Plot(uchar x, uchar y)
{
	uint bt = 0, readValue = 0;
	uchar x_adr, y_adr, h_bit, l_bit;
	if ( x < 0 || x > 127 || y < 0 || y > 63)
	{
		return;
	}
	y_adr = 0x80 + y % 32;
	if (y > 31)
	{
		x_adr = 0x88 + x / 16;
	}
	else
	{
		x_adr = 0x80 + x / 16;
	}
	bt = 0x8000 >> (x % 16);
	LCD12864_WriteCmd(0x34);
	LCD12864_WriteCmd(y_adr);
	LCD12864_WriteCmd(x_adr);
	LCD12864_DATA_SET_INPUT
	LCD12864_ReadData();
	LCD12864_Delay1ms(1);
	readValue = LCD12864_ReadData();
	readValue <<= 8;
	LCD12864_Delay1ms(1);
	readValue |= LCD12864_ReadData();
	bt = bt | readValue;
	h_bit = bt >> 8;
	l_bit = bt;
	LCD12864_DATA_SET_OUTPUT
	LCD12864_WriteCmd(y_adr);
	LCD12864_WriteCmd(x_adr);
	LCD12864_WriteData(h_bit);
	LCD12864_WriteData(l_bit);
	LCD12864_WriteCmd(0x36);
	//LCD12864_WriteCmd(0x30);
}

/**
 * 画直线（Bresenham画线算法）
 *   输入：起点终点横纵坐标
 *   输出：无
 **/
void LCD12864_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) { // Bresenham Line Algorithm
	int Dx = x1 - x0;
	int Dy = y1 - y0;
	int steep = (abs(Dy) >= abs(Dx));
	if (steep) {
		int a = x0;
		int b = y0;
		x0 = b;
		y0 = a;
		a = x1;
		b = y1;
		x1 = b;
		y1 = a;
		Dx = x1 - x0;
		Dy = y1 - y0;
	}
	int xstep = 1;
	if (Dx < 0) {
		xstep = -1;
		Dx = -Dx;
	}
	int ystep = 1;
	if (Dy < 0) {
		ystep = -1;
		Dy = -Dy;
	}
	int TwoDy = 2 * Dy;
	int TwoDyTwoDx = TwoDy - 2 * Dx; // 2*Dy - 2*Dx
	int E = TwoDy - Dx; //2*Dy - Dx
	int y = y0;
	int xDraw, yDraw;
	for (int x = x0; x != x1; x += xstep) {
		if (steep) {
			xDraw = y;
			yDraw = x;
		} else {
			xDraw = x;
			yDraw = y;
		}
		LCD12864_Plot(xDraw, yDraw);
		if (E > 0) {
			E += TwoDyTwoDx; //E += 2*Dy - 2*Dx;
			y = y + ystep;
		}
		else {
			E += TwoDy; //E += 2*Dy;
		}
	}
}

/**
 * 显示一行文字
 *   输入：字符串（GB2312），行数（0-3）
 *   输出：无
 **/
void LCD12864_PrintLine(char str[16], uchar line)
{
	uchar i;
	LCD12864_SetCursor(line, 0);
	for (i = 0; i < 16; i++)
	{
		LCD12864_WriteData(str[i]);
	}
}

/**
 * 在指定位置显示字符
 *   输入：字符（GB2312），行数（0-3），列数
 *   输出：无
 **/
void LCD12864_PrintChar(char c1, char c2, uchar line, uchar col)
{
	LCD12864_SetCursor(line, col);
	LCD12864_WriteData(c1);
	LCD12864_WriteData(c2);
}

/**
 * Arduino测试程序
 **/
void setup() {
	pinMode(LCD12864_RS, OUTPUT);
	pinMode(LCD12864_RW, OUTPUT);
	pinMode(LCD12864_EN, OUTPUT);
	pinMode(LCD12864_RST, OUTPUT);
	pinMode(LCD12864_D0, OUTPUT);
	pinMode(LCD12864_D1, OUTPUT);
	pinMode(LCD12864_D2, OUTPUT);
	pinMode(LCD12864_D3, OUTPUT);
	pinMode(LCD12864_D4, OUTPUT);
	pinMode(LCD12864_D5, OUTPUT);
	pinMode(LCD12864_D6, OUTPUT);
	pinMode(LCD12864_D7, OUTPUT);
	LCD12864_Init();
	LCD12864_ClearScreen();
}

void loop() {
	LCD12864_PrintLine("                ", 0);
	LCD12864_PrintLine("                ", 1);
	LCD12864_PrintLine("                ", 2);
	LCD12864_PrintLine("                ", 3);
	LCD12864_ClearScreen();

	LCD12864_WriteCmd(0x30);

	LCD12864_PrintLine("\xcf\xb2\xd3\xad\xca\xae\xbe\xc5\xb4\xf3      ", 0);

	while (1);
}
