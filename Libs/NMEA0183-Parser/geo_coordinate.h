#ifndef __MK_GEO_COORD__
#define __MK_GEO_COORD__

#define uint8_t unsigned char
#define uint16_t unsigned short
#define int8_t char
#define int16_t short

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int8_t  lon_deg; // E+ W-
	uint8_t lon_min;
	uint8_t lon_sec;
	int8_t  lat_deg; // N+ S-
	uint8_t lat_min;
	uint8_t lat_sec;
	float   altitude;
	float   velocity;
	float   orientation;
	float   declination_angle;
	float   declination_side;
	char    gps_state;
	char    data_state;
}GeoCoordinate;


#endif
