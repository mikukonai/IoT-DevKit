#!/usr/bin/env python
# -*- coding: utf-8 -*-

import commands
import time
import datetime

def loop():
	while True:
		cst = datetime.datetime.now()
		delta = datetime.timedelta(hours = 8, minutes = 20)
		gmt = cst - delta

		cst_time = cst.strftime('%Y%m%d%H%M')
		gmt_time = gmt.strftime('%Y%m%d%H%M')

		gmt_date = gmt_time[0:8]
		gmt_hour = int(gmt_time[8:10])
		cst_hour = int(cst_time[8:10])
		gmt_minute = int(gmt_time[10:12])

		gmt_minute = ((gmt_minute * 10) // 60) * 6

		img_timestamp = gmt_date + str(gmt_hour).zfill(2) + str(gmt_minute).zfill(2) + '00000'
		img_url = 'http://i.weather.com.cn/i/product/pic/l/sevp_aoc_rdcp_sldas_ebref_az9250_l88_pi_' + img_timestamp + '.png'

		cmd_line = 'wget -P ~/tq ' + img_url
		print(cmd_line)
		commands.getoutput(cmd_line)

		cmd_line = 'mv ~/tq/sevp_aoc_rdcp_sldas_ebref_az9250_l88_pi_' + img_timestamp + '.png' + ' ' + cst_time[0:10] + str(gmt_minute).zfill(2) + '.png'
		print(cmd_line)
		commands.getoutput(cmd_line)

		print("南京 雷达图 " + cst_time[0:10] + str(gmt_minute).zfill(2) + '.png' + " 已保存")

		time.sleep(300)

if __name__ == '__main__':
	loop()
