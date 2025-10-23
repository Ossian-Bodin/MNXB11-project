
#ifndef MEASUREMENT_H
#define MEASUREMENT_H
#include <iostream>
class Measurement { //class object Measurement declaration
 public:
  Measurement(int _year, int _month, int _day, int _hour, int _minute, int _second,
              double _temperature, std::string _quality);

  //Get functions
  int getYear() const;
  int getMonth() const;
  int getDay() const;
  int getHour() const;
  int getMinute() const;
  int getSecond() const;
  double getTemperature() const;
  std::string isValid() const;

 private:
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  double temperature;
  std::string quality;
};
#endif  // MEASUREMENT_H
