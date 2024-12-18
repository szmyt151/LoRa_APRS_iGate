#include "units.h"

String convertDegreesToWindDirection(float degrees) {
    String directions[] = {"N",
                           "NNE",
                           "NE",
                           "ENE",
                           "E",
                           "ESE",
                           "SE",
                           "SSE",
                           "S",
                           "SSW",
                           "SW",
                           "WSW",
                           "W",
                           "WNW",
                           "NW",
                           "NNW",
                           "N"
    };
    return directions[static_cast<int>(round(degrees / 22.5 + 0.5)) % 16];
}

float kmPerHourToKnots(float speedInKmH) {
    return speedInKmH * 0.539957f;
}

float celsiusToFahrenheit(float celsiusTemperature) {
    return celsiusTemperature * 9 / 5 + 32;
}

float fahrenheitToCelsius(float fahrenheitTemperature) {
    return (fahrenheitTemperature - 32) * 5.0 / 9.0;
}

float kmPerHourToMilesPerHour(float speedInKmH) {
    return speedInKmH * 0.621371f;
}

float mmToInch(float mm) {
    return mm * 0.0393701f;
}

int inHgToTenthsHpa(float pressure_in) {
    float pressure_mb = pressure_in * 33.8639;
    int pressure_tenths = (int)((pressure_mb * 10) + 0.5);
    return pressure_tenths;
}