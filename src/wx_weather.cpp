#include <units.h>
#include "configuration.h"
#include "wx_utils.h"
#include "display.h"
#include "utils.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define SEALEVELPRESSURE_HPA (1013.25)
#define CORRECTION_FACTOR (8.2296) // for meters

extern Configuration Config;
extern String fifthLine;

String obsTimeUtc;
float lat;
float lon;
int winddir;
int humidity;
float temp_f;
float windSpeed;
float windGust;
float pressure_in;

namespace WX_Weather
{
    void getWeatherDataApi()
    {
        Utils::println("getWeatherDataApi start");

        Utils::println("getWeatherDataApi apiKey= " + Config.weather.apiKey);
        Utils::println("getWeatherDataApi stationId= " + Config.weather.stationId);

        if (Config.weather.apiKey && Config.weather.stationId)
        {
            const String weatherUrl = "https://api.weather.com/v2/pws/observations/current?stationId=" + Config.weather.stationId + "&format=json&units=e&apiKey=" + Config.weather.apiKey;

            HTTPClient http;
            http.begin(weatherUrl);

            int httpCode = http.GET();
            Utils::println("getWeatherDataApi httpCode= " + httpCode);

            if (httpCode > 0)
            {
                if (httpCode == 200)
                {
                    String payload = http.getString();
                    http.end();

                    Utils::println("getWeatherDataApi payload= " + payload);

                    StaticJsonDocument<2048> doc;
                    DeserializationError error = deserializeJson(doc, payload);

                    if (!error)
                    {
                        obsTimeUtc = doc["observations"][0]["obsTimeUtc"].as<String>();
                        lat = doc["observations"][0]["lat"];
                        lon = doc["observations"][0]["lon"];
                        winddir = doc["observations"][0]["winddir"];
                        humidity = doc["observations"][0]["humidity"];
                        temp_f = doc["observations"][0]["imperial"]["temp"];
                        windSpeed = doc["observations"][0]["imperial"]["windSpeed"];
                        windGust = doc["observations"][0]["imperial"]["windGust"];
                        pressure_in = doc["observations"][0]["imperial"]["pressure"];
                        Utils::println("getWeatherDataApi stop");

                        return;
                    }
                    else
                    {
                        Utils::println("Error get weather data");
                    }
                }
                http.end();
            }
        }
    }

    void setup()
    {
        Utils::println("Wx weather setup");

        if (Config.wxsensor.active)
        {
            Utils::println("Wx weather active");

            if (Config.weather.apiKey && Config.weather.stationId)
            {
                Utils::println("Wx weather run getWeatherDataApi");
                getWeatherDataApi();
            }
        }
    }

    String generateTempString(const float sensorTemp)
    {
        String strTemp = String((int)sensorTemp);
        switch (strTemp.length())
        {
        case 1:
            return "00" + strTemp;
        case 2:
            return "0" + strTemp;
        case 3:
            return strTemp;
        default:
            return "-999";
        }
    }

    String generateHumString(const float sensorHum)
    {
        String strHum = String((int)sensorHum);
        switch (strHum.length())
        {
        case 1:
            return "0" + strHum;
        case 2:
            return strHum;
        case 3:
            if ((int)sensorHum == 100)
            {
                return "00";
            }
            else
            {
                return "-99";
            }
        default:
            return "-99";
        }
    }

    String generatePresString(const float sensorPres)
    {
        String strPress = String((int)sensorPres);
        String decPress = String(int((sensorPres - int(sensorPres)) * 10));
        switch (strPress.length())
        {
        case 1:
            return "000" + strPress + decPress;
        case 2:
            return "00" + strPress + decPress;
        case 3:
            return "0" + strPress + decPress;
        case 4:
            return strPress + decPress;
        case 5:
            return strPress;
        default:
            return "-99999";
        }
    }

    String readData()
    {
        Utils::println("Wx weather readData");

        getWeatherDataApi();
        Utils::println("Wx weather readData done");

        int year, month, day, hour, minute, second;
        sscanf(obsTimeUtc.c_str(), "%d-%d-%dT%d:%d:%dZ", &year, &month, &day, &hour, &minute, &second);

        int lat_deg = (int)lat;
        float lat_min = (lat - lat_deg) * 60.0;

        int lon_deg = (int)lon;
        float lon_min = (lon - lon_deg) * 60.0;

        char lat_str[11];
        char lon_str[12];
        snprintf(lat_str, sizeof(lat_str), "%02d%05.2fN", lat_deg, lat_min);
        snprintf(lon_str, sizeof(lon_str), "%03d%05.2fE", lon_deg, lon_min);

        char wxPacket[200];
        snprintf(wxPacket, sizeof(wxPacket),
                 "@%02d%02d%02dz%s/%s_"
                 "c%03d"
                 "s%03d"
                 "g%03d"
                 "t%03d"
                 "h%02d"
                 "b%05d",
                 day, hour, minute,
                 lat_str, lon_str,
                 winddir,
                 (int)windSpeed,
                 (int)windGust,
                 (int)temp_f,
                 humidity,
                 inHgToTenthsHpa(pressure_in));

        return wxPacket;
    }
}