// LED_BUILTIN = pin NO.2

// Neo Pixel
#include <Adafruit_NeoPixel.h>

// ESP wifi 제어 라이브러리
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>                   

#include"web.h"

// ESP wifi 셋업
const char* ssid     = "iptime";
const char* password = "appdevelopment";

AsyncWebServer server(80);

const bool rvt = false;

// NTP 데이터 수집 셋업
WiFiUDP ntpUDP;
const char* NTP      = "pool.ntp.org";
NTPClient timeClient(ntpUDP,NTP,32400,3600000);

#define LED     0
#define button  16

#define NUMPIXELS 4
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, LED, NEO_GRB + NEO_KHZ800);

// Replaces placeholder with button section in your web page
String Processor(const String& var)
{
  if(var == "BUTTONPLACEHOLDER")
  {
    String buttons ="";
    String outputStateValue = outputState();
    buttons+= "<h4>자체 전원 - GPIO 2 - State <span id=\"outputState\"></span></h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"output\" " + outputStateValue + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  return String();
}

String outputState()
{
  if(rvt)
  {return "checked";}
  else
  {return "";}
  return "";
}


void setup()
{
  Serial.begin(9600);
  strip.begin();

//  첫 부팅 LED 점검
  for(int i = 0; i < NUMPIXELS; i++)
    strip.setPixelColor(i, 255, 255, 255);
  strip.show();
  
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(LED_BUILTIN, !rvt);
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nIP address: ");
  Serial.println(WiFi.localIP());
  
  timeClient.begin();
 
  for(int i = 255; i > 0; i--)
  {
    strip.setBrightness(i);
    strip.show();
    delay(10);
  }

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {request->send_P(200, "text/html", index_html, Processor);}
  );

  // Start server
  server.begin();
  pinMode(button, INPUT);
}

void loop()
{
  timeClient.update();

  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  
  Serial.print("time : ");Serial.print(currentHour);Serial.print(":");Serial.print(currentMinute);Serial.println(rvt ? "true" : "false");
  digitalWrite(LED_BUILTIN, rvt);
  delay(1000);
}
