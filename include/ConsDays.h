#ifndef CONS_DAYS_h
#define CONS_DAYS_h

#include <string>
// #include <TTree>

struct Date {
  int day;
  int month;
  int year;
};

struct ConsDays {
  bool nondecreasing; 
  int cons_days;

  // The date corresponds to the day it ended i.e. 
  // the day before was the last day that was included 
  // in the streak
  Date date;
};

std::vector<ConsDays> getConsDays(const std::vector<Measurement>& measurements);

void plotConsDaysHist(const std::vector<Measurement>& measurements);

void plotConsDayYear();

bool isLeapYear(int year);

int getDaysPerMonth(int month);

bool isNextDay(Date current_date, Date previous_date);

#endif /* CONS_DAYS_h */