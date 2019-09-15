#include <stdio.h>
#include <stdlib.h>
#include "datetime.h"
#include "geo_coordinate.h"
#include "nmea_parser.h"

#define GPS_SERIAL_DEVFILE "/dev/ttyS0"

int main(void) {
	FILE* fd;
	char cc = 0; // Current_Char
	int count = 0;
	char line[200];
	Datetime *dt = (Datetime*)malloc(sizeof(Datetime));
	Datetime *local = (Datetime*)malloc(sizeof(Datetime));
	GeoCoordinate *gc = (GeoCoordinate*)malloc(sizeof(GeoCoordinate));
	char str[100];

	int lf_flag = 0;

	// open serial port
	if((fd = fopen (GPS_SERIAL_DEVFILE, "r")) < 0) {
		printf("Serial port opening error.\n");
	}
	while(1)
	{
		cc = fgetc(fd); // read one character
		//printf("%c", cc);
		// Begin of one NMEA frame
		if(cc == '$') {
			count = 0;
			line[count++] = cc;
		}
		// End of one NMEA frame
		else if(cc == '\n') {

			if(lf_flag == 1) {
				lf_flag = 0;
				continue;
			}

			lf_flag = 1;

			line[count++] = '\r';
			line[count++] = '\n';
			line[ count ] = '\0';
			count = 0;

			if( nmea_parser(line, dt, gc) < 0) {
				continue;
			}

			printf("NMEA消息帧：%s", line);

			datetime_to_string(dt, str);
			printf("协调世界时：%s\n", str);
			local->timezone = +80;
			utc_to_local(dt, local);
			datetime_to_string(local, str);
			printf("北京时：%s\n", str);

			if(gc->gps_state == '0' || gc->data_state == 'V') {
				printf("未定位/定位数据无效\n");
			}
			else {
				printf("纬度：%d°%d′%d″\n经度：%d°%d′%d″\n航速：%f knots\n航向：%f°\n",
					gc->lat_deg,
					gc->lat_min,
					gc->lat_sec,
					gc->lon_deg,
					gc->lon_min,
					gc->lon_sec,
					gc->velocity,
					gc->orientation);
			}
		} // LF1 end
			
			
		// else if(cc == '\n') {
		// 	printf("\nThis is LF\n");
		// 	count = 0;
		// 	continue;
		// }
		else {
			line[count++] = cc;
		}
	}
	return 0;
}
