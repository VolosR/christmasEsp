#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

#include <WiFi.h>
#include <Time.h>
#include "fonts.h"
#include "donald.h"
#include <TimeLib.h>

TFT_eSPI tft = TFT_eSPI();

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 5;

const char* ssid = "ssid***";
const char* password = "password***";
const char* ntpServer = "pool.ntp.org";
unsigned long epochTime;

unsigned long c[4] = {86400, 3600, 60, 1};
long nowA[4];
String nowString[4];

char* monthNames[] = {"", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

unsigned long unixTime(int year, int month, int day, int hour, int minute, int second)
{
  month = (month + 9) % 12;
  year = year - month / 10;
  return (365L * year + year / 4 - year / 100 + year / 400 + (month * 306 + 5) / 10 + ( day - 1 ) - 719468L) * 86400L + hour * 3600L + minute * 60 + second;
}

unsigned long lastDayOfMonth(unsigned long epochTime)
{
  int m = month(epochTime) - 1;
  int y = year(epochTime);
  return unixTime(y, m, 0, 0, 0, 0);
}


// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

//String jahr;
void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(0);

  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  ledcAttachPin(18, pwmLedChannelTFT);
  ledcWrite(pwmLedChannelTFT, 250);
  tft.setFreeFont(&Orbitron_Medium_18);
  tft.setTextColor(TFT_WHITE, 0x020A);
  tft.drawString("Christmas Counter    ", 6, 128);
  tft.setFreeFont(&Orbitron_Light_24);
  //Orbitron_Medium_18
  tft.setTextColor(0xEA79 , TFT_BLACK);
  //   tft.drawString(" 2021. ",6,84);
  tft.setTextColor(0xE71C , TFT_BLACK);
  tft.drawString("DAYS :", 6, 158);
  tft.drawString("HOUR :", 6, 186);
  tft.drawString("MINS :", 6, 214);
  tft.drawString(" SEC ", 160, 158);
  tft.setTextColor(TFT_WHITE, 0x020A);
  tft.drawLine(156, 158, 156, 240, 0xFE68);
  tft.drawString("  TIME/DATE  ", 6, 0, 2);
  tft.setTextColor( 0x3779, TFT_BLACK);
  //Exact Time Until Christmas Day
  initWiFi();
  configTime(0, 0, ntpServer);
}

long rest = 0;
String d, h, m, s;
byte ts, tm, th, td = 0;
bool started;
byte f = 0;

String now1, now2;
String date, monat, tag, jahr;
void ddraw()
{
  epochTime = getTime();
//  epochTime = 1641085261;
  epochTime = epochTime + 3600;
//  unsigned long ti = unixTime(year(epochTime), 12, 25, 0, 0, 0) - epochTime;
  unsigned long ti = unixTime(year(epochTime), 12, 25, 0, 0, 0);
  if (ti < epochTime) ti = unixTime(year(epochTime)+1, 12, 25, 0, 0, 0);
  ti = ti - epochTime;
  //  unsigned long ti=1640390400-epochTime;
  int days = ti / 86400;
  rest = ti - (days * 86400);
  int hours = rest / 3600;
  rest = rest - (hours * 3600);
  int minutes = rest / 60;
  int sec = rest - (minutes * 60);

  if (sec < 10) s = "0" + String(sec); else s = String(sec);
  if (minutes < 10) m = "0" + String(minutes); else m = String(minutes);
  if (hours < 10) h = "0" + String(hours); else h = String(hours);
  if (days < 100) d = "0" + String(days); else if (days < 10) d = "00" + String(days); else d = String(days);

  //  rest=epochTime-1638230400; //30.11.2021
  rest = epochTime - lastDayOfMonth(epochTime);
  for (int i = 0; i < 4; i++)
  {
    nowA[i] = rest / c[i];
    rest = rest - (nowA[i] * c[i]);
    nowString[i] = String(nowA[i]);
    if (nowA[i] < 10)
    {
      nowString[i] = "0" + String(nowA[i]);
    }
  }
  tft.setFreeFont(&DSEG7_Classic_Bold_24);

  //0xFE68
  tft.setTextColor( 0xFE68, TFT_BLACK);
  now1 = nowString[1] + ":" + nowString[2];
  if (now1 != now2)
  {
    tft.drawString(now1, 6, 30);
    now2 = now1;
  }
  tft.setTextColor( TFT_WHITE, TFT_BLACK);
  if (date != nowString[0])
  {
    date = nowString[0];
    monat = monthNames[month(epochTime)];
    Serial.println(monat);
    //    tft.drawString(" Dezember "+date+".",6,58,2);
    tag = day(epochTime);
    tft.drawString(" " + monat + " " + tag + ".", 6, 58, 2);

    jahr = year(epochTime);
    Serial.println(epochTime);
    Serial.println(jahr);
    tft.drawString(" " + jahr + ". ", 6, 84);
  }




  tft.setTextColor( 0xEA79, TFT_BLACK);
  tft.setFreeFont(&DSEG7_Modern_Regular_42);
  if (ts != sec) {
    tft.drawString(s, 160, 196);
    ts = sec;
  }

  tft.setTextColor(0x3CD8 , TFT_BLACK);

  tft.setFreeFont(&DSEG7_Classic_Bold_20);
  if (tm != minutes) {
    tft.drawString(m, 114, 220);
    tm = minutes;
  }

  if (th != hours) {
    tft.drawString(h, 114, 192);
    th = hours;
  }

  if (td != days) {
//    if (days < 100) tft.drawString(d, 114, 164); else tft.drawString(d, 98, 164);
    tft.drawString(d, 98, 164);
    td = days;
  }

}

void loop() {
  delay(49);
  tft.pushImage(102, -6, 146, 125, donald[f]);
//  tft.pushImage(102, -6, 146, 125, christmas[f]);
  f++;
  if (f == 16)
//  if (f == 10)
    f = 0;
  ddraw();
}
