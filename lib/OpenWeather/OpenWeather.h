/*
  OpenWeather.cpp - Library to get weather from Openweather
  Based on the SimpleWeather library created by Dushyant Ahuja.
  Adapted by Vasco Baptista
  Released into the public domain.
*/

#ifndef OpenWeather_h
#define OpenWeather_h

#include "Arduino.h"
#include <ESP8266WiFi.h>

struct weatherData{
  String weather;
  String description;
  String icon;
  float temp;
  int8 humidity;
  int tz_offset;
  unsigned long sunrise;
  unsigned long sunset;
};

struct weatherDataForecast{
  String weather;
  String description;
  String icon;
  float min_temp;
  float max_temp;
};

class OpenWeather
{
  public:
    OpenWeather(String api_key, float lat, float lon);
    void updateStatus(weatherData *w);
    void updateStatus(weatherData *w, weatherDataForecast wf[], uint8 forecast_days);
  private:
    String _response;
    String _url;
    bool _includes_forecast;
};

#endif
