// Gregorian Calendar to Chinese Lunar Calendar
// 公历-农历转换
// mikukonai 2017

#include "LunarCalendar.h"
#include <stdio.h>

int main(int argc, char **argv) {
	int g_year = 0;
	int g_month = 0;
	int g_day = 0;
	printf("请输入公历年（2000年以后，两位数，例如2017-17）：");
	scanf("%d", &g_year);
	printf("请输入公历月份：");
	scanf("%d", &g_month);
	printf("请输入公历日：");
	scanf("%d", &g_day);

	int isLeapMonth = GetLunarIsLeapMonth(g_year, g_month, g_day);
	int l_year = GetLunarYear(g_year, g_month, g_day);
	int l_month = GetLunarMonth(g_year, g_month, g_day);
	int l_day = GetLunarDay(g_year, g_month, g_day);

	char zh_day[187] = "喵喵初一初二初三初四初五初六初七初八初九初十十一十二十三十四十五十六十七十八十九二十廿一廿二廿三廿四廿五廿六廿七廿八廿九三十";

	printf("农历 %d 年", l_year);

	if(isLeapMonth) {
		printf("闰");
	}

	switch(l_month) {
		case 1:  printf("正月"); break;
		case 2:  printf("二月"); break;
		case 3:  printf("三月"); break;
		case 4:  printf("四月"); break;
		case 5:  printf("五月"); break;
		case 6:  printf("六月"); break;
		case 7:  printf("七月"); break;
		case 8:  printf("八月"); break;
		case 9:  printf("九月"); break;
		case 10: printf("十月"); break;
		case 11: printf("冬月"); break;
		case 12: printf("腊月"); break;
		default: printf("喵喵喵？"); break;
	}
	char d[7];
	d[0] = zh_day[l_day * 6];
	d[1] = zh_day[l_day * 6 + 1];
	d[2] = zh_day[l_day * 6 + 2];
	d[3] = zh_day[l_day * 6 + 3];
	d[4] = zh_day[l_day * 6 + 4];
	d[5] = zh_day[l_day * 6 + 5];
	d[6] = '\0';
	printf("%s\n", d);


	return 0;
}
