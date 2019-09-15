# 家庭物联网的HTTP接口设计

## URI格式

**格式**

```
http://miku.iot/场景/子场景/设备名称
```

例如

```
http://miku.iot/home/default/humidifier
http://miku.iot/home/bedroom/aircond
http://miku.iot/company/default/humidifier
http://miku.iot/car/default/player
```


- 在路由器设置网关的url为`miku.iot`，
- 在网关侧处理http请求：首先根据请求uri，定位对应设备的ip，
- 然后根据http请求类型，运行相应的控制逻辑，发送相应的控制请求。

> 注意：这里的“网关”指的是逻辑网关，即物联网网关。路由器是真正的物理网络的网关。物联网网关采用Intel Galileo或者树莓派，物联网终端目前采用ESP8266。

## 请求类型

**GET**

用于获取设备状态。（MIME：application/json）

例如温度计：
```
{
    "status" : "running",
    "temperature" : "16.7℃",
    "humidity" : "75%RH"
}
```

**POST**

对设备进行控制。

参数如下：

- switch（0、1）：用来控制设备开关  
- value（0~255）：标准的模拟量
