// LED_BUILTIN = pin NO.2
// NEO_PIXEL   = pin NO.0

// Neo Pixel
#include <Adafruit_NeoPixel.h>

// ESP wifi ctr library
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include"network.h"

AsyncWebServer server(80);

// NTP data collector
WiFiUDP ntpUDP;
const char* NTP      = "pool.ntp.org";
NTPClient timeClient(ntpUDP, NTP, 32400, 3600000);

// NEO PIXEL STRIP LED
#define NUMPIXELS 3
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, 0, NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial.begin(9600);
  
  pinMode(LED_BUILTIN, OUTPUT);
  strip.begin();

  // LED check
  for (int i = 0; i < NUMPIXELS; i++)
    strip.setPixelColor(i, 255, 255, 255);
  strip.show();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  
  // Local IP Check
  Serial.print("\nIP address: ");
  Serial.println(WiFi.localIP());
  // mDNS Service
  if (!MDNS.begin("homelight"))
    Serial.println("Error setting up MDNS responder!");

  timeClient.begin();

  for (int i = 255; i > 0; i--)
  {
    strip.setBrightness(i);
    strip.show();
    delay(10);
  }

  // root web page
  server.on(
    "/", HTTP_GET, [](AsyncWebServerRequest * request)
  { request->send_P(200, "text/html", index_html, Processor); }
  );
  
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest * request)
  {
    String inputMessage;
    String inputParam;

    if (request->hasParam("state"))
    {
      inputMessage = request->getParam("state")->value();
      inputParam = "state";
      digitalWrite(LED_BUILTIN, inputMessage.toInt());
      rvt = !rvt;
    }
    else
    {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  // Send a GET request to <ESP_IP>/state
  server.on("/state", HTTP_GET, [] (AsyncWebServerRequest * request)
  { request->send(200, "text/plain", String(rvt).c_str()); }
  );

  // Start server
  server.begin();
}

// Blink LED <<TEST>>
bool blink_ = true;

void loop()
{
  rvt = !rvt;
  // update NTP time 
  timeClient.update();
  currentHour = timeClient.getHours();
  currentMinute = timeClient.getMinutes();

  Serial.print("time : "); Serial.print(currentHour); Serial.print(":"); Serial.print(currentMinute); Serial.println( rvt ? " true" : " false");
  digitalWrite(LED_BUILTIN, rvt);
  
// main clock Light System
  strip.setBrightness(255);
  if(currentHour < 6)
  {
    for (int i = 0; i < NUMPIXELS; i++)
        strip.setPixelColor(i, 0, 0, 0); // a darkness!
  }
  else if(currentHour >= 6)
  {
    for (int i = 0; i < NUMPIXELS; i++)
        strip.setPixelColor(i, 255, 80, 25); // bright red colour (similar sunrise near the horizon !)
  }
  else if(currentHour >= 7)
  {
    for (int i = 0; i < NUMPIXELS; i++)
        strip.setPixelColor(i, 255, 200, 65); // bright yellow colour (similar sunrise I liked color !)
  }
  else if(currentHour >= 8)
  {
    for (int i = 0; i < NUMPIXELS; i++)
        strip.setPixelColor(i, 255, 255, 255); // GOOD MORNING
  }

  // // TEST CASE 1 알리 접속 시간대에 blinking! ㅋㅋㅋ <<TEST>>
  // if(currentHour >= 21 && currentMinute >= 50)
  // {
  //   blink_ = !blink_;

  //     for (int i = 0; i < NUMPIXELS; i++)
  //       strip.setPixelColor(i, 255, 255, 255);
  //   if(!blink_)
  //     strip.setBrightness(0);
  //   Serial.print(blink_ ? "Blink is TRUE" : "Blink is False");
  // }
  strip.show();
  delay(1000);
}
