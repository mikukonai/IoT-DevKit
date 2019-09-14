#ifndef __MK_NMEA_PARSER__
#define __MK_NMEA_PARSER__

#define IS_NUMBER(x) ((x) >= '0' && (x) <= '9')
#define IS_DOT(x) ((x)=='.')
#define IS_DECIMAL(x) (IS_NUMBER(x) || IS_DOT(x))
#define IS_ALPHABET(x) (((x) >= 'a' && (x) <= 'z') || ((x) >= 'A' && (x) <= 'Z'))
#define IS_CHAR(x) (IS_NUMBER(x) || IS_ALPHABET(x))
#define IS_COMMA(x) ((x)==',')
#define IS_DOLLAR(x) ((x)=='$')
#define IS_STAR(x) ((x)=='x')
#define IS_CRLF(x) (((x)=='\n')||((x)=='\r'))

#define MAX_FIELD_NUM 30

#define uint8_t unsigned char
#define uint16_t unsigned short
#define int8_t char
#define int16_t short

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datetime.h"
#include "geo_coordinate.h"

#define NMEA_GPGGA 1
#define NMEA_GPGLL 2
#define NMEA_GPRMC 3
#define NMEA_GPVTG 4

#define NMEA_VOID		50
#define NMEA_LAT_NUM	52
#define NMEA_LAT_ORI	53
#define NMEA_LON_NUM	54
#define NMEA_LON_ORI	55
#define NMEA_GPS_STATE	56
#define NMEA_SAT_NUM	57
#define NMEA_ALT		58
#define NMEA_ALT_UNT	59
#define NMEA_ALT_DIF	60
#define NMEA_DATA_STATE	61
#define NMEA_VEL		62
#define NMEA_ORI		63
#define NMEA_DECL_ANG	64
#define NMEA_DECL_ORI	65
#define NMEA_UTC_TIME	66
#define NMEA_UTC_DATE	67
#define NMEA_CHECKSUM	99

static uint8_t PROTOCOL[20][MAX_FIELD_NUM] = {
	{NMEA_VOID},
	{NMEA_GPGGA, NMEA_UTC_TIME, NMEA_LAT_NUM, NMEA_LAT_ORI, NMEA_LON_NUM, NMEA_LON_ORI, NMEA_GPS_STATE, NMEA_SAT_NUM, NMEA_VOID, NMEA_ALT, NMEA_VOID, NMEA_VOID, NMEA_VOID, NMEA_VOID, NMEA_VOID, NMEA_CHECKSUM},
	{NMEA_GPGLL, NMEA_LAT_NUM, NMEA_LAT_ORI, NMEA_LON_NUM, NMEA_LON_ORI, NMEA_UTC_TIME, NMEA_DATA_STATE, NMEA_CHECKSUM},
	{NMEA_GPRMC, NMEA_UTC_TIME, NMEA_DATA_STATE, NMEA_LAT_NUM, NMEA_LAT_ORI, NMEA_LON_NUM, NMEA_LON_ORI, NMEA_VEL, NMEA_ORI, NMEA_UTC_DATE, NMEA_DECL_ANG, NMEA_DECL_ORI, NMEA_CHECKSUM}, //NMEA_GPRMC
	{NMEA_GPVTG, NMEA_ORI, NMEA_VOID, NMEA_DECL_ANG, NMEA_VOID, NMEA_VOID, NMEA_VOID, NMEA_VOID, NMEA_VOID, NMEA_CHECKSUM}
};

// 计算校验和：对$和*之间的字符做异或
//   输入：语句行line
//   输出：校验和（一个字节）
uint8_t nmea_checksum(char *line) {
	uint8_t sum = 0;
	uint8_t tmp = 0;
	uint16_t len = strlen(line);
	uint16_t index = 1;
	uint16_t csindex = 0;

	for( ;index < len; index++) {
		if(line[index] == '*') {
			csindex = index + 1;
			break;
		}
		sum ^= (uint8_t)(line[index]);
	}
	printf("Calculated Checksum: 0x%x\n", sum);
	tmp += (IS_ALPHABET(line[ csindex ])) ? ((line[csindex]-'A'+10) * 16) : ((line[csindex]-'0') * 16);
	tmp += (IS_ALPHABET(line[csindex+1])) ? ((line[csindex+1]-'A'+10)) : ((line[csindex+1]-'0'));
	printf("Transferred Checksum: 0x%x\n", tmp);
	if(tmp != sum) {
		fprintf(stderr, "Checksum error.\n");
		return 0;
	}
	else {
		return 1;
	}
}

// 扫描一遍，把所有字段的逗号位置提取出来
//   输入：语句行line
//   输出：字段起点位置表field_index
void nmea_get_field_index(char *line, uint16_t *field_index) {
	uint16_t len = strlen(line);
	uint8_t  fcount = 0;
	uint16_t index = 0;

	for( ; index < len; index++) {
		if(line[index] == '$' || line[index] == ',' || line[index] == '*') {
			field_index[fcount] = index;
			fcount++;
		}
	}
	field_index[fcount] = len - 2; //最后一项定为字符串长度-2（因为以crlf结尾），以便提取最后一项
}

// 取字段内容
char *nmea_get_field(char *line, char *field, uint16_t *field_index, uint16_t fnum) {
	uint16_t findex1 = field_index[fnum] + 1;
	uint16_t findex2 = field_index[fnum + 1] - 1;
	uint16_t count = 0;
	uint16_t i = findex1;
	if(findex1 <= findex2) {
		for(; i <= findex2; i++) {
			field[count++] = line[i];
		}
	}
	else {
		field[0] = '\0';
		return field;
	}
	field[count] = '\0';
	return field;
}

void nmea_lat_parser(char *field, GeoCoordinate *gc) {
	// TODO 语法检查
	gc->lat_deg = (field[0]-'0') * 10 + (field[1]-'0');
	gc->lat_min = (field[2]-'0') * 10 + (field[3]-'0');
	gc->lat_sec = (field[5]-'0') * 10 + (field[6]-'0');
	//gc->lat_sec = (uint8_t)(60.0f * ((float)(field[5]-'0') / 10.0f + (float)(field[6]-'0') / 100.0f + (float)(field[7]-'0') / 1000.0f + (float)(field[8]-'0') / 10000.0f));
}

void nmea_lon_parser(char *field, GeoCoordinate *gc) {
	// TODO 语法检查
	gc->lon_deg = (field[0]-'0') * 100 + (field[1]-'0') * 10 + (field[2]-'0');
	gc->lon_min = (field[3]-'0') * 10 + (field[4]-'0');
	gc->lon_sec = (field[6]-'0') * 10 + (field[7]-'0');
	//gc->lon_sec = (uint8_t)(60.0f * ((float)(field[6]-'0') / 10.0f + (float)(field[7]-'0') / 100.0f + (float)(field[8]-'0') / 1000.0f + (float)(field[9]-'0') / 10000.0f));
}

void nmea_altitude_parser(char *field, GeoCoordinate *gc) {
	// TODO 语法检查
	gc->altitude = (float)atof(field);
}

void nmea_velocity_parser(char *field, GeoCoordinate *gc) {
	// TODO 语法检查
	gc->velocity = (float)atof(field);
}

void nmea_orientation_parser(char *field, GeoCoordinate *gc) {
	// TODO 语法检查
	gc->orientation = (float)atof(field);
}

void nmea_utctime_parser(char *field, Datetime *dt) {
	// TODO 语法检查
	dt->hour = (field[0]-'0') * 10 + (field[1]-'0');
	dt->minute = (field[2]-'0') * 10 + (field[3]-'0');
	dt->second = (field[4]-'0') * 10 + (field[5]-'0');
	// dt->millisecond = (field[7]-'0') * 100 + (field[8]-'0') * 10 + (field[9]-'0');
	dt->millisecond = (field[7]-'0') * 100 + (field[8]-'0') * 10;
}
void nmea_utcdate_parser(char *field, Datetime *dt) {
	// TODO 语法检查
	dt->day = (field[0]-'0') * 10 + (field[1]-'0');
	dt->month = (field[2]-'0') * 10 + (field[3]-'0');
	dt->year_l = (field[4]-'0') * 10 + (field[5]-'0');
	if(dt->year_l >= 50) {
		dt->year_h = 19;
	}
	else {
		dt->year_h = 20;
	}
}

// 扫描全部字段，解析整个语句，并填写Datetime和GeoCoord两个结构体
int8_t nmea_parser(char *line, Datetime *dt, GeoCoordinate *gc) {
	uint16_t *field_index = (uint16_t*)malloc(sizeof(uint16_t) * MAX_FIELD_NUM);
	uint8_t  stmt_type = 0;
	uint8_t  field_type = 0;
	uint16_t index = 0;
	uint16_t fcount = 0;
	char field[20];

	// 构造字段索引表
	nmea_get_field_index(line, field_index);

	// 获取语句类型（索引表第0字段）
	nmea_get_field(line, field, field_index, 0);
	if(!strcmp("GPGGA", field)) {
		stmt_type = NMEA_GPGGA;
	}
	else if(!strcmp("GPGLL", field)) {
		stmt_type = NMEA_GPGLL;
	}
	else if(!strcmp("GPRMC", field)) {
		stmt_type = NMEA_GPRMC;
	}
	else if(!strcmp("GPVTG", field)) {
		stmt_type = NMEA_GPVTG;
	}
	else {
		stmt_type = NMEA_VOID;
		// fprintf(stderr, "Parser error: unknown statement.\n");
		return -1;
	}

	// 检查校验和
	if(!nmea_checksum(line)) {
		printf("Parser error: Checksum error.\n");
		return -1;
	}

	// 清理字段缓冲
	memset(field, '\0', 20);

	// 遍历并解释校验和之前的每一字段
	do {
		// 获取第fcount个字段的类型
		field_type = PROTOCOL[stmt_type][fcount];
		// 获取第fcount个字段的实际内容
		nmea_get_field(line, field, field_index, fcount);
		// 根据字段类型作出解释
		switch(field_type) {
		// 纬度及方向
		case NMEA_LAT_NUM:
			nmea_lat_parser(field, gc);
			break;
		case NMEA_LAT_ORI:
			if(!strcmp("N", field)) {
				gc->lat_deg = abs(gc->lat_deg);
			}
			else if(!strcmp("S", field)) {
				gc->lat_deg = -abs(gc->lat_deg);
			}
			else {
				// fprintf(stderr, "Parser warning: unknown NMEA_LAT_ORI.\n");
			}
			break;
		// 经度及方向
		case NMEA_LON_NUM:
			nmea_lon_parser(field, gc);
			break;
		case NMEA_LON_ORI:
			if(!strcmp("E", field)) {
				gc->lon_deg = abs(gc->lon_deg);
			}
			else if(!strcmp("W", field)) {
				gc->lon_deg = -abs(gc->lon_deg);
			}
			else {
				// fprintf(stderr, "Parser warning: unknown NMEA_LON_ORI.\n");
			}
			break;
		// 海拔
		case NMEA_ALT:
			nmea_altitude_parser(field, gc);
			break;
		// UTC时间和日期
		case NMEA_UTC_TIME:
			nmea_utctime_parser(field, dt);
			break;
		case NMEA_UTC_DATE:
			nmea_utcdate_parser(field, dt);
			break;
		// 航向和航速
		case NMEA_VEL:
			nmea_velocity_parser(field, gc);
			break;
		case NMEA_ORI:
			nmea_orientation_parser(field, gc);
			break;
		case NMEA_DATA_STATE:
			if(!strcmp("V", field)) {
				gc->data_state = 'V';
			}
			else if(!strcmp("A", field)) {
				gc->data_state = 'A';
			}
			else {
				// fprintf(stderr, "Parser warning: unknown NMEA_DATA_STATE.\n");
			}
			break;
		case NMEA_GPS_STATE:
			if(!strcmp("0", field)) {
				gc->gps_state = '0';
			}
			else if(!strcmp("1", field)) {
				gc->gps_state = '1';
			}
			else if(!strcmp("2", field)) {
				gc->gps_state = '2';
			}
			else if(!strcmp("3", field)) {
				gc->gps_state = '3';
			}
			else {
				// fprintf(stderr, "Parser warning: unknown NMEA_GPS_STATE.\n");
			}
			break;
		default:
			break;
		}

		// 清理字段缓冲
		memset(field, '\0', 20);
		// 修改遍历计数器
		fcount++;
	}
	while(field_type != NMEA_CHECKSUM);


}


#endif