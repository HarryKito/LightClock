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
  strip.begin();

  //  첫 부팅 LED check
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
  
  // Send a GET request to <ESP_IP>/update?state=<inputMessage>
  server.on(
    "/update", HTTP_GET, [] (AsyncWebServerRequest * request)
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
  server.on(
    "/state", HTTP_GET, [] (AsyncWebServerRequest * request)
  { request->send(200, "text/plain", String(rvt).c_str()); }
  );

  // Start server
  server.begin();
}

// Blink LED <<TEST>>
bool blink_ = true;

void loop()
{
  // update NTP time 
  timeClient.update();
  currentHour = timeClient.getHours();
  currentMinute = timeClient.getMinutes();

  Serial.print("time : "); Serial.print(currentHour); Serial.print(":"); Serial.print(currentMinute); Serial.println(rvt ? " true" : " false");
  digitalWrite(LED_BUILTIN, rvt);

  // TEST CASE 1 알리 접속 시간대에 깜빡이게 ㅋㅋㅋ <<TEST>>
  if(currentHour >= 12 && currentMinute >= 10)
  {
    strip.setBrightness(100);
    blink_ = !blink_;

      for (int i = 0; i < NUMPIXELS; i++)
        strip.setPixelColor(i, 255, 255, 255);
    if(!blink_)
      strip.setBrightness(0);
    Serial.print(blink_ ? "Blink is TRUE" : "Blink is False");
  }
  strip.show();
  
  delay(1000);
}
