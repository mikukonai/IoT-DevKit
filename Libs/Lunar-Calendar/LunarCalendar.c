// Gregorian Calendar to Chinese Lunar Calendar
// 公历-农历转换
// mikukonai 2015

#include "LunarCalendar.h"

/*
公历年对应的农历数据,每年三字节,
格式第一字节BIT7-4 位表示闰月月份,值为0 为无闰月,BIT3-0 对应农历第1-4 月的大小
第二字节BIT7-0 对应农历第5-12 月大小,第三字节BIT7 表示农历第13 个月大小
月份对应的位为1 表示本农历月大(30 天),为0 表示小(29 天)
第三字节BIT6-5 表示春节的公历月份,BIT4-0 表示春节的公历日期
*/
const uchar LunarData[597] = {
	0x04, 0xAe, 0x53,    //1901 0
	0x0A, 0x57, 0x48,    //1902 3
	0x55, 0x26, 0xBd,    //1903 6
	0x0d, 0x26, 0x50,    //1904 9
	0x0d, 0x95, 0x44,    //1905 12
	0x46, 0xAA, 0xB9,    //1906 15
	0x05, 0x6A, 0x4d,    //1907 18
	0x09, 0xAd, 0x42,    //1908 21
	0x24, 0xAe, 0xB6,    //1909
	0x04, 0xAe, 0x4A,    //1910
	0x6A, 0x4d, 0xBe,    //1911
	0x0A, 0x4d, 0x52,    //1912
	0x0d, 0x25, 0x46,    //1913
	0x5d, 0x52, 0xBA,    //1914
	0x0B, 0x54, 0x4e,    //1915
	0x0d, 0x6A, 0x43,    //1916
	0x29, 0x6d, 0x37,    //1917
	0x09, 0x5B, 0x4B,    //1918
	0x74, 0x9B, 0xC1,    //1919
	0x04, 0x97, 0x54,    //1920
	0x0A, 0x4B, 0x48,    //1921
	0x5B, 0x25, 0xBC,    //1922
	0x06, 0xA5, 0x50,    //1923
	0x06, 0xd4, 0x45,    //1924
	0x4A, 0xdA, 0xB8,    //1925
	0x02, 0xB6, 0x4d,    //1926
	0x09, 0x57, 0x42,    //1927
	0x24, 0x97, 0xB7,    //1928
	0x04, 0x97, 0x4A,    //1929
	0x66, 0x4B, 0x3e,    //1930
	0x0d, 0x4A, 0x51,    //1931
	0x0e, 0xA5, 0x46,    //1932
	0x56, 0xd4, 0xBA,    //1933
	0x05, 0xAd, 0x4e,    //1934
	0x02, 0xB6, 0x44,    //1935
	0x39, 0x37, 0x38,    //1936
	0x09, 0x2e, 0x4B,    //1937
	0x7C, 0x96, 0xBf,    //1938
	0x0C, 0x95, 0x53,    //1939
	0x0d, 0x4A, 0x48,    //1940
	0x6d, 0xA5, 0x3B,    //1941
	0x0B, 0x55, 0x4f,    //1942
	0x05, 0x6A, 0x45,    //1943
	0x4A, 0xAd, 0xB9,    //1944
	0x02, 0x5d, 0x4d,    //1945
	0x09, 0x2d, 0x42,    //1946
	0x2C, 0x95, 0xB6,    //1947
	0x0A, 0x95, 0x4A,    //1948
	0x7B, 0x4A, 0xBd,    //1949
	0x06, 0xCA, 0x51,    //1950
	0x0B, 0x55, 0x46,    //1951
	0x55, 0x5A, 0xBB,    //1952
	0x04, 0xdA, 0x4e,    //1953
	0x0A, 0x5B, 0x43,    //1954
	0x35, 0x2B, 0xB8,    //1955
	0x05, 0x2B, 0x4C,    //1956
	0x8A, 0x95, 0x3f,    //1957
	0x0e, 0x95, 0x52,    //1958
	0x06, 0xAA, 0x48,    //1959
	0x7A, 0xd5, 0x3C,    //1960
	0x0A, 0xB5, 0x4f,    //1961
	0x04, 0xB6, 0x45,    //1962
	0x4A, 0x57, 0x39,    //1963
	0x0A, 0x57, 0x4d,    //1964
	0x05, 0x26, 0x42,    //1965
	0x3e, 0x93, 0x35,    //1966
	0x0d, 0x95, 0x49,    //1967
	0x75, 0xAA, 0xBe,    //1968
	0x05, 0x6A, 0x51,    //1969
	0x09, 0x6d, 0x46,    //1970
	0x54, 0xAe, 0xBB,    //1971
	0x04, 0xAd, 0x4f,    //1972
	0x0A, 0x4d, 0x43,    //1973
	0x4d, 0x26, 0xB7,    //1974
	0x0d, 0x25, 0x4B,    //1975
	0x8d, 0x52, 0xBf,    //1976
	0x0B, 0x54, 0x52,    //1977
	0x0B, 0x6A, 0x47,    //1978
	0x69, 0x6d, 0x3C,    //1979
	0x09, 0x5B, 0x50,    //1980
	0x04, 0x9B, 0x45,    //1981
	0x4A, 0x4B, 0xB9,    //1982
	0x0A, 0x4B, 0x4d,    //1983
	0xAB, 0x25, 0xC2,    //1984
	0x06, 0xA5, 0x54,    //1985
	0x06, 0xd4, 0x49,    //1986
	0x6A, 0xdA, 0x3d,    //1987
	0x0A, 0xB6, 0x51,    //1988
	0x09, 0x37, 0x46,    //1989
	0x54, 0x97, 0xBB,    //1990
	0x04, 0x97, 0x4f,    //1991
	0x06, 0x4B, 0x44,    //1992
	0x36, 0xA5, 0x37,    //1993
	0x0e, 0xA5, 0x4A,    //1994
	0x86, 0xB2, 0xBf,    //1995
	0x05, 0xAC, 0x53,    //1996
	0x0A, 0xB6, 0x47,    //1997
	0x59, 0x36, 0xBC,    //1998
	0x09, 0x2e, 0x50,    //1999 294
	0x0C, 0x96, 0x45,    //2000 297
	0x4d, 0x4A, 0xB8,    //2001
	0x0d, 0x4A, 0x4C,    //2002
	0x0d, 0xA5, 0x41,    //2003
	0x25, 0xAA, 0xB6,    //2004
	0x05, 0x6A, 0x49,    //2005
	0x7A, 0xAd, 0xBd,    //2006
	0x02, 0x5d, 0x52,    //2007
	0x09, 0x2d, 0x47,    //2008
	0x5C, 0x95, 0xBA,    //2009
	0x0A, 0x95, 0x4e,    //2010
	0x0B, 0x4A, 0x43,    //2011
	0x4B, 0x55, 0x37,    //2012
	0x0A, 0xd5, 0x4A,    //2013
	0x95, 0x5A, 0xBf,    //2014
	0x04, 0xBA, 0x53,    //2015
	0x0A, 0x5B, 0x48,    //2016
	0x65, 0x2B, 0xBC,    //2017
	0x05, 0x2B, 0x50,    //2018
	0x0A, 0x93, 0x45,    //2019
	0x47, 0x4A, 0xB9,    //2020
	0x06, 0xAA, 0x4C,    //2021
	0x0A, 0xd5, 0x41,    //2022
	0x24, 0xdA, 0xB6,    //2023
	0x04, 0xB6, 0x4A,    //2024
	0x69, 0x57, 0x3d,    //2025
	0x0A, 0x4e, 0x51,    //2026
	0x0d, 0x26, 0x46,    //2027
	0x5e, 0x93, 0x3A,    //2028
	0x0d, 0x53, 0x4d,    //2029
	0x05, 0xAA, 0x43,    //2030
	0x36, 0xB5, 0x37,    //2031
	0x09, 0x6d, 0x4B,    //2032
	0xB4, 0xAe, 0xBf,    //2033
	0x04, 0xAd, 0x53,    //2034
	0x0A, 0x4d, 0x48,    //2035
	0x6d, 0x25, 0xBC,    //2036
	0x0d, 0x25, 0x4f,    //2037
	0x0d, 0x52, 0x44,    //2038
	0x5d, 0xAA, 0x38,    //2039
	0x0B, 0x5A, 0x4C,    //2040
	0x05, 0x6d, 0x41,    //2041
	0x24, 0xAd, 0xB6,    //2042
	0x04, 0x9B, 0x4A,    //2043
	0x7A, 0x4B, 0xBe,    //2044
	0x0A, 0x4B, 0x51,    //2045
	0x0A, 0xA5, 0x46,    //2046
	0x5B, 0x52, 0xBA,    //2047
	0x06, 0xd2, 0x4e,    //2048
	0x0A, 0xdA, 0x42,    //2049
};

uchar days_of_month[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

uchar isLeapYear(uchar y, uchar m)
{
	int year = 2000+(int)y;
	if(((year%4==0)&&(year%100!=0))||(year%400==0))
	{
		if(m==2)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

//返回某公历日的公历年积日
uint GetGregDOY(uchar gYear, uchar gMonth, uchar gDay)
{
	uint DOY = 0;
	uchar mCount = 1;
	for(mCount = 1; mCount < gMonth; mCount++)
	{
		DOY += ((uint)days_of_month[mCount] + (uint)isLeapYear(gYear, mCount));
	}
	DOY += (uint)gDay;

	return DOY;
}

//返回某公历年中的农历闰月。若无，则返回0。
uchar GetLunarLeapMonth(uchar gYr)
{
	uint gYear = 2000 + (uint)gYr;
	uint addr = (gYear - 1901) * 3;
	uchar leapMonth = LunarData[addr];
	leapMonth = leapMonth >> 4;
	leapMonth &= 0x0f;

	return leapMonth;
}

//返回某公历年春节月份。
uchar GetLunarSpringFestivalMonth(uchar gYr)
{
	uint gYear = 2000 + (uint)gYr;
	uint addr = (gYear - 1901) * 3 + 2;
	uchar SFMonth = LunarData[addr];
	SFMonth = SFMonth >> 5;
	SFMonth &= 0x03;

	return SFMonth;
}

//返回某公历年春节日期。
uchar GetLunarSpringFestivalDay(uchar gYr)
{
	uint gYear = 2000 + (uint)gYr;
	uint addr = (gYear - 1901) * 3 + 2;
	uchar SFDay = LunarData[addr];
	SFDay &= 0x1f;

	return SFDay;
}

//返回某公历年春节的公历年积日
uint GetGregSFDOY(uchar gYear)
{
	uchar SFMonth = GetLunarSpringFestivalMonth(gYear);
	uchar SFDay   = GetLunarSpringFestivalDay(gYear);
	uint DOY = GetGregDOY(gYear, SFMonth, SFDay);

	return DOY;
}

//返回两公历日之间的公历日差值
int GetGregDif(uchar aYear, uchar aMonth, uchar aDay, uchar bYear, uchar bMonth, uchar bDay)
{
	int aDOY = (int)GetGregDOY(aYear, aMonth, aDay);
	int bDOY = (int)GetGregDOY(bYear, bMonth, bDay);
	int dif  = bDOY - aDOY;

	return dif;
}

//返回某公历日与当公历年春节之间的日差值
int GetGregSFDif(uchar gYear, uchar gMonth, uchar gDay)
{
	int SFDOY = (int)GetGregSFDOY(gYear);
	int gDOY = (int)GetGregDOY(gYear, gMonth, gDay);
	int dif  = gDOY - SFDOY;

	return dif;
}

//返回含闰参数农历月的月份索引
uchar GetLunarMonthIndex(uchar LYear, uchar LMonth, uchar isLeap)
{
	uchar leapMonth = GetLunarLeapMonth(LYear);
	uchar monthIndex = 0;

	if(leapMonth == 0)
	{
		monthIndex = LMonth;
	}
	else
	{
		if(LMonth < leapMonth)
		{
			monthIndex = LMonth;
		}
		else if(LMonth == leapMonth && isLeap == 0)
		{
			monthIndex = LMonth;
		}
		else if(LMonth == leapMonth && isLeap != 0)
		{
			monthIndex = LMonth + 1;
		}
		else if(LMonth < leapMonth)
		{
			monthIndex = LMonth + 1;
		}
	}

	return monthIndex;
}

//按农历月索引返回日数
uchar GetLunarDOMfromIndex(uchar LYear, uchar mIndex)
{
	uchar isLeapYear = GetLunarLeapMonth(LYear);

	uint Year = 2000 + (uint)LYear;
	uint addr = (Year - 1901) * 3;
	uchar size = 0;
	uchar DOM = 0;

	//将含闰月参数的实际月份转化为表内地址，便于查表
	switch(mIndex)
	{
	case 1:
		size = LunarData[addr];
		size >>= 3;
		size &= 0x01;
		DOM = (size==0) ? (29) : (30);
		break;
	case 2:
		size = LunarData[addr];
		size >>= 2;
		size &= 0x01;
		DOM = (size==0) ? (29) : (30);
		break;
	case 3:
		size = LunarData[addr];
		size >>= 1;
		size &= 0x01;
		DOM = (size==0) ? (29) : (30);
		break;
	case 4:
		size = LunarData[addr];
		size &= 0x01;
		DOM = (size==0) ? (29) : (30);
		break;

	case 5:
		size = LunarData[addr+1];
		size >>= 7;
		size &= 0x01;
		DOM = (size==0) ? (29) : (30);
		break;
	case 6:
		size = LunarData[addr+1];
		size >>= 6;
		size &= 0x01;
		DOM = (size==0) ? (29) : (30);
		break;
	case 7:
		size = LunarData[addr+1];
		size >>= 5;
		size &= 0x01;
		DOM = (size==0) ? (29) : (30);
		break;
	case 8:
		size = LunarData[addr+1];
		size >>= 4;
		size &= 0x01;
		DOM = (size==0) ? (29) : (30);
		break;
	case 9:
		size = LunarData[addr+1];
		size >>= 3;
		size &= 0x01;
		DOM = (size==0) ? (29) : (30);
		break;
	case 10:
		size = LunarData[addr+1];
		size >>= 2;
		size &= 0x01;
		DOM = (size==0) ? (29) : (30);
		break;
	case 11:
		size = LunarData[addr+1];
		size >>= 1;
		size &= 0x01;
		DOM = (size==0) ? (29) : (30);
		break;
	case 12:
		size = LunarData[addr+1];
		size &= 0x01;
		DOM = (size==0) ? (29) : (30);
		break;

	case 13:
		if(isLeapYear == 0)
		{
			return 0;
		}
		else
		{
			size = LunarData[addr+2];
			size >>= 7;
			size &= 0x01;
			DOM = (size==0) ? (29) : (30);
			break;
		}
	}

	return DOM;
}

//返回某农历月日数
uchar GetLunarDaysOfMonth(uchar LYear, uchar LMonth, uchar isLeap)
{
	uchar leapMonth = GetLunarLeapMonth(LYear);
	uchar monthIndex = GetLunarMonthIndex(LYear, LMonth, isLeap);

	//将含闰月参数的实际月份转化为表内地址，便于查表
	uchar DOM = GetLunarDOMfromIndex(LYear, monthIndex);

	return DOM;
}

//返回某农历日与当农历年春节之间的日差值
//即农历年积日
uint GetLunarDOY(uchar LYear, uchar LMonthIndex, uchar LDay)
{
	uint DOY = 0;
	uchar mCount = 1;
	for(mCount = 1; mCount < LMonthIndex; mCount++)
	{
		DOY += (uint)GetLunarDOMfromIndex(LYear, mCount);
	}
	if(LDay <= GetLunarDOMfromIndex(LYear, LMonthIndex))
	{
		DOY += (uint)LDay;
	}
	else
	{
		DOY += 0;
	}

	return DOY;
}

//返回某农历年全年日数
uint GetLunarAllYearDays(uchar LYear)
{
	uint DOY = 0;
	uchar mCount = 1;
	uchar maxMonthIndex = 0;
	
	if(GetLunarLeapMonth(LYear)==0)
	{
		maxMonthIndex = 12;
	}
	else
	{
		maxMonthIndex = 13;
	}

	for(mCount = 1; mCount <= maxMonthIndex; mCount++)
	{
		DOY += (uint)GetLunarDOMfromIndex(LYear, mCount);
	}

	return DOY;
}

//计算农历年
uchar GetLunarYear(uchar gYear, uchar gMonth, uchar gDay)
{
	int SFDif = GetGregSFDif(gYear, gMonth, gDay);
	//若当日在春节前，则年份减一
	if(SFDif < 0)
	{
		return (gYear - 1);
	}
	//若当日在春节或之后，则农历年等于公历年
	else
	{
		return gYear;
	}
}


//计算农历月(索引)
uchar GetLunarIndexMonth(uchar gYear, uchar gMonth, uchar gDay)
{
	uchar LYear = GetLunarYear(gYear, gMonth, gDay);
	//uchar leapMonth = GetLunarLeapMonth(gYear);

	int SFDif = GetGregSFDif(gYear, gMonth, gDay);
	int prev = 0;
	int next = 0;

	uchar mCount = 0;
	uchar LunarMonth = 0;

	if(SFDif == 0)
	{
		return 1;
	}
	//正月以后
	else if(SFDif > 0)
	{
		for(mCount = 1; mCount < 14; mCount++)
		{
			prev = GetLunarDOY(LYear, mCount, 0);   //本月0日年积日
			next = GetLunarDOY(LYear, mCount+1, 0); //下月0日年积日
			//若春节到公历的年积日恰好在mCount期间，则该公历日的农历月必为mCount
			if(SFDif >= prev && SFDif < next)
			{
				LunarMonth = mCount;
				return LunarMonth;
			}
		}
	}
	//正月以前
	else
	{
		//负日数转为距上个农历年春节的正日数
		SFDif = GetLunarAllYearDays(LYear) + SFDif;
		for(mCount = 1; mCount < 14; mCount++)
		{
			prev = GetLunarDOY(LYear, mCount, 0);   //本月0日年积日
			next = GetLunarDOY(LYear, mCount+1, 0); //下月0日年积日
			//若春节到公历的年积日恰好在mCount期间，则该公历日的农历月必为mCount
			if(SFDif >= prev && SFDif < next)
			{
				LunarMonth = mCount;
				return LunarMonth;
			}
		}
		return 5;
	}
	return 1;
}

//计算是否为农历闰月
uchar GetLunarIsLeapMonth(uchar gYear, uchar gMonth, uchar gDay)
{
	uchar LunarYear  = GetLunarYear(gYear, gMonth, gDay);
	uchar LunarMIndex = GetLunarIndexMonth(gYear, gMonth, gDay);
	uchar LeapMonth  = GetLunarLeapMonth(LunarYear);
	if(LeapMonth != 0)
	{
		if(LunarMIndex == LeapMonth + 1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

//计算农历月
uchar GetLunarMonth(uchar gYear, uchar gMonth, uchar gDay)
{
	uchar LunarYear  = GetLunarYear(gYear, gMonth, gDay);
	uchar LunarMIndex = GetLunarIndexMonth(gYear, gMonth, gDay);
	uchar LeapMonth  = GetLunarLeapMonth(LunarYear);
	if(LeapMonth != 0)
	{
		if(LunarMIndex <= LeapMonth)
		{
			return LunarMIndex;
		}
		else if(LunarMIndex > LeapMonth)
		{
			return (LunarMIndex-1);
		}
	}
	else
	{
		return LunarMIndex;
	}
	return 2;
}

//计算农历日
uchar GetLunarDay(uchar gYear, uchar gMonth, uchar gDay)
{
	uchar LunarYear  = GetLunarYear(gYear, gMonth, gDay);
	uchar LunarMIndex = GetLunarIndexMonth(gYear, gMonth, gDay);
	int SFDif = GetGregSFDif(gYear, gMonth, gDay);
	int LunarDOY = 0;
	int LunarAllYearDays = 0;
	uchar dayCount = 0;

	if(SFDif == 0)
	{
		return 1;
	}
	else if(SFDif > 0)
	{
		for(dayCount = 1; dayCount<=GetLunarDOMfromIndex(LunarYear, LunarMIndex); dayCount++)
		{
			if( GetLunarDOY(LunarYear, LunarMIndex, dayCount) == SFDif + 1 )
			{
				return dayCount;
			}
		}
	}
	else if(SFDif < 0)
	{
		for(dayCount = 1; dayCount<=GetLunarDOMfromIndex(LunarYear, LunarMIndex); dayCount++)
		{
			LunarDOY = GetLunarDOY(LunarYear, LunarMIndex, dayCount) - 1;
			LunarAllYearDays = GetLunarAllYearDays(LunarYear);
			if( LunarDOY - SFDif ==  LunarAllYearDays )
			{
				return dayCount;
			}
		}
	}
	return 3;
}


