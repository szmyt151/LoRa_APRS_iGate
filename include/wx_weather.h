#ifndef WX_WEATHER_H_
#define WX_WEATHER_H_

#include <Arduino.h>


namespace WX_Weather {

    void    getWxAccessViaWifi();
    void    getWeatherDataApi();
    void    setup();
    String  generateTempString(const float sensorTemp);
    String  generateHumString(const float sensorHum);
    String  generatePresString(const float sensorPres);
    String  readData();

}

#endif