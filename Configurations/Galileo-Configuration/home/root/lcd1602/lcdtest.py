#!/usr/bin/env python
# -*- coding: utf-8 -*-

import time
import commands

def disp(s):
	f = open('/home/root/lcd1602/screen.txt','w')
	f.write(s)
	f.close()
	commands.getoutput('/home/root/lcd1602/i2c1602')

#     0123456789ABCDEF  0123456789ABCDEF
disp("Hello Galileo!  \n123°C           ")

