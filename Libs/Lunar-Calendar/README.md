# Lunar-Calendar-API

API for calculating Chinese lunar date.
农历计算API。

测试用代码为`./test.c`，使用UTF-8编码。使用或移植时，请注意对汉字进行适当处理。

# 算法说明

基于查表法的农历计算API。

给出格里历年月日，即可计算出对应的农历年月日。所谓农历年实际上是和公历一致的。

计算范围为格里历2000年1月1日开始，到2049年12月31日。

并不能通过农历年月日反查格里历年月日。

# 权利声明

本API是根据多年前在MSP430单片机上使用的代码整理而来，农历数据表格源于网上资料，但算法为本人手写。

由于核心的农历数据表格的来源无从查证，因此本人并不保留任何权利。

本API为个人出于兴趣爱好用于业余电子制作之用，不保证正确性和可靠性。
