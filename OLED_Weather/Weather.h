#ifndef _WEATHER_H_
#define _WEATHER_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

typedef struct WeatherNowData {
  String ServerCode;
  String LastUpdate;
  String Temp;
  String FeelLike;
  String Icon;
  String WeatherText;
  String WindDir;
  String WindScale;
  String Humidity;
  String Precip;
  String Aqi;
} WeatherNowData;

typedef struct WeatherForecastData {
  String ServerCode;
  String LastUpdate;
  String FxDate;
  String TempMax;
  String TempMin;
  String IconDay;
} WeatherForecastData;

class Weather {
  public:
    Weather();

    void config(String userKey, String location, String unit, String lang);

    void UpDateWeatherNow(WeatherNowData *data);
    void UpDateAirQuality(WeatherNowData *data);
    void UpDateWeatherForecast(WeatherForecastData *data);

  private:
    const char* _host = "devapi.heweather.net"; // 服务器地址
    const int httpsPort = 443;

    String getMeteoconIcon(String cond_code);

    String _requserKey;  // 私钥
    String _reqLocation; // 位置
    String _reqUnit;     // 单位
    String _reqLang;     // 语言
};

#endif
