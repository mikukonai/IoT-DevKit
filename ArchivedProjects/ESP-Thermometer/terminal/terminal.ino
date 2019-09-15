#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "Wire.h"
#include "LiquidCrystal_I2C.h"

#define BUILTIN_LED 16

LiquidCrystal_I2C lcd(0x3f,16,2);

const char *ssid = "<SSID>";
const char *password = "<PW>";

const char *host = "192.168.31.17";
const int httpsPort = 80;
const String url = "";

void blink(int time) {
    digitalWrite(BUILTIN_LED, LOW);
    delay(time);
    digitalWrite(BUILTIN_LED, HIGH);
}

String IpAddress2String(const IPAddress& ipAddress)
{
    return String(ipAddress[0]) + String(".") +\
    String(ipAddress[1]) + String(".") +\
    String(ipAddress[2]) + String(".") +\
    String(ipAddress[3])  ; 
}

void setup()
{
    Wire.begin(0, 2);
    pinMode(BUILTIN_LED, OUTPUT);

    lcd.init();
    lcd.backlight();
    lcd.clear();
    Serial.begin(115200);

    // 网络初始化与连接
    WiFi.begin(ssid, password);
    Serial.println();
    Serial.print("Connecting");
    lcd.printstr("Connecting...");
    while (WiFi.status() != WL_CONNECTED) {
        blink(50);
        delay(80);
        Serial.print(".");
    }

    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    lcd.setCursor(0, 1);
    lcd.printstr(IpAddress2String(WiFi.localIP()).c_str());
    delay(500);
}

void loop() {
    WiFiClient client;

    if (!client.connect(host, httpsPort)) {
        return;
    }
    delay(5);

    String postRequest = (String)("GET ") + url + "/ HTTP/1.1\r\n" +
                         "Content-Type: text/html;charset=utf-8\r\n" +
                         "Host: " + host + "\r\n" +
                         "User-Agent: NodeMCU_Terminal\r\n" +
                         "Connection: Keep Alive\r\n\r\n";
    client.print(postRequest); // 发送HTTP请求

    String line = client.readStringUntil('\n');
    String response = "";
    blink(50);
    while (line.length() != 0) {
        line = client.readStringUntil('\n');
        response += line;
    }
    client.stop();

    int quoteCount = 0;
    String datetime_str = "";
    String humidity_str = "";
    String temperature_str = "";
    String pressure_str = "";
    String illuminance_str = "";
    int fieldFlag = 0;
    int jsonFlag = 0;

    for(int i = 0; i < response.length(); i++) {
        if(response[i] == '{') {
            jsonFlag = 1;
        }
        if(jsonFlag > 0 && response[i] == '"') {
            quoteCount++;
        }
        else {
            if(fieldFlag == 1) {
                datetime_str += String(response[i]);
            }
            else if(fieldFlag == 2) {
                humidity_str += String(response[i]);
            }
            else if(fieldFlag == 3) {
                temperature_str += String(response[i]);
            }
            else if(fieldFlag == 4) {
                pressure_str += String(response[i]);
            }
            else if(fieldFlag == 5) {
                illuminance_str += String(response[i]);
            }
        }
        if(quoteCount == 3) {
            fieldFlag = 1;  // datetime
        }
        else if(quoteCount == 7) {
            fieldFlag = 2;  // humidity
        }
        else if(quoteCount == 11) {
            fieldFlag = 3;  // temperature
        }
        else if(quoteCount == 15) {
            fieldFlag = 4;  // pressure
        }
        else if(quoteCount == 19) {
            fieldFlag = 5;  // illum
        }
        else {
            fieldFlag = 0;
        }
    }

    lcd.clear();
    for (int c = 0; c < temperature_str.length(); c++){
        lcd.write(temperature_str[c]);
    }
    lcd.write(0xdf);
    lcd.write('C');
    lcd.write(' ');
    lcd.write(' ');
    lcd.write(' ');
    lcd.write(' ');
    
    for (int c = 0; c < humidity_str.length(); c++){
        lcd.write(humidity_str[c]);
    }
    lcd.write('%');
    lcd.write('R');
    lcd.write('H');
  
    lcd.setCursor(0, 1);

    // 气压：只精确到小数点后一位
    for (int c = 0; c < 6; c++){
        lcd.write(pressure_str[c]);
    }
    lcd.write('h');
    lcd.write('P');
    lcd.write('a');
    lcd.write(' ');
    lcd.write(' ');
    
    // 时间：只显示分：秒，对应datetime_str的11-12，14-15
    lcd.write(datetime_str[11]);
    lcd.write(datetime_str[12]);
    lcd.write(':');
    lcd.write(datetime_str[14]);
    lcd.write(datetime_str[15]);

    delay(2000);
}
