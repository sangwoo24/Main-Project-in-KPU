#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SimpleTimer.h>
#include <Keypad.h>
#include <DHT.h>

//---------------------------------------------------------------------------------
const char* ssid = "Sangwoo"; // 와이파이 이름
const char* password = "56962353"; // 와이파이 비밀번호
#define mqtt_server "tailor.cloudmqtt.com" // MQTT server 주소
#define mqtt_port 16420 // port 번호
#define mqtt_topic "SSW" // topic (자유롭게 작성}
#define mqtt_user "tgiqgumo" // username
#define mqtt_password "vtJiGGDTNy8J" // password

WiFiClient espClient;
PubSubClient client(espClient);

//---------------------------------------------------------------------------------

//[temp & humi] setting-----------------------------
#define         DHTTYPE DHT22
#define         DHTPIN 14
DHT             dht(DHTPIN, DHTTYPE, 11);
char            dht_buffer[10];
float           humidity;
float           temp_f;
unsigned long   lastDHTReadMillis = 0;
#define         interval 2000
//--------------------------------------------------

SimpleTimer timer;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
//Keypad Setting--------------------------------------------------------------
const byte n_rows = 2;
const byte n_cols = 4;
byte colPins[n_cols] = {0, 13, 12, 15};
byte rowPins[n_rows] = { 2, 16 };
char keys[n_rows][n_cols] = {{'1','2','3','4'},{'5'}};
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, n_rows, n_cols);
//----------------------------------------------------------------------------

int i = 0;
int flag = 0;
int break_flag = 0,break_flag_walk = 0;
int food = 5,age = 0,feel = 1;
String feeling = "good!",Name = "Sang";
int die_flag = 0;


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// 'start화면', 128x32px
const unsigned char main_map [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xff, 0x81, 0x00, 0x7f, 0xe1, 0x00, 0xff, 0xc0, 0xff, 0xe0, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x80, 0x01, 0x00, 0x40, 0x21, 0x00, 0x00, 0x40, 0x00, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x80, 0x01, 0x00, 0x40, 0x21, 0x00, 0x00, 0x40, 0x01, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x80, 0x01, 0x00, 0x40, 0x21, 0x00, 0x00, 0x40, 0x03, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x80, 0x01, 0x00, 0x40, 0x21, 0x00, 0x00, 0x40, 0x06, 0x00, 0x80, 0x10, 0xe0, 0x08, 0x00, 
  0x00, 0x80, 0x01, 0xe0, 0x40, 0x21, 0xe0, 0x08, 0x40, 0x0f, 0x00, 0x8f, 0x91, 0x10, 0x08, 0x00, 
  0x00, 0x80, 0x01, 0x00, 0x40, 0x21, 0x00, 0x08, 0x40, 0x19, 0xc0, 0x80, 0x91, 0x13, 0xc8, 0x00, 
  0x00, 0x80, 0x01, 0x00, 0x40, 0x21, 0x00, 0x08, 0x40, 0x30, 0x60, 0x80, 0x91, 0x10, 0x48, 0x00, 
  0x00, 0xff, 0xc1, 0x00, 0x7f, 0xe1, 0x00, 0x08, 0x00, 0x60, 0x30, 0x87, 0x90, 0xe0, 0x48, 0x00, 
  0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x08, 0x00, 0x40, 0x00, 0x80, 0x90, 0x00, 0x48, 0x00, 
  0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x08, 0x00, 0x00, 0x00, 0x80, 0x93, 0xfc, 0x48, 0x00, 
  0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0xff, 0xe0, 0x00, 0x00, 0x80, 0x10, 0x40, 0x08, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x40, 0x08, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1c, 0x30, 0x00, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0xb0, 0x00, 0x03, 0x8c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x9c, 0xf0, 0x00, 0x03, 0xfc, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x98, 0x30, 0x00, 0x06, 0x0c, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
  0x00, 0x00, 0xf0, 0x33, 0x00, 0x0c, 0xa4, 0x80, 0x00, 0x00, 0xfc, 0x80, 0x00, 0x00, 0x00, 0x70, 
  0x00, 0x00, 0x34, 0xbf, 0x00, 0x7c, 0x07, 0x80, 0x00, 0x00, 0x84, 0x80, 0x00, 0x00, 0x00, 0xd0, 
  0x00, 0x00, 0x10, 0x31, 0x00, 0x44, 0x4c, 0x00, 0x00, 0x00, 0x84, 0xf8, 0xf8, 0xf9, 0xf0, 0x10, 
  0x00, 0x00, 0x19, 0x21, 0x00, 0x46, 0x18, 0x00, 0x00, 0x00, 0x84, 0x88, 0x88, 0x81, 0x00, 0x10, 
  0x00, 0x00, 0x0c, 0x60, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0xfc, 0x80, 0x88, 0xf9, 0xf0, 0x10, 
  0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0xa0, 0x00, 0x00, 0x00, 0x80, 0x80, 0xf8, 0x08, 0x10, 0x10, 
  0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0xa8, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x08, 0x10, 0x10, 
  0x00, 0x00, 0x1e, 0x70, 0x00, 0x03, 0xb8, 0x00, 0x00, 0x00, 0x80, 0x80, 0xf8, 0xf9, 0xf0, 0x10, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0xfe, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00
};
// '백그라운드', 128x32px
const unsigned char back [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x34, 0xc0, 0x00, 0x02, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x1d, 0xf0, 0x23, 0x7c, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x1e, 0x58, 0x33, 0x24, 0x00, 0x1f, 0xc0, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x7e, 0x08, 0x11, 0xcc, 0x00, 0x30, 0x40, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x48, 0x30, 0x0f, 0x78, 0x00, 0x20, 0x47, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xcc, 0x60, 0x00, 0x00, 0x06, 0x20, 0x40, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x20, 0x40, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xc0, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x1f, 0x80, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x06, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x04, 0x07, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x00, 0x0d, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x00, 0x08, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x61, 0x80, 0x18, 0x40, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xc0, 0x30, 0x30, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x40, 0x40, 0x18, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x20, 0xc0, 0x0c, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x11, 0x80, 0x07, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0b, 0x00, 0x01, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x0e, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x04, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x03, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x01, 0x80, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0xc0, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x40, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x20, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// '백그라운드 제외 캐릭터만', 18x19px
const unsigned char ch [] PROGMEM = {
  0x00, 0x00, 0x00, 0x60, 0x00, 0x80, 0xb8, 0x01, 0x40, 0x84, 0x06, 0x40, 0xc3, 0xf8, 0x40, 0x4c, 
  0x0c, 0x80, 0x50, 0x02, 0x80, 0x60, 0x01, 0x80, 0x40, 0x00, 0x80, 0xce, 0x1c, 0x80, 0x8e, 0x1c, 
  0x40, 0x8e, 0x1c, 0x40, 0x80, 0x00, 0x40, 0x82, 0x48, 0x40, 0x83, 0xf8, 0x40, 0x40, 0xa0, 0x80, 
  0x20, 0xe1, 0x00, 0x10, 0x02, 0x00, 0x0f, 0xfc, 0x00
};
// '냥코2단계', 18x19px
const unsigned char ch_2 [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x06, 0x00, 0x1c, 0x0e, 0x00, 0x17, 
  0xfa, 0x00, 0x14, 0x12, 0x00, 0x18, 0x0e, 0x00, 0x10, 0x04, 0x00, 0x16, 0x34, 0x00, 0x76, 0x37, 
  0x00, 0x50, 0x05, 0x00, 0x53, 0xe5, 0x00, 0x49, 0x49, 0x00, 0x04, 0x10, 0x00, 0x03, 0xe0, 0x00, 
  0x02, 0x20, 0x00, 0x0e, 0x38, 0x00, 0x00, 0x00, 0x00
};
// 'food1', 27x29px
const unsigned char food_1 [] PROGMEM = {
  0x00, 0x00, 0x78, 0x00, 0x00, 0x01, 0xc8, 0x00, 0x00, 0x07, 0x18, 0x00, 0x00, 0x04, 0x10, 0x00, 
  0x00, 0xfc, 0x30, 0x00, 0x01, 0x88, 0x60, 0x00, 0x03, 0x08, 0xe0, 0x00, 0x06, 0x0f, 0xb8, 0x00, 
  0x0b, 0x0c, 0x34, 0x00, 0x11, 0x80, 0x62, 0x00, 0x21, 0xc1, 0xe1, 0x00, 0x20, 0x3f, 0x01, 0x00, 
  0x40, 0x00, 0x00, 0x80, 0x40, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x40, 0x80, 0x00, 0x00, 0x40, 
  0x80, 0x00, 0x00, 0x40, 0x80, 0x00, 0x00, 0x40, 0x80, 0x00, 0x00, 0x40, 0x80, 0x00, 0x00, 0x40, 
  0x40, 0x00, 0x00, 0x80, 0x40, 0x00, 0x00, 0x80, 0x20, 0x00, 0x01, 0x00, 0x20, 0x00, 0x01, 0x00, 
  0x10, 0x00, 0x02, 0x00, 0x08, 0x00, 0x04, 0x00, 0x06, 0x00, 0x18, 0x00, 0x01, 0xc0, 0xe0, 0x00, 
  0x00, 0x3f, 0x00, 0x00
};
// 'food2', 27x29px
const unsigned char food_2 [] PROGMEM = {
  0x00, 0x00, 0x78, 0x00, 0x00, 0x01, 0xc8, 0x00, 0x00, 0x07, 0x18, 0x00, 0x00, 0x04, 0x10, 0x00, 
  0x00, 0xfc, 0x30, 0x00, 0x01, 0x88, 0x60, 0x00, 0x03, 0x08, 0xe0, 0x00, 0x06, 0x0f, 0xb8, 0x00, 
  0x0b, 0x0c, 0x34, 0x00, 0x11, 0x80, 0x62, 0x00, 0x21, 0xc1, 0xe3, 0x00, 0x20, 0x3f, 0x0e, 0x00, 
  0x40, 0x00, 0x18, 0x00, 0x40, 0x00, 0x30, 0x00, 0x80, 0x00, 0x40, 0x00, 0x80, 0x00, 0xc0, 0x00, 
  0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 
  0x40, 0x00, 0x80, 0x00, 0x40, 0x00, 0xc0, 0x00, 0x20, 0x00, 0x40, 0x00, 0x20, 0x00, 0x60, 0x00, 
  0x10, 0x00, 0x30, 0x00, 0x08, 0x00, 0x18, 0x00, 0x06, 0x00, 0x10, 0x00, 0x01, 0xc0, 0xe0, 0x00, 
  0x00, 0x3f, 0x00, 0x00
};
// 'food3', 27x29px
const unsigned char food_3 [] PROGMEM = {
  0x00, 0x00, 0x78, 0x00, 0x00, 0x01, 0xc8, 0x00, 0x00, 0x07, 0x18, 0x00, 0x00, 0x04, 0x10, 0x00, 
  0x00, 0xfc, 0x30, 0x00, 0x01, 0x88, 0x60, 0x00, 0x03, 0x08, 0xe0, 0x00, 0x06, 0x0f, 0xb8, 0x00, 
  0x07, 0x0c, 0x3c, 0x00, 0x03, 0x80, 0x6c, 0x00, 0x03, 0xc1, 0xe8, 0x00, 0x01, 0x3f, 0x18, 0x00, 
  0x01, 0x80, 0x10, 0x00, 0x00, 0x80, 0x30, 0x00, 0x00, 0x80, 0x40, 0x00, 0x00, 0xc0, 0xc0, 0x00, 
  0x00, 0x40, 0x80, 0x00, 0x00, 0x40, 0x80, 0x00, 0x00, 0x40, 0x80, 0x00, 0x00, 0x40, 0x80, 0x00, 
  0x00, 0x40, 0x80, 0x00, 0x00, 0x40, 0xc0, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00, 0xc0, 0x60, 0x00, 
  0x00, 0x80, 0x30, 0x00, 0x00, 0x80, 0x18, 0x00, 0x01, 0x80, 0x10, 0x00, 0x01, 0xc0, 0xe0, 0x00, 
  0x00, 0x3f, 0x00, 0x00
};
// '산책 배경화면', 128x13px
const unsigned char walk_map [] PROGMEM = {
  0x02, 0x00, 0x0c, 0x03, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0xff, 0x07, 0xf8, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1e, 0x07, 0x01, 0xb0, 0x00, 0x02, 0x00, 0x01, 0xb1, 0x8d, 0x8c, 0x00, 0x00, 0x00, 
  0x0f, 0x00, 0x33, 0x05, 0x87, 0x18, 0x00, 0x03, 0x00, 0x03, 0x18, 0x98, 0xc6, 0x07, 0x00, 0x00, 
  0x09, 0x80, 0x61, 0x8c, 0x8c, 0x4c, 0x00, 0x01, 0x00, 0x06, 0x0f, 0xb0, 0x7e, 0x08, 0x80, 0x00, 
  0x68, 0x98, 0xc4, 0x98, 0xd8, 0x46, 0x00, 0x01, 0x00, 0x07, 0xf8, 0x3f, 0xc2, 0x08, 0x98, 0x00, 
  0x0d, 0x80, 0x96, 0xf0, 0x70, 0x02, 0xfe, 0x05, 0x80, 0x04, 0x08, 0x20, 0x42, 0x07, 0x24, 0xc0, 
  0x07, 0x01, 0x90, 0x63, 0x23, 0x63, 0x83, 0x04, 0xc0, 0x04, 0x08, 0x20, 0x42, 0x02, 0x19, 0x20, 
  0x00, 0x03, 0x00, 0xc4, 0x31, 0x31, 0x80, 0xfc, 0x40, 0x04, 0xc8, 0x26, 0x42, 0x02, 0x08, 0xc0, 
  0x02, 0x06, 0x30, 0x94, 0x11, 0x00, 0x9c, 0x30, 0xe0, 0x04, 0xc8, 0x26, 0x42, 0x02, 0x08, 0x40, 
  0x02, 0x04, 0x0d, 0x81, 0xb8, 0x30, 0xf7, 0x81, 0xb0, 0x04, 0xc8, 0x26, 0x42, 0x02, 0x08, 0x40, 
  0x00, 0x0c, 0x43, 0x18, 0x9c, 0x10, 0x60, 0xe3, 0x1c, 0x07, 0xff, 0xbf, 0xfe, 0x02, 0x08, 0x40, 
  0x00, 0x08, 0x06, 0x08, 0x04, 0x00, 0x20, 0x3e, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};




void setup() {
  Serial.begin(115200);
  pinMode(12,INPUT);
  pinMode(13,INPUT);
  pinMode(15,INPUT);
  pinMode(2,INPUT);
  pinMode(0,INPUT);
  pinMode(16,INPUT);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  client.setServer(mqtt_server, mqtt_port);

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  main_m();
  timer.setInterval(30000,age_up);
  timer.setInterval(30000,hunger_down);
  timer.setInterval(30000,vo);
}

int start_flag = 0;
int key_flag = 0;
void loop()//---------------main-----------------
{
  if (!client.connected()) 
  {
    client.connect("ESP8266Client", mqtt_user, mqtt_password);
  }
  client.loop();
  start_map();  
  menu();
}


void start_map()
{
  int i = 0,flag = 0,val2 = 0,cnt = 0,cnt2 = 0,time_flag = 0,time_flag2 = 0;
  while(1)
  {
    timer.run();
    if(die_flag != 1)
    {
      int val = analogRead(0);
      if(val >= 800)
      {
        if(time_flag == 0)
        {
          cnt = millis();
          time_flag = 1;
        }
        if(val2 > 800)
        {
          cnt2 = millis();
        }    
      }

      if(cnt2 - cnt > 10000)
      {
        Serial.println("10초이상");
        cnt = 0;
        cnt2 = 0;
        time_flag = 0;
        time_flag2 = 0;
        if((feel == -1)||(feel == 0))
        {
          feel += 1;
        }
      
      }
      Serial.print(val);
      Serial.print(" ");
      Serial.print(cnt);
      Serial.print(" ");
      Serial.println(cnt2);
    
      delay(10);
      val2 = val;
      char myKey = myKeypad.getKey();
      if (myKey == '4')
      {
        break_flag = 1;
      }
      display.clearDisplay();
      display.drawBitmap(0, 0, back, 128, 32, 1);
      if((i % 3) == 0)
      {
        display.drawBitmap(i, 12, ch, 18, 19, 1);
      }
      else
      {
        display.drawBitmap(i, 7, ch, 18, 19, 1);
      }
      delay(50);
      display.display();

      if(flag == 0)
      {
        if(i < 70)
        {
          i++;
          if(i == 69) flag = 1;
        }
      }
      else if(flag == 1)
      {
        i--;
        if(i == 0) flag = 0;
      }
    
      if (break_flag == 1)
      {
        break_flag = 0;
        break;
      } 
    }
    else
    {
      display.clearDisplay();
      display.setCursor(40, 15);
      display.println("die");
      display.display();
    }
  }
  client.loop();
}



void age_up()
{
  age += 1;
  if(age >= 10)
  {
    //age = 0;
    die_flag = 1; 
  }
}




void hunger_down()
{
  if(food > 0)
  {
    food -= 1;
  }
  if(food <= 2)
  {
    if(food == 0)
    {
      die_flag = 1;
    }
    else
    {
      if(feel != -1)
      {
        feel -= 1;
      }
    }
  }
  if((food <= 3)&&(food > 0))
  {
    client.publish(mqtt_topic,"Hungry!!!");
  }
}

void menu()
{
  timer.run();
  if(die_flag != 1)
  {
    display.clearDisplay();
    while(1)
    {
      char myKey = myKeypad.getKey();
      if(myKey == '1')
      {
        display.print(myKey);
        display.clearDisplay();
        delay(50);
        state();
      }
      else if(myKey == '3')
      {
        display.clearDisplay();
        display.display();
        delay(50);
        feed();
      }
      else if(myKey == '2')
      {     
        display.clearDisplay();
        delay(50);
        walk();
      }
      
      else if(myKey == '5')
      {
        display.clearDisplay();
        display.display();
        delay(50);
        caltemp();
      }
      else if(myKey == '4') break;
      else;
      display.setCursor(0, 0);     // Start at top-left corner
      display.println("1.State      3.Food");
      display.setCursor(0, 11);
      display.println("2.Walk       4.Exit");
      display.setCursor(0, 22);
      display.println("             5.Temp");
      display.display();
    }
    
  }
  else
  {
    display.clearDisplay();
    display.setCursor(40, 15);
    display.println("die");
    display.display();
  }
}
void caltemp()
{
  char tmp[30];
  while(1)
    {
      timer.run();
      display.clearDisplay();
      char myKey2 = myKeypad.getKey();
      if(myKey2 == '4')
      {
        display.clearDisplay();
        display.display();
        break;
      }
      sprintf(tmp,"%f",temp_f);
      display.setCursor(0, 0);
      display.print("temp : ");
      display.println(tmp);

      sprintf(tmp,"%f",humidity);
      display.setCursor(0, 11);
      display.print("humi : ");
      display.println(tmp);
         
      display.display();
    }  
}
void age_view()
{
  if((age >= 0)&&(age <= 4))
  {
    display.drawBitmap(90, 13, ch, 18, 19, 1);
  }
  else if((age >= 5)&&(age <= 9))
  {
    display.drawBitmap(90, 13, ch_2, 18, 19, 1);
  }

  
}

void state()
{
  char str[20];
  while(1)
  { 
    timer.run();
    if(die_flag != 1)
    {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("age : ");
      sprintf(str,"%d",age);
      display.println(str);

      display.setCursor(80, 0);
      display.print("food : ");
      sprintf(str,"%d",food);
      display.println(str);
    
      display.setCursor(0, 10);
      display.print("name :"+ Name);

      display.setCursor(0, 20);
      if(food >= 4)
      {
        feel = 1;
      }
      if(feel == -1) display.print("feel :bad!!!" );
      else if(feel == 0) display.print("feel :soso" );
      if(feel == 1) display.print("feel :good^^" );

      age_view();  
      display.display();
      char myKey = myKeypad.getKey();
      if(myKey == '4')
      {
        display.clearDisplay();
        break;
      } 
    }
    else
    {
      display.clearDisplay();
      display.setCursor(40, 15);
      display.println("die");
      display.display();
    }
  }
}

void feed()
{
  display.clearDisplay();
  display.drawBitmap(50, 1, food_1, 27, 29, 1);
  display.display();
  delay(1500);
  
  display.clearDisplay();
  display.drawBitmap(50, 1, food_2, 27, 29, 1);
  display.display();
  delay(1500);
  
  display.clearDisplay();
  display.drawBitmap(50, 1, food_3, 27, 29, 1);
  display.display();
  delay(1500);
  if(food < 5)
  {
    food += 1;
  }
  display.clearDisplay();
}
void main_m()
{
  display.clearDisplay();
  while(1)
  {
    char myKey = myKeypad.getKey();
    if(myKey == '1')
    {
      break;
    }
    display.drawBitmap(0, 0, main_map, 128, 32, 1);
    display.display();
  }
}

void walk()
{
  if((feel == -1)||(feel == 0))
  {
    feel += 1;
  }
  while(1)
  {
    char myKey = myKeypad.getKey();
    if(myKey == '4')
    {
      break_flag_walk = 1;
    }
    display.clearDisplay();
    display.drawBitmap(0, 0, walk_map, 128, 13, 1);
    if(age < 5)
    {
      display.drawBitmap(60, 15, ch, 18, 19, 1);
    }
    else if(age >= 5)
    {
      display.drawBitmap(60, 15, ch_2, 18, 19, 1);
    }
    delay(50);    
    display.display();
    display.startscrollleft(0, 1);
    delay(5000);
    display.stopscroll();
    display.clearDisplay();
    display.display();
    break;
    //display.clearDisplay();
  }
}
void gettemperature()
{
    unsigned long currentMillis = millis();

    if(currentMillis - lastDHTReadMillis >= interval)
    {
        lastDHTReadMillis = currentMillis;

        humidity = dht.readHumidity();
        temp_f = dht.readTemperature();

        if(isnan(humidity) || isnan(temp_f))
        {
            return;
        }
    }
}
void vo()
{
  gettemperature();
  if((temp_f < 20)||(temp_f > 35)||(humidity < 35)||(humidity > 50))//기온이상발생
  {
    if((feel == 0)||(feel == 1))
    {
      feel -= 1;  
    }
  }
}
