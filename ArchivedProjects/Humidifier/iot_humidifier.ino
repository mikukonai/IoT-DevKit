/**
 *
 * 智能加湿器 控制程序（Arduino C++）
 * 用于ESP8266（NodeMCU-0.9）
 *
 * Mikukonai 2017.12
 *
 * 2020-02-19 修正了湿度显示错误的问题；强化UI显示效果
 *
 */
#include <string.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "SparkFunBME280.h"
#include "Wire.h"

// 网络参数
const char* ssid = "<SSID>";
const char* password = "<PWD>";
const int port = 80;


// 端口功能定义
const int Motor = 12; // 风机
const int BUZZ = 13;  // 蜂鸣器
const int PWM = 14;   // 雾化控制
const int KS = 15;    // 水位信号输入

// 状态
// 0 - 关机（所有灯灭，风机、雾化均不工作）
// 1 - 雾化（风机工作，雾化工作）
// 2 - 排雾（所有灯灭，风机工作，雾化不工作）
// 3 - 故障（KS触发，低水位，此时电源指示红灯闪烁，风机、雾化均不工作）
// 计划增加的状态
// 4 - 自动（风机工作，雾化跟踪湿度调节功率）
#define SHUTDOWN 0
#define RUNNING  1
#define EXHAUST  2
#define LOWLEVEL 3
#define AUTO     4
static int state = 0;

// 全局传感器数据
static int humidity = 0;
static float temperature = 0.0f;
static float pressure = 0.0f;

// 颜色
#define RED    4
#define GREEN  2
#define BLUE   1
#define PURPLE 5
#define CYAN   3 // Yanhe Green
#define YELLOW 6
#define BLACK  0
#define WHITE  7

// 风机功率（占空比）
//   取值0~1024
static int duty_ratio = 1023;

// ULC循环计数器
static int led_counter = 0;
static int led_color = 1;
#define LED_EXHAUST_PERIOD 1000
#define LED_EXHAUST_DUTY   50
#define LED_SHUTDOWN_PERIOD 10000
#define LED_SHUTDOWN_DUTY   200

// 全局循环计数器
static int loop_count = 0;

ESP8266WebServer server(80);
BME280 sensor;

static char html[5000];
static char tmp[250];

static const char html1[] = "\
<html><head><meta content=\"text/html; charset=utf-8\" http-equiv=\"Content-Type\"/><title>Miku智能加湿器</title><style>.title {display:block;font-size:60px;margin-top:40px;margin-bottom:40px;margin-left: 0px;margin-right: 0px;font-weight: 300;color:#0066ff;}.temp {background:rgba(255,255,255,0);font-size:270px;color:#000;font-weight:100;margin-bottom:70px;}body {background-color:rgb(230, 236, 240);background-size:cover;background-position: right;margin: 0;font-family:Microsoft YaHei,Arial,Helvetica,sans-serif;color:#000;font-size:30px;text-align:center;}.btn{-webkit-appearance:button;display:block;width:50%;height:120px;background:rgba(255,255,255,0.2);color:#0066ff;border: 0px solid #ffffff;line-height:70px;text-align:center;font-weight:200;font-size:60px;text-decoration:none;}.top-panel{position:fixed;height:150px;line-height:150px;font-size:70px;font-weight:300;color:rgba(255,255,255,0.7);width:100%;text-align:center;background:#ffffff;filter:alpha(opacity=50);-moz-opacity:0.5;-khtml-opacity:0.5;opacity:0.5;}.top-panel-text{position:fixed;height:150px;line-height:150px;font-size:50px;color:#000;width:100%;text-align:center;}</style>\
<script language=\"JavaScript\">function switch_status(s){var ttl=document.getElementById(\"status\");if(s==1){ttl.innerHTML=\"正在运行\";}else{ttl.innerHTML=\"待机中\";}}</script></head>\
<body><iframe id=\"hf\" style=\"display:none\"></iframe><div class=\"top-panel\"></div><div class=\"top-panel-text\">Miku智能加湿器</div>\
<div class=\"top-panel-text\"><a href=\"javascript:void(0);\" onclick=\"window.location.go(-1);\" style=\"font-size:50px;color:#0066ff;text-decoration:none;text-align:left; float:left;\">〈 返回</a><a href=\"http://192.168.31.123/iotgw.html\" style=\"font-size:50px;color:#0066ff;text-decoration:none;text-align:right;float:right;\">控制台 〉</a></div>\
<div style=\"display:block;height:200px;\"></div><div id=\"status\" class=\"title\">\
";
static const char html2[] = "\
</div><div class=\"temp\">\
%d\
";
static const char html3[] = "\
<span style=\"font-size:100px;\">%</span></div><div style=\"margin-top:60px;margin-bottom:120px;\"><table align=\"center\" style=\"font-size:60px;color:#000;\"><tr><td style=\"text-align:right;\">气温</td><td> : </td><td>\
";
static const char html4[] = "\
°C</td></tr><tr><td style=\"text-align:right;\">气压</td><td> : </td><td>\
";
static const char html5[] = "\
hPa</td></tr></table></div>\
<button class=\"btn\" style=\"float:left;\" onclick=\"document.getElementById('hf').src = './on';switch_status(1);\">启动</button>\
<button class=\"btn\" style=\"float:right;border-left:1px solid rgba(255,255,255,0.5);\" onclick=\"document.getElementById('hf').src = './off';switch_status(0);\">关闭</button>\
<div style=\"display:block;height:170px;\"></div><div style=\"font-weight:300;margin-top:60px;margin-bottom:10px;\">主控基于 ESP8266 | 加湿器本体 小熊JSQ-A30Y1</div><div style=\"font-weight:300;margin-top:0px;margin-bottom:10px;\">&copy; 2017.12 Mikukonai</div></br></body></html>\
";

// 由于ESP8266 SDK提供的sprintf函数不支持对于浮点数的格式化，所以
// 使用该函数实现%.2f的格式化
void simple_float_to_str(char *str, float f) {
    str[0] = 0;
    int d  = (int)f;
    int d1 = (int)(f * 10) % 10;
    int d2 = (int)(f *100) % 10;
    sprintf(str, "%d", d);
    char temp[4];
    temp[0] = '.';
    temp[1] = d1 + '0';
    temp[2] = d2 + '0';
    temp[3] = '\0';
    strcat(str, temp);
}

// BMP280初始化
void BMP280_setup() {
    sensor.settings.commInterface = I2C_MODE;
    sensor.settings.I2CAddress = 0x76;
    sensor.settings.runMode = 3; //Normal mode
    sensor.settings.tStandby = 0;
    sensor.settings.filter = 0;
    sensor.settings.tempOverSample = 1;
    sensor.settings.pressOverSample = 1;
    sensor.settings.humidOverSample = 1;
    sensor.begin();
}

// 蜂鸣器短鸣一声
void buzzer() {
    digitalWrite(BUZZ, HIGH);
    delay(50);
    digitalWrite(BUZZ, LOW);
}

// 灯光向量控制（目前只输出开关量）
void led_ctrl(int R, int G, int B) {
    unsigned int color = 0;
    color = ((B) << 4) | ((G) << 5) | ((R) << 6);
    Wire.beginTransmission(0x20);
    Wire.write(~color);
    Wire.endTransmission();
}

// 颜色输出
void led_out(int color) {
    led_ctrl(color>>2 & 0x01, color>>1 & 0x01, color & 0x01);
}

// 统一灯光控制 Unified LED Controller
//   使用全局变量如下：
//     state
//     humidity temperature pressure
//     led_counter
//     led_color
//   可以不显式传递参数
void ULC() {
    // EXHAUST状态：言和绿
    if(state == EXHAUST) {
        led_out(CYAN);
    }
    // SHUTDOWN状态：循环闪烁
    else if(state == SHUTDOWN) {
        if(led_counter < (LED_SHUTDOWN_PERIOD - LED_SHUTDOWN_DUTY)) {
            // 关闭全部灯光
            led_out(BLACK);
            led_counter++;
        }
        else if(led_counter >= (LED_SHUTDOWN_PERIOD - LED_SHUTDOWN_DUTY) && led_counter < LED_SHUTDOWN_PERIOD){
            // 闪一下
            led_out(led_color);
            led_counter++;
        }
        else if(led_counter >= LED_SHUTDOWN_PERIOD) {
            // 关闭全部灯光
            led_out(BLACK);
            if(led_color == 7) {
                led_color = 1;
            }
            else {
                led_color++;
            }
            led_counter = 0;
        }
    }
    // RUNNING状态：根据湿度改变灯的颜色
    else if(state == RUNNING) {
        // 夏季气温高于25摄氏度时：
        if(temperature >= 25) {
            // 湿度低：黄灯
            if(humidity > 0 && humidity <= 40) {
                Serial.println("Low Humidity: Yellow");
                led_out(YELLOW);
            }
            // 湿度适宜：绿灯
            else if(humidity > 40 && humidity <= 50) {
                Serial.println("Comfortable Humidity: Green");
                led_out(GREEN);
            }
            // 湿度高：蓝灯
            else if(humidity > 50 && humidity <= 100) {
                Serial.println("High Humidity: Blue");
                led_out(BLUE);
            }
            // 其他情况：红灯（传感器故障）
            else {
                Serial.println("Sensor fault: Red");
                led_out(RED);
            }
        }
        // 春秋冬季气温16~24摄氏度时：
        else if(temperature >= 16 && temperature < 25) {
            // 湿度低：黄灯
            if(humidity > 0 && humidity <= 50) {
                Serial.println("Low Humidity: Yellow");
                led_out(YELLOW);
            }
            // 湿度适宜：绿灯
            else if(humidity > 50 && humidity <= 80) {
                Serial.println("Comfortable Humidity: Green");
                led_out(GREEN);
            }
            // 湿度高：蓝灯
            else if(humidity > 80 && humidity <= 100) {
                Serial.println("High Humidity: Blue");
                led_out(BLUE);
            }
            // 其他情况：红灯（传感器故障）
            else {
                Serial.println("Sensor fault: Red");
                led_out(RED);
            }
        }
        // 气温低于15摄氏度时，湿度已经不重要了
        else if(temperature <= 15) {
            led_out(CYAN);
        }
        // 其他情况：红灯（传感器故障）
        else {
            Serial.println("Sensor fault: Red");
            led_out(RED);
        }
    }
}

// KS中断
void water_level_int() {
    analogWrite(PWM, 0);  // 立即关闭雾化和风机
    digitalWrite(Motor, LOW);
    state = LOWLEVEL;
}

void setup() {
    pinMode(KS, INPUT_PULLUP);
    pinMode(PWM, OUTPUT);
    pinMode(Motor, OUTPUT);
    pinMode(BUZZ, OUTPUT);

    Serial.begin(115200);
    Serial.println("Debug output");

    Wire.begin();
    BMP280_setup();

    // 注册水位过低中断
    attachInterrupt(KS, water_level_int, CHANGE);

    led_counter = 0;

    led_out(BLACK);
    // 灯光全亮
    led_out(WHITE);delay(500);

    state = SHUTDOWN;

    // 网络初始化与连接
    WiFi.begin(ssid, password);
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        led_out(BLACK);
        delay(80);
        led_out(CYAN);
        delay(80);
        Serial.print(".");
    }
    Serial.println ( "" );

    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // 注册响应回调
    server.on("/",[]() {
        buzzer();
        humidity = (int)sensor.readFloatHumidity();
        temperature = sensor.readTempC();
        pressure = sensor.readFloatPressure() / 100.0f;
        Serial.println(humidity);
        Serial.println(temperature);
        Serial.println(pressure);
        strcat(html, html1);
        if(state == RUNNING) {
            strcat(html, "正在运行");
        }
        else {
            strcat(html, "待机中");
        }
        sprintf(tmp, html2, humidity);
        strcat(html, tmp);
        strcat(html, html3);
        simple_float_to_str(tmp, temperature);
        strcat(html, tmp);
        strcat(html, html4);
        simple_float_to_str(tmp, pressure);
        strcat(html, tmp);

        strcat(html, html5);
        Serial.println(html);
        server.send(200, "text/html", html);
        html[0] = '\0';
    });

    server.on("/on",[]() {
        buzzer();
        if(digitalRead(KS) == LOW) {
            duty_ratio = 1023;
            state = RUNNING;
        }
        server.send ( 200, "text/html", "<html><body><h1>I'm running~</h1><p><a href=\"./off\">关机</a></p></body></html>" );
    });

    server.on("/off",[]() {
        buzzer();
        if(state == RUNNING) {
            state = EXHAUST;
        }
        else {
            state = SHUTDOWN;
        }
        server.send ( 200, "text/html", "<html><body><h1>SHUTDOWN~</h1><p><a href=\"./on\">开机</a></p></body></html>" );
        led_out(BLACK);
    });

    server.on("/auto",[]() {
        buzzer();
        if(digitalRead(KS) == LOW) {
            duty_ratio = 1023;
            state = AUTO;
        }
        server.send ( 200, "text/html", "<html><body><h1>I'm running Auto~</h1><p><a href=\"./off\">关机</a></p></body></html>" );
        led_out(BLACK);
    });

    server.onNotFound([]() {
        server.send ( 404, "text/html", "<html><body><h1>Miku IoT Humidifier 404</h1><p>Page '" + server.uri() + "' Not Found.</p></body></html>" );
        buzzer();
    });

    // 启动服务器
    server.begin();

    // 灯光全灭
    led_out(BLACK);
}

void loop() {

    server.handleClient();

    // 每循环10000次，读一次传感器，避免频繁读取可能导致的问题
    // 并且相当于低通滤波
    if(loop_count < 10000) {
        loop_count++;
    }
    else if(loop_count == 10000){
        humidity = (int)sensor.readFloatHumidity();
        temperature = sensor.readTempC();
        pressure = sensor.readFloatPressure() / 100.0f;
        loop_count = 0;
    }

    // 状态机
    if(state == LOWLEVEL) {
        Serial.println( "state LOWLVL" );
        // 显式关闭雾化和风机
        analogWrite(PWM, 0);
        digitalWrite(Motor, LOW);
        // 先关闭所有灯光
        led_out(BLACK);
        // 堵塞式闪烁红灯10次
        for(int i = 0; i < 10; i++) {
            led_out(BLACK);
            delay(200);
            led_out(RED);
            delay(200);
        }
        // 闪完之后关机
        state = SHUTDOWN;
    }
    else if(state == EXHAUST) {
        Serial.println( "state EXHAUST" );
        // 关闭雾化
        analogWrite(PWM, 0);
        // 显式启动风机
        digitalWrite(Motor, HIGH);
        // 开灯
        ULC();
        delay(5000); // 延时5秒，堵塞也无妨
        // 关闭风机
        digitalWrite(Motor, LOW);
        // 转SHUTDOWN状态
        state = SHUTDOWN;
    }
    else if(state == SHUTDOWN) {
        // 显式关闭风机
        digitalWrite(Motor, LOW);
        // 显式关闭雾化
        analogWrite(PWM,0);
        // 开灯
        ULC();
        // 维持SHUTDOWN状态
        state = SHUTDOWN;
    }
    else if(state == RUNNING) {
        if(digitalRead(KS) == LOW) {
            duty_ratio = 1023;
            // 写雾化占空比
            analogWrite(PWM, duty_ratio);
            // 显式开启风机
            digitalWrite(Motor, HIGH);
        }
        else {
            // 关闭雾化
            analogWrite(PWM, 0);
            // 显式关闭风机
            digitalWrite(Motor, LOW);
            state == LOWLEVEL;
        }
        // 开灯
        ULC();
        // 维持RUNNING状态
        state = RUNNING;
    }
    else if(state == AUTO) {
        if(digitalRead(KS) == LOW) {
            if(humidity >= 75) {
                // 写雾化占空比
                analogWrite(PWM, 0);
                // 不关闭风机
            }
            else {
                duty_ratio = 1023;
                // 写雾化占空比
                analogWrite(PWM, duty_ratio);
                // 显式开启风机
                digitalWrite(Motor, HIGH);
            }
        }
        else {
            // 关闭雾化
            analogWrite(PWM, 0);
            // 显式关闭风机
            digitalWrite(Motor, LOW);
            state == LOWLEVEL;
        }
        // 开灯
        ULC();
        // 维持AUTO状态
        state = AUTO;
    }
}