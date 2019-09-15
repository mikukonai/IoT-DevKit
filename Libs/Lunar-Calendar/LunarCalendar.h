// Gregorian Calendar to Chinese Lunar Calendar
// 公历-农历转换
// mikukonai 2015

#ifndef __MIKUKONAI_LUNAR_CALENDAR__
#define __MIKUKONAI_LUNAR_CALENDAR__

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

/*
公历年对应的农历数据,每年三字节,
格式第一字节BIT7-4 位表示闰月月份,值为0 为无闰月,BIT3-0 对应农历第1-4 月的大小
第二字节BIT7-0 对应农历第5-12 月大小,第三字节BIT7 表示农历第13 个月大小
月份对应的位为1 表示本农历月大(30 天),为0 表示小(29 天)
第三字节BIT6-5 表示春节的公历月份,BIT4-0 表示春节的公历日期
*/

uchar isLeapYear(uchar y, uchar m);

//返回某公历日的公历年积日
uint GetGregDOY(uchar gYear, uchar gMonth, uchar gDay);

//返回某公历年中的农历闰月。若无，则返回0。
uchar GetLunarLeapMonth(uchar gYr);

//返回某公历年春节月份。
uchar GetLunarSpringFestivalMonth(uchar gYr);

//返回某公历年春节日期。
uchar GetLunarSpringFestivalDay(uchar gYr);

//返回某公历年春节的公历年积日
uint GetGregSFDOY(uchar gYear);

//返回两公历日之间的公历日差值
int GetGregDif(uchar aYear, uchar aMonth, uchar aDay, uchar bYear, uchar bMonth, uchar bDay);

//返回某公历日与当公历年春节之间的日差值
int GetGregSFDif(uchar gYear, uchar gMonth, uchar gDay);

//返回含闰参数农历月的月份索引
uchar GetLunarMonthIndex(uchar LYear, uchar LMonth, uchar isLeap);

//按农历月索引返回日数
uchar GetLunarDOMfromIndex(uchar LYear, uchar mIndex);

//返回某农历月日数
uchar GetLunarDaysOfMonth(uchar LYear, uchar LMonth, uchar isLeap);

//返回某农历日与当农历年春节之间的日差值
//即农历年积日
uint GetLunarDOY(uchar LYear, uchar LMonthIndex, uchar LDay);

//返回某农历年全年日数
uint GetLunarAllYearDays(uchar LYear);

//计算农历年
uchar GetLunarYear(uchar gYear, uchar gMonth, uchar gDay);

//计算农历月(索引)
uchar GetLunarIndexMonth(uchar gYear, uchar gMonth, uchar gDay);

//计算是否为农历闰月
uchar GetLunarIsLeapMonth(uchar gYear, uchar gMonth, uchar gDay);

//计算农历月
uchar GetLunarMonth(uchar gYear, uchar gMonth, uchar gDay);

//计算农历日
uchar GetLunarDay(uchar gYear, uchar gMonth, uchar gDay);

#endif
