# 家庭物联网的架构设计

## 网络拓扑与配置

路由器A（TP-LINK），作为一级路由器。路由器B（小米），作为二级路由器。所有的物联网设备均挂载到路由器B。

各房间的上网网口，均连接到路由器A。平时上网，使用路由器A提供的Wi-Fi信号。

路由器B设定静态IP分配，所有物联网设备的IP地址应该固定不变。

物联网网关（Galileo）同其他终端一样挂载到路由器B，其IP地址固定，与其他终端共同位于二级子网。

路由器B设定Galileo为DMZ主机。

这样，访问路由器B所辖二级子网的任何一个IP地址，都将定向到Galileo。这正是它作为网关的作用。

## 网关设置

二级子网的IP地址如下：

```
子网地址：
192.168.31.0/24

网关（路由器B）：
192.168.31.1

物联网网关（Galileo）：
192.168.31.100

智能加湿器：
192.168.31.14
```

Galileo使用Node.js作为服务器软件。

例如，下列程序可以通过GET方法实现对加湿器的控制：

```JavaScript
var http = require('http');
var url = require('url');

http.createServer( function (request, response) {
   var params = url.parse(request.url, true).query;
   var uri = "";
   response.writeHead(200, {'Content-Type': 'text/html; charset=utf-8'});
   if(params.device == "humidifier") {
      uri = "http://192.168.31.14";
      if(params.command == "on") {
         http.get(uri + "/on");
         console.log('on');
      }
      else if(params.command == "off") {
         http.get(uri + "/off");
         console.log('off');
      }
      else {
         response.end("<html><h1>Bad command.</h1></html>");
      }
   }
   else {
      response.end("<html><h1>No such device.</h1></html>");
   }
   response.end("<html><h1>Done.</h1></html>");
}).listen(8080);

console.log('IoT Gateway Server running.');
```

当然，使用GET方法控制设备是不Restful的，这里只是为了说明系统框架而已。

该Node.js程序同时完成以下三个任务：

1. 封装各个物联网设备各自的接口，并提供统一的访问控制和权限检查
2. 联合控制多个设备，实现较为复杂的家居自动化逻辑
3. 作为HTTP服务器，提供友好的前端用户界面

## 使用场景描述

处于家庭内网的设备，可以通过访问192.168.31.x或者路由器B的WAN地址，访问由Galileo提供的控制页面，即可控制所有的处在二级子网的物联网终端。

暂不考虑搭建内网DNS。
