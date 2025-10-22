#include "Measurement.h"

//Constructor initialization
Measurement::Measurement(int _year, int _month, int _day, int _hour, int _minute,
                         int _second, double _temperature, std::string _quality)
    : year(_year),
      month(_month),
      day(_day),
      hour(_hour),
      minute(_minute),
      second(_second),
      temperature(_temperature),
      quality(_quality) {}

int Measurement::getYear() const { return year; }
int Measurement::getMonth() const { return month; }
int Measurement::getDay() const { return day; }
int Measurement::getHour() const { return hour; }
int Measurement::getMinute() const { return minute; }
int Measurement::getSecond() const { return second; }
double Measurement::getTemperature() const { return temperature; }
std::string Measurement::isValid() const {
  if (quality == "G") {
    return "Valid";
  } else {
    return "Invalid";
  }
}
