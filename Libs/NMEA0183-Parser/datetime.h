// https://en.wikipedia.org/wiki/ISO_8601

#ifndef __MK_DATETIME__
#define __MK_DATETIME__

#include <stdio.h>
#include <stdlib.h>

#define IS_NUMBER(x) ((x) >= '0' && (x) <= '9')
#define IS_LOOP_YEAR(x) (((x) % 400 == 0) || ((x) % 4 == 0 && (x) % 100 != 0))

#define uint8_t unsigned char
#define uint16_t unsigned short
#define int8_t char
#define int16_t short

static uint8_t days_of_month[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

typedef struct {
	uint8_t year_h;
	uint8_t year_l;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint16_t millisecond;
	int8_t  timezone;	// Timezone: [+|-][NN] : eg. CST +80
}Datetime;

int8_t datetime_ctor(Datetime *dt, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint16_t millisecond) {
	if(year > 0 && year < 9999) {
		dt->year_h = (uint8_t)(year / 100);
		dt->year_l = (uint8_t)(year % 100);
	}
	else {
		fprintf(stderr, "Error: illegal 'year'.\n");
		return -1;
	}
	// is leap-year?
	if(IS_LOOP_YEAR(year)) {
		days_of_month[2] = 29;
	}
	else {
		days_of_month[2] = 28;
	}
	// month validating
	if(1 <= month && month <= 12) {
		dt->month = month;
	}
	else {
		fprintf(stderr, "Error: illegal 'month'.\n");
		return -1;
	}
	// day validating
	if(1 <= day && day <= days_of_month[month]) {
		dt->day = day;
	}
	else {
		fprintf(stderr, "Error: illegal 'day'.\n");
		return -1;
	}
	// hour validating
	if(0 <= hour && hour <= 23) {
		dt->hour = hour;
	}
	else {
		fprintf(stderr, "Error: illegal 'hour'.\n");
		return -1;
	}
	// minute validating
	if(0 <= minute && minute <= 59) {
		dt->minute = minute;
	}
	else {
		fprintf(stderr, "Error: illegal 'minute'.\n");
		return -1;
	}
	// second validating
	if(0 <= second && second <= 59) {
		dt->second = second;
	}
	else {
		fprintf(stderr, "Error: illegal 'second'.\n");
		return -1;
	}
	// millisecond validating
	if(0 <= millisecond && millisecond <= 999) {
		dt->millisecond = millisecond;
	}
	else {
		fprintf(stderr, "Error: illegal 'millisecond'.\n");
		return -1;
	}

	dt->timezone = +80; // default CST

	return 0; // success
}

/**
 * 将日期时间结构体转化成字符串
 * 字符串格式：yyyy-mm-dd hh:MM:ss.SSS
 */
char *datetime_to_string(Datetime *dt, char *dt_string) {
	uint16_t year = dt->year_h * 100 + dt->year_l;
	uint8_t index = 0;
	dt_string[0] = year / 1000 + '0';
	dt_string[1] = year % 1000 / 100 + '0';
	dt_string[2] = year % 100 / 10 + '0';
	dt_string[3] = year % 10 + '0';

	dt_string[4] = '-';

	dt_string[5] = dt->month / 10 + '0';
	dt_string[6] = dt->month % 10 + '0';

	dt_string[7] = '-';

	dt_string[8] = dt->day / 10 + '0';
	dt_string[9] = dt->day % 10 + '0';

	dt_string[10] = ' ';

	dt_string[11] = dt->hour / 10 + '0';
	dt_string[12] = dt->hour % 10 + '0';

	dt_string[13] = ':';

	dt_string[14] = dt->minute / 10 + '0';
	dt_string[15] = dt->minute % 10 + '0';

	dt_string[16] = ':';

	dt_string[17] = dt->second / 10 + '0';
	dt_string[18] = dt->second % 10 + '0';

	dt_string[19] = '.';

	dt_string[20] = dt->millisecond / 100 + '0';
	dt_string[21] = dt->millisecond % 100 / 10 + '0';
	dt_string[22] = dt->millisecond % 10 + '0';

	dt_string[23] = '\0';

	return dt_string;
}

/**
 * 将字符串转化成日期时间结构体
 * 字符串格式：yyyy-mm-dd hh:MM:ss.SSS
 */
Datetime *string_to_datetime(char *dt_string, Datetime *dt) {
	uint16_t year = 0;
	uint8_t  month = 0;
	uint8_t  day = 0;
	uint8_t  hour = 0;
	uint8_t  minute = 0;
	uint8_t  second = 0;
	uint16_t  millisecond = 0;

	year += (IS_NUMBER(dt_string[0]) ? (dt_string[0] - '0') : 0) * 1000;
	year += (IS_NUMBER(dt_string[1]) ? (dt_string[1] - '0') : 0) * 100;
	year += (IS_NUMBER(dt_string[2]) ? (dt_string[2] - '0') : 0) * 10;
	year += (IS_NUMBER(dt_string[3]) ? (dt_string[3] - '0') : 0);

	if(year > 0 && year < 9999) {
		dt->year_h = (uint8_t)(year / 100);
		dt->year_l = (uint8_t)(year % 100);
	}
	else {
		fprintf(stderr, "Error: bad datetime syntax or wrong 'year' number.\n");
		return NULL;
	}

	// is leap-year?
	if(IS_LOOP_YEAR(year)) {
		days_of_month[2] = 29;
	}
	else {
		days_of_month[2] = 28;
	}

	if(dt_string[4] != '-') {
		fprintf(stderr, "Error: bad datetime syntax: seperator.\n");
		return NULL;
	}

	month += (IS_NUMBER(dt_string[5]) ? (dt_string[5] - '0') : 0) * 10;
	month += (IS_NUMBER(dt_string[6]) ? (dt_string[6] - '0') : 0);

	if(month >= 1 && month <= 12) {
		dt->month = month;
	}
	else {
		fprintf(stderr, "Error: bad datetime syntax or wrong 'month' number.\n");
		return NULL;
	}

	if(dt_string[7] != '-') {
		fprintf(stderr, "Error: bad datetime syntax: seperator.\n");
		return NULL;
	}

	day += (IS_NUMBER(dt_string[8]) ? (dt_string[8] - '0') : 0) * 10;
	day += (IS_NUMBER(dt_string[9]) ? (dt_string[9] - '0') : 0);

	if(day >= 1 && day <= days_of_month[month]) {
		dt->day = day;
	}
	else {
		fprintf(stderr, "Error: bad datetime syntax or wrong 'day' number.\n");
		return NULL;
	}

	if(dt_string[10] != ' ') {
		fprintf(stderr, "Error: bad datetime syntax: seperator.\n");
		return NULL;
	}

	hour += (IS_NUMBER(dt_string[11]) ? (dt_string[11] - '0') : 0) * 10;
	hour += (IS_NUMBER(dt_string[12]) ? (dt_string[12] - '0') : 0);

	if(hour >= 0 && hour <= 23) {
		dt->hour = hour;
	}
	else {
		fprintf(stderr, "Error: bad datetime syntax or wrong 'hour' number.\n");
		return NULL;
	}

	if(dt_string[13] != ':') {
		fprintf(stderr, "Error: bad datetime syntax: seperator.\n");
		return NULL;
	}

	minute += (IS_NUMBER(dt_string[14]) ? (dt_string[14] - '0') : 0) * 10;
	minute += (IS_NUMBER(dt_string[15]) ? (dt_string[15] - '0') : 0);

	if(minute >= 0 && minute <= 59) {
		dt->minute = minute;
	}
	else {
		fprintf(stderr, "Error: bad datetime syntax or wrong 'minute' number.\n");
		return NULL;
	}

	if(dt_string[16] != ':') {
		fprintf(stderr, "Error: bad datetime syntax: seperator.\n");
		return NULL;
	}

	second += (IS_NUMBER(dt_string[17]) ? (dt_string[17] - '0') : 0) * 10;
	second += (IS_NUMBER(dt_string[18]) ? (dt_string[18] - '0') : 0);

	if(second >= 0 && second <= 59) {
		dt->second = second;
	}
	else {
		fprintf(stderr, "Error: bad datetime syntax or wrong 'second' number.\n");
		return NULL;
	}

	if(dt_string[19] != '.') {
		fprintf(stderr, "Error: bad datetime syntax: seperator.\n");
		return NULL;
	}

	millisecond += (IS_NUMBER(dt_string[20]) ? (dt_string[20] - '0') : 0) * 100;
	millisecond += (IS_NUMBER(dt_string[21]) ? (dt_string[21] - '0') : 0) * 10;
	millisecond += (IS_NUMBER(dt_string[22]) ? (dt_string[22] - '0') : 0);

	if(millisecond >= 0 && millisecond <= 999) {
		dt->millisecond = millisecond;
	}
	else {
		fprintf(stderr, "Error: bad datetime syntax or wrong 'millisecond' number.\n");
		return NULL;
	}

	return dt; // SUCCESS
}


/**
 * 将from的时间移动shift时区，结果保存在to
 * shift向东为正，向西为负，例如已知北京时间为from，要求当时的朝鲜时间，则shift为+05
 */
Datetime *timezone_shift(Datetime *from, Datetime *to, int8_t shift) {
	int8_t timezone = -shift;
	int8_t timezone_h = timezone / 10;
	int8_t timezone_m = timezone % 10;

	int8_t minute_temp = 0;
	int8_t hour_temp = 0;
	int8_t day_temp = 0;
	int8_t month_temp = 0;
	int16_t year_temp = 0;

	int8_t dom = 0;

	to->timezone += shift;
	to->millisecond = from->millisecond;
	to->second = from->second;

	minute_temp = (int8_t)(from->minute) - timezone_m * 6;

	if(minute_temp < 0) {
		hour_temp = (int8_t)(from->hour) - timezone_h - 1;
		to->minute = 60 + minute_temp;
	}
	else if(minute_temp >= 60) {
		hour_temp = (int8_t)(from->hour) - timezone_h + 1;
		to->minute = minute_temp - 60;
	}
	else {
		hour_temp = (int8_t)(from->hour) - timezone_h;
		to->minute = minute_temp;
	}

	if(hour_temp < 0) {
		to->hour = 24 + hour_temp;
		day_temp = (int8_t)(from->day) - 1;
	}
	else if(hour_temp >= 24) {
		to->hour = hour_temp - 24;
		day_temp = (int8_t)(from->day) + 1;
	}
	else {
		to->hour = hour_temp;
		day_temp = (int8_t)(from->day);
	}

	// precalculated month_temp for getting DOM (Days of month)
	// is leap-year?
	if(IS_LOOP_YEAR(from->year_h * 100 + from->year_l)) {
		days_of_month[2] = 29;
	}
	else {
		days_of_month[2] = 28;
	}
	month_temp = (int8_t)(from->month) + 1;
	if(month_temp > 12) {
		dom = days_of_month[month_temp - 12];
	}
	else {
		dom = days_of_month[month_temp];
	}
	if(day_temp <= 0) {
		month_temp = (int8_t)(from->month) - 1;
		if(month_temp <= 0) {
			dom = days_of_month[12 + month_temp];
		}
		else {
			dom = days_of_month[month_temp];
		}
		to->day = dom + day_temp;
	}
	else if(day_temp > dom) {
		month_temp = (int8_t)(from->month) + 1;
		to->day = day_temp - dom;
	}
	else {
		to->day = day_temp;
		month_temp = (int8_t)(from->month);
	}

	if(month_temp <= 0) {
		to->month = 12 + month_temp;
		year_temp = (int16_t)(from->year_h * 100 + from->year_l) - 1;
	}
	else if(month_temp > 12) {
		to->month = month_temp - 12;
		year_temp = (int16_t)(from->year_h * 100 + from->year_l) + 1;
	}
	else {
		to->month = month_temp;
		year_temp = (int16_t)(from->year_h * 100 + from->year_l);
	}
	to->year_h = year_temp / 100;
	to->year_l = year_temp % 100;

	return to;
}

Datetime *local_to_utc(Datetime *dt, Datetime *utc) {
	return timezone_shift(dt, utc, -(dt->timezone));
}

Datetime *utc_to_local(Datetime *utc, Datetime *local) {
	return timezone_shift(utc, local, local->timezone);
}
#endif
