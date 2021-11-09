#include <Arduino.h>
#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

// Needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "OpenWeather.h"

// User configuration
#define OPENWEATHER_API         ""
#define LOCATION_LATITUDE       50.08
#define LOCATION_LONGITUDE      19.80
#define FORECAST_DAYS           3       // Minimum: 0, Maximum: 8 (includes the current day)


// Weather related
weatherData w;
weatherDataForecast wf[FORECAST_DAYS];
OpenWeather weather(OPENWEATHER_API, LOCATION_LATITUDE, LOCATION_LONGITUDE);

// NTP related
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);



// format and print a time_t value, with a time zone appended.
/*
void printDateTime(time_t t)
{
    char buf[32];
    char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
    strcpy(m, monthShortStr(month(t)));
    sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d",
        hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t));
    Serial.println(buf);
}
*/


void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting...");

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
    Serial.println("Failed to connect to the WiFi network. Resetting...");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  // If you get here you have connected to the WiFi
  Serial.print("Connected to the WiFi network with IP: ");
  Serial.println(WiFi.localIP());

  // Start the NTC client
  timeClient.begin();
}

void loop() {
  timeClient.update();

  Serial.println(timeClient.getFormattedTime());

  // Print time
  //time_t local = myTZ.toLocal(timeClient.getEpochTime());
  //printDateTime(local);

  weather.updateStatus(&w, wf, FORECAST_DAYS);

  Serial.println("---------- CURRENT ----------"); 
  Serial.print("Weather: ");
  Serial.println(w.weather);
  Serial.print("Description: ");
  Serial.println(w.description);
  Serial.print("Icon: ");
  Serial.println(w.icon);
  Serial.print("Temperature: ");
  Serial.println(w.temp);
  Serial.print("Humidity: ");
  Serial.println(w.humidity);
  Serial.print("Timezone offset: ");
  Serial.println(w.tz_offset);
  Serial.print("Sunrise: ");
  Serial.println(w.sunrise);
  Serial.print("Sunset: ");
  Serial.println(w.sunset);

  for (uint8 i = 0; i < FORECAST_DAYS; i++){
    Serial.print("---------- DAY "); 
    Serial.print(i);
    Serial.println(" ----------"); 
    Serial.print("Weather: ");
    Serial.println(wf[i].weather);
    Serial.print("Description: ");
    Serial.println(wf[i].description);
    Serial.print("Icon: ");
    Serial.println(wf[i].icon);
    Serial.print("Minimum temperature: ");
    Serial.println(wf[i].min_temp);
    Serial.print("Maximum temperature: ");
    Serial.println(wf[i].max_temp);
  }

  delay(600000);
}
