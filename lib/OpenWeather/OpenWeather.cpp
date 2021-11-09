#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "OpenWeather.h"

OpenWeather::OpenWeather(String api_key, float lat, float lon){
  _url = "/data/2.5/onecall?lat=" + String(lat) + "&lon=" + String(lon) + "&exclude=minutely,hourly,alerts&appid=" + api_key +"&units=metric";
}

void OpenWeather::updateStatus(weatherData *w){
  updateStatus(w, NULL, 0);
}

void OpenWeather::updateStatus(weatherData *w, weatherDataForecast wf[], uint8 forecast_days){
  const char *openweather = "api.openweathermap.org";
  const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80
  WiFiClientSecure httpsClient;

  httpsClient.setInsecure();
  httpsClient.setTimeout(15000); // 15 Seconds
  //delay(1000);

  Serial.println("OpenWeather: Connecting");
  int r=0; //retry counter
  while((!httpsClient.connect(openweather, httpsPort)) && (r < 30)){
      delay(100);
      r++;
  }

  Serial.println("OpenWeather: Query sent");

  httpsClient.print(String("GET ") + _url + " HTTP/1.1\r\n" +
               "Host: " + openweather + "\r\n" +
               "Connection: close\r\n\r\n");

  while (httpsClient.connected()) {
    _response = httpsClient.readStringUntil('\n');
    if (_response == "\r") break;
  }

  //httpsClient.readStringUntil('\n'); // The API sends an extra line with just a number. This breaks the JSON parsing, hence an extra read
  while(httpsClient.connected()){
    _response = httpsClient.readString();
  }
  
  Serial.println("OpenWeather: Response received");

  DynamicJsonDocument doc(2000 + forecast_days * 500);  // Should be enough... I guess
  deserializeJson(doc,_response);

  w->weather =     doc["current"]["weather"][0]["main"].as<String>();
  w->description = doc["current"]["weather"][0]["description"].as<String>();
  w->icon =        doc["current"]["weather"][0]["icon"].as<String>();
  w->temp =        doc["current"]["temp"].as<float>();
  w->humidity =    doc["current"]["humidity"].as<int8>();
  w->tz_offset =   doc["timezone_offset"].as<int>();
  w->sunrise =     doc["current"]["sunrise"].as<unsigned long>();
  w->sunset =      doc["current"]["sunset"].as<unsigned long>();

  if (forecast_days > 0){
    if (forecast_days > 8) forecast_days = 8;
    for (uint8 i = 0; i < forecast_days; i++){
      wf[i].weather =     doc["daily"][i]["weather"][0]["main"].as<String>();
      wf[i].description = doc["daily"][i]["weather"][0]["description"].as<String>();
      wf[i].icon =        doc["daily"][i]["weather"][0]["icon"].as<String>();
      wf[i].min_temp =    doc["daily"][i]["temp"]["min"].as<float>();
      wf[i].max_temp =    doc["daily"][i]["temp"]["max"].as<float>();
    }
  }
}

