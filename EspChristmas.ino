#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
//#include "kevin.h"

#include <WiFi.h>
#include "time.h"
#include "fonts.h"
#include "kevin3.h"

TFT_eSPI tft = TFT_eSPI();

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 5;

const char* ssid = "IGK202";
const char* password = "18072019";
const char* ntpServer = "pool.ntp.org";
unsigned long epochTime; 

unsigned long c[4]={86400,3600,60,1};
long now[4];
String nowString[4];

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
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
  tft.setTextColor(TFT_WHITE,0x020A);
  tft.drawString("Christmas Counter    ",6,128);
   tft.setFreeFont(&Orbitron_Light_24);
   //Orbitron_Medium_18
   tft.setTextColor(0xEA79 ,TFT_BLACK);
   tft.drawString(" 2021. ",6,84);
   tft.setTextColor(0xE71C ,TFT_BLACK);
  tft.drawString("DAYS :",6,158);
  tft.drawString("HOUR :",6,186);
  tft.drawString("MINS :",6,214);
  tft.drawString(" SEC ",160,158);
  tft.setTextColor(TFT_WHITE,0x020A);
  tft.drawLine(156,158,156,240,0xFE68);
 tft.drawString("  TIME/DATE  ",6,0,2);
 tft.setTextColor( 0x3779,TFT_BLACK);
//Exact Time Until Christmas Day
  initWiFi();
  configTime(0, 0, ntpServer);
}

long rest=0;
String d,h,m,s;
byte ts,tm,th,td=0;
bool started;
byte f=0;

String now1,now2;
String date;
void ddraw()
{
  epochTime = getTime();
  epochTime=epochTime+3600;
  unsigned long ti=1640390400-epochTime;
  int days=ti/86400;
  rest=ti-(days*86400);
  int hours=rest/3600;
  rest=rest-(hours*3600);
  int minutes=rest/60;
  int sec=rest-(minutes*60);
  
  if(sec<10) s="0"+String(sec); else s=String(sec);
  if(minutes<10) m="0"+String(minutes); else m=String(minutes);
  if(hours<10) h="0"+String(hours); else h=String(hours);
  if(days<10) d="0"+String(days); else d=String(days);


  rest=epochTime-1635638400;
  for(int i=0;i<4;i++)
     {
      now[i]=rest/c[i];
      rest=rest-(now[i]*c[i]);
      nowString[i]=String(now[i]);
      if(now[i]<10)
      nowString[i]="0"+String(now[i]);
     }
   tft.setFreeFont(&DSEG7_Classic_Bold_24);

   //0xFE68
   tft.setTextColor( 0xFE68,TFT_BLACK);
   now1=nowString[1]+":"+nowString[2]; 
   if(now1!=now2)
     {
      tft.drawString(now1,6,30);
      now2=now1;
      } 
      tft.setTextColor( TFT_WHITE,TFT_BLACK);
  if(date!=nowString[0])
  {  
    date=nowString[0];  
    tft.drawString(" November "+date+".",6,58,2);
  }
  
  


tft.setTextColor( 0xEA79,TFT_BLACK);
  tft.setFreeFont(&DSEG7_Modern_Regular_42);
  if(ts!=sec){
    tft.drawString(s,160,196);
    ts=sec;}

    tft.setTextColor(0x3CD8 ,TFT_BLACK);

    tft.setFreeFont(&DSEG7_Classic_Bold_20);
    if(tm!=minutes){
    tft.drawString(m,112,220);
    tm=minutes;}

    if(th!=hours){
    tft.drawString(h,112,192);
    th=hours;}

    if(td!=days){
    tft.drawString(d,112,164);
    td=days;}
  
  }

void loop() {
  delay(49);
  tft.pushImage(102,-6,146,125,kevin4[f]);
  f++;
  if(f==15)
  f=0;
 ddraw();
}
