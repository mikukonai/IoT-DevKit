#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "SparkFunBME280.h"
#include <stdint.h>
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "DS3231.h"

#define BUILTIN_LED 16

const char* ssid = "<SSID>";
const char* password = "<PW>";
const int port = 80;

// 12小时制？
bool is12h = false;
bool century = false;
bool PM;

// 全局传感器数据
static int humidity = 0;
static float temperature = 0.0f;
static float pressure = 0.0f;

// 全局时间
static int second = 0;
static int minute = 0;
static int hour = 0;
static int date = 0;
static int month = 0;
static int year = 0;


#define ADDRESS_BH1750FVI 0x23
#define ONE_TIME_H_RESOLUTION_MODE 0x20
byte highByte = 0;
byte lowByte = 0;
unsigned int sensorOut = 0;
unsigned int illuminance = 0;

ESP8266WebServer server(80);

BME280 sensor;
DS3231 Clock;

// 字符串拼接相关
long lastMsg = 0;
static char msg[250];
static char tmp[250];
static char html[5000];
static const char header[] = "<html><head><meta content=\"text/html; charset=utf-8\" http-equiv=\"Content-Type\"/></head>";


int readBH1750() {
    Wire.beginTransmission(ADDRESS_BH1750FVI); //"notify" the matching device
    Wire.write(ONE_TIME_H_RESOLUTION_MODE);     //set operation mode
    Wire.endTransmission();

    delay(180);

    Wire.requestFrom(ADDRESS_BH1750FVI, 2); //ask Arduino to read back 2 bytes from the sensor
    highByte = Wire.read();  // get the high byte
    lowByte = Wire.read(); // get the low byte

    sensorOut = (highByte<<8)|lowByte;
    illuminance = sensorOut/1.2;
    return illuminance;
}

void readDS3231() {
    second = Clock.getSecond();
    minute = Clock.getMinute();
    hour = Clock.getHour(is12h, PM);
    date = Clock.getDate();
    month = Clock.getMonth(century);
    year = Clock.getYear();
}
void setDS3231(int YEAR, int MONTH, int DAY, int HOUR, int MINUTE, int SECOND) {
    Clock.setClockMode(false);
    Clock.setSecond(SECOND);//Set the second 
    Clock.setMinute(MINUTE);//Set the minute 
    Clock.setHour(HOUR);  //Set the hour 
//    Clock.setDoW(6);    //Set the day of the week
    Clock.setDate(DAY);  //Set the date of the month
    Clock.setMonth(MONTH);  //Set the month of the year
    Clock.setYear(YEAR);  //Set the year (Last two digits of the year)
}


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

void blink(int time) {
    digitalWrite(BUILTIN_LED, LOW);
    delay(time);
    digitalWrite(BUILTIN_LED, HIGH);
}

void setup() {
    Wire.begin(0, 2);
    BMP280_setup();
    pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
    Serial.begin(115200);

    // 网络初始化与连接
    WiFi.begin(ssid, password);
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        blink(50);
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
        blink(10);

        // 读取BME280
        humidity = (int)sensor.readFloatHumidity();
        temperature = sensor.readTempC();
        pressure = sensor.readFloatPressure() / 100.0f;
        // 读取BH1750
//        illuminance = readBH1750();
        // 读取时间
        readDS3231();

        strcat(html, "{");

        strcat(html, "\"datetime\":");
        sprintf(tmp, "\"20%02d-%02d-%02d %02d:%02d:%02d\"", year, month, date, hour, minute, second);
        strcat(html, tmp);
        strcat(html, ",");
        
        strcat(html, "\"humidity\":");
        sprintf(tmp, "\"%d\"", humidity);
        strcat(html, tmp);
        strcat(html, ",");

        strcat(html, "\"temperature\":\"");
        simple_float_to_str(tmp, temperature);
        strcat(html, tmp);
        strcat(html, "\",");


        strcat(html, "\"pressure\":\"");
        simple_float_to_str(tmp, pressure);
        strcat(html, tmp);
        strcat(html, "\"");

//      strcat(html, "\"illuminance\":");
//      sprintf(tmp, "\"%d\"", illuminance);
//      strcat(html, tmp);
//      strcat(html, "\",");

        strcat(html, "}");
        
        Serial.println(html);
        
        server.send(200, "application/json", html);
        html[0] = '\0';

        blink(10);
    });

    server.onNotFound([]() {
        server.send(404, "text/html", "<html><body><h1>Miku IoT Barometer 404</h1><p>Invalid Request '" + server.uri() + "'</p></body></html>" );
        blink(10); delay(100);
        blink(10); delay(100);
        blink(10); delay(100);
        blink(10); delay(100);
        blink(10);
    });

    // 启动服务器
    server.begin();
}

static int timeString[20];
static int timeStringCount = 0;

void loop() {
    // 2018.10.20 注释
    // 串口调试输入"yymmddHHMMSS回车"调整时间
    if (Serial.available() > 0) {
        readDS3231();
        byte b = Serial.read();
        if(b >= '0' && b <= '9') {
            timeString[timeStringCount] = (int)((char)b - '0');
            timeStringCount++;
        }
        else if(b == 13) {
            timeStringCount = 0;
            int YEAR   = 10 * timeString[0] + timeString[1];
            int MONTH  = 10 * timeString[2] + timeString[3];
            int DAY    = 10 * timeString[4] + timeString[5];
            int HOUR   = 10 * timeString[6] + timeString[7];
            int MINUTE = 10 * timeString[8] + timeString[9];
            int SECOND = 10 * timeString[10] + timeString[11];
            setDS3231(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND);
            Serial.print("Time set!");
        }
        else {
            timeStringCount = 0;
            Serial.print("Bad time setting string.");
        }
    }
    server.handleClient();
     long now = millis();
     if (now - lastMsg > 1000) {
         lastMsg = now;
         blink(5);
     }
}
