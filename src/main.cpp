#include <Arduino.h>
#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Timezone.h>             // https://github.com/JChristensen/Timezone

// Timezone related
TimeChangeRule standard_end = {"CET", Last, Sun, Mar, 2, 60};
TimeChangeRule summer_end = {"CEST", Last, Sun, Nov, 3, 120};
Timezone myTZ(standard_end, summer_end);

// NTP related
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// format and print a time_t value, with a time zone appended.
void printDateTime(time_t t)
{
    char buf[32];
    char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
    strcpy(m, monthShortStr(month(t)));
    sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d",
        hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t));
    Serial.println(buf);
}


void setup() {
  Serial.begin(115200);
  Serial.println();

  // WiFiManager local intialization
  WiFiManager wifiManager;

  // Reset settings - for testing purposes
  //wifiManager.resetSettings();

  // Set static ip
  //start-block2
  //IPAddress _ip = IPAddress(192, 168, 0, 250);
  //IPAddress _gw = IPAddress(192, 168, 0, 1);
  //IPAddress _sn = IPAddress(255, 255, 255, 0);
  //end-block2
  //wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);

  // Tries to connect to last known settings
  // If it does not connect it starts an access point
  // and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("WeatherStationAP", "weather")) {
    Serial.println("Failed to connect to the WiFi network. Resetting.");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  // If you get here you have connected to the WiFi
  Serial.println("Connected to the WiFi network with IP:");
  Serial.println(WiFi.localIP());

  // Start the NTC client
  timeClient.begin();
}

void loop() {
  timeClient.update();

  // Serial.println(timeClient.getFormattedTime());

  time_t local = myTZ.toLocal(timeClient.getEpochTime());
  printDateTime(local);
  delay(10000);
}


/* TODO:
-Read: https://github.com/skx/esp8266/blob/master/d1-helsinki-tram-times/d1-helsinki-tram-times.ino#L315
*/