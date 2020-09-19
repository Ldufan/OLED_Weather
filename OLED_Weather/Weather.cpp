#include "Weather.h"

Weather::Weather() {
}

// 配置请求信息，私钥、位置、单位
void Weather::config(String userKey, String location, String unit, String lang) {
  _requserKey = userKey;
  _reqLocation = location;
  _reqUnit = unit;
  _reqLang = lang;
}

// 更新实况天气
void Weather::UpDateWeatherNow(WeatherNowData *data) {
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure(); // 不进行服务器身份认证
  HTTPClient https;
  String url = "https://devapi.heweather.net/v7/weather/now?location=" + _reqLocation +
              "&key=" + _requserKey + "&unit=" + _reqUnit + "&lang=" + _reqLang + "&gzip=n";
  if (https.begin(*client, url)) {  // HTTPS连接成功
    int httpCode = https.GET(); // 请求
    if (httpCode > 0) { // 错误返回负值
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) { // 服务器响应
        String payload = https.getString();
        // _parseNowJson(payload);
        const size_t capacity = 2*JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) +
                                JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(15) + 350;
        DynamicJsonDocument doc(capacity);
        deserializeJson(doc, payload);
        JsonObject now = doc["now"];
        data->ServerCode = doc["code"].as<String>();        // API状态码
        data->LastUpdate = doc["updateTime"].as<String>();  // 当前API最近更新时间
        data->Temp = now["temp"].as<int>();                 // 实况温度
        data->FeelLike = now["feelsLike"].as<int>();        // 实况体感温度
        data->Icon =  getMeteoconIcon(now["icon"].as<String>());
        data->WeatherText = now["text"].as<String>();       // 实况天气状况的文字描述
        data->WindDir = now["windDir"].as<String>();        // 实况风向
        data->WindScale = now["windScale"].as<int>();       // 实况风力等级
        data->Humidity = now["humidity"].as<int>();         // 实况相对湿度百分比数值
        data->Precip = now["precip"].as<float>();           // 实况降水量,毫米
      }
    } else { // 错误返回负值
      data->ServerCode = https.errorToString(httpCode).c_str();
      data->LastUpdate = "-1";
      data->Temp = "N/A";
      data->FeelLike = "-1";
      data->Icon = ")";
      data->WeatherText = "-1";
      data->WindDir = "-1";
      data->WindScale = "-1";
      data->Humidity = "-1";
      data->Precip = "-1";
    }
    https.end();
  } else { // HTTPS连接失败
    data->ServerCode = "-1";
    data->LastUpdate = "-1";
    data->Temp = "N/A";
    data->FeelLike = "-1";
    data->Icon = ")";
    data->WeatherText = "-1";
    data->WindDir = "-1";
    data->WindScale = "-1";
    data->Humidity = "-1";
    data->Precip = "-1";
  }
}

// 更新天气预报
void Weather::UpDateWeatherForecast(WeatherForecastData *data) {
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure(); // 不进行服务器身份认证
  HTTPClient https;
  String url = "https://devapi.heweather.net/v7/weather/3d?location=" + _reqLocation +
              "&key=" + _requserKey + "&unit=" + _reqUnit + "&lang=" + _reqLang + "&gzip=n";
  if (https.begin(*client, url)) {  // HTTPS连接成功
    int httpCode = https.GET(); // 请求
    if (httpCode > 0) { // 错误返回负值
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) { // 服务器响应
        String payload = https.getString();
        const size_t capacity = 2*JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(3) +
                                JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(5) +
                                3*JSON_OBJECT_SIZE(26) + 1320;
        DynamicJsonDocument doc(capacity);
        deserializeJson(doc, payload);

        const char* code = doc["code"];
        const char* updateTime = doc["updateTime"];
        JsonArray daily = doc["daily"];
        JsonObject daily_0 = daily[0];
        JsonObject daily_1 = daily[1];
        JsonObject daily_2 = daily[2];

        for (int i = 0; i < 3; i++) {
          data[i].FxDate = daily[i]["fxDate"].as<String>();
          data[i].TempMax = daily[i]["tempMax"].as<String>();
          data[i].TempMin = daily[i]["tempMin"].as<String>();
          data[i].IconDay = getMeteoconIcon(daily[i]["iconDay"].as<String>());
        }
      }
    } else { // 错误返回负值
      for (int i = 0; i < 3; i++) {
        data[i].ServerCode = https.errorToString(httpCode).c_str();
        data[i].LastUpdate = "-1";
        data[i].FxDate = "-1";
        data[i].TempMax = "-1";
        data[i].TempMin = "-1";
        data[i].IconDay = ")";
      }
    }
    https.end();
  } else { // HTTPS连接失败
    for (int i = 0; i < 3; i++) {
      data[i].ServerCode = "-1";
      data[i].LastUpdate = "-1";
      data[i].FxDate = "-1";
      data[i].TempMax = "-1";
      data[i].TempMin = "-1";
      data[i].IconDay = ")";
    }
  }
}

// 更新空气质量
void Weather::UpDateAirQuality(WeatherNowData *data) {
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure(); // 不进行服务器身份认证
  HTTPClient https;
  String url = "https://devapi.heweather.net/v7/air/now?location=" + _reqLocation +
              "&key=" + _requserKey + "&unit=" + _reqUnit + "&lang=" + _reqLang + "&gzip=n";
  if (https.begin(*client, url)) {  // HTTPS连接成功
    int httpCode = https.GET(); // 请求
    if (httpCode > 0) { // 错误返回负值
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) { // 服务器响应
        String payload = https.getString();
        const size_t capacity = 2*JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) +
                                JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(10) + 250;
        DynamicJsonDocument doc(capacity);
        deserializeJson(doc, payload);
        JsonObject now = doc["now"];
        data->Aqi = now["aqi"].as<String>();                // 实时空气质量指数
      }
    } else { // 错误返回负值
      data->Aqi = "-1";
    }
    https.end();
  } else { // HTTPS连接失败
    data->Aqi = "-1";
  }
}

// 获取天气图标
String Weather::getMeteoconIcon(String cond_code) {
  if (cond_code == "999") {
    return ")";
  }
  if (cond_code == "100") {
    return "B";
  }
  if (cond_code == "150") {
    return "C";
  }
  if (cond_code == "503" || cond_code == "504" || cond_code == "507" || cond_code == "508") {
    return "D";
  }
  if (cond_code == "502" || cond_code == "511") {
    return "E";
  }
  if (cond_code == "512" || cond_code == "513") {
    return "F";
  }
  if (cond_code == "405" || cond_code == "499") {
    return "G";
  }
  if (cond_code == "104") {
    return "H";
  }
  if (cond_code == "154") {
    return "I";
  }
  if (cond_code == "509" || cond_code == "510" || cond_code == "514" || cond_code == "515") {
    return "L";
  }
  if (cond_code == "500" || cond_code == "501") {
    return "M";
  }
  if (cond_code == "102") {
    return "N";
  }
  if (cond_code == "302") {
    return "O";
  }
  if (cond_code == "305" || cond_code == "309" || cond_code == "314" || cond_code == "399") {
    return "Q";
  }
  if (cond_code == "306" || cond_code == "307" || cond_code == "310" || cond_code == "311" ||
      cond_code == "312" || cond_code == "315" || cond_code == "316" || cond_code == "317") {
    return "R";
  }
  if (cond_code == "300" || cond_code == "301" ||cond_code == "350" || cond_code == "351") {
    return "T";
  }
  if (cond_code == "400" || cond_code == "404" || cond_code == "406" || cond_code == "407"||
      cond_code == "408" || cond_code == "456" || cond_code == "457") {
    return "U";
  }
  if (cond_code == "401" || cond_code == "409") {
    return "V";
  }
  if (cond_code == "402" || cond_code == "403" || cond_code == "410") {
    return "W";
  }
  if (cond_code == "304" || cond_code == "308" || cond_code == "312" || cond_code == "313" ||
      cond_code == "318") {
    return "X";
  }
  if (cond_code == "101" ||cond_code == "103" || cond_code == "153") {
    return "Y";
  }
  if (cond_code == "303") {
    return "0";
  }
  if (cond_code == "900" || cond_code == "901") {
    return "'";
  }
  return ")";
}

