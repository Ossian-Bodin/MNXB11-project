#include <TH1I.h>
#include <TCanvas.h>

#include "Measurement.h"
#include "DataExtraction.h"
#include "ConsDays.h"


std::vector<ConsDays> getConsDays(const std::vector<Measurement>& measurements) {
  // Calculate the average temperature of each day with measurements and save the date
  std::vector<double> avg_temp_day;
  std::vector<int> days;
  std::vector<int> months;
  std::vector<int> years;

  double avg_temp{0.};

  int num_measurements{0};
  int current_day{-1};
  int current_month{-1};
  int current_year{-1};

  for (const auto &measurement : measurements) {
    if (current_day != measurement.getDay() || current_month != measurement.getMonth() || current_year != measurement.getYear()) {
      // If there were measurements for the previous day, calculate the average and add to vector
      if (num_measurements != 0) {
        avg_temp /= num_measurements;
        avg_temp_day.push_back(avg_temp);
        days.push_back(current_day);
        months.push_back(current_month);
        years.push_back(current_year);
      }

      // Reset num_measurements and avg_temp
      num_measurements = 0;
      avg_temp = 0.;
      
      // Set new current date
      current_day = measurement.getDay();
      current_month = measurement.getMonth();
      current_year = measurement.getYear();
    }

    avg_temp += measurement.getTemperature();
    num_measurements += 1;
  }

  // The return vector that stores the structs Consdays
  std::vector<ConsDays> res;

  int cons_days{1};

  // We start from second measurement day because we cannot know if 
  // temperature increased or decreased from the day before the 
  // 
  double previous_temp{avg_temp_day[1]};
  Date previous_date;
  previous_date.day = days[1];
  previous_date.month = months[1];
  previous_date.year = years[1];

  Date current_date;
  
  bool nondecreasing{avg_temp_day[1] > avg_temp_day[0]};

  // Iterate over the average temperatures starting from the third day so we can can
  // now if the 
  for (int i{2} ; i < int(avg_temp_day.size()) ; ++i) {

    current_date.day = days[i];
    current_date.month = months[i];
    current_date.year = years[i];

    // Check if the next measurement is the day after the previous one
    if (isNextDay(current_date, previous_date)) {
      // std::cout<< "It was next day" << std::endl;
      // Check if temperature is nondecreasing
      if (avg_temp_day[i] >= previous_temp) {
        // If it was nondecreasing already, add 1 to cons_days counter. 
        // Otherwise the decreasing streak is broken and we append the previous
        // values to the vectors and set nondecreasing to false and reset counter
        if (nondecreasing) {
          cons_days +=1;
        }
        else {
          ConsDays new_val;
          new_val.nondecreasing = false;
          new_val.cons_days = cons_days;
          new_val.date = current_date;
          
          res.push_back(new_val);
          
          nondecreasing = true;
          cons_days = 1;
        }
      }
      else {
        if (nondecreasing) {

          ConsDays new_val;
          new_val.nondecreasing = true;
          new_val.cons_days = cons_days;
          new_val.date = current_date;
        
          res.push_back(new_val);

          nondecreasing = false;
          cons_days = 1;
        }
        else{
          cons_days +=1;
        }
      }
    }
    // If the next measurement day does not follow the previous one we
    // have to restart and skip forward two days for the same reason as
    // previously
    else {
      std::cout << "Something is funky..." << std::endl;
      return res;
      // previous_date.day = days[1];
      // previous_date.month = months[1];
      // previous_date.year = years[1];
      // nondecreasing = (avg_temp_day[]);
      // cons_days = 1;
    }

    

    // Iterate the new date forward
    previous_date = current_date;
  }

  return res;
}

void plotConsDaysHist(const std::vector<Measurement>& measurements) {
  std::vector<ConsDays> res{getConsDays(measurements)};
  TCanvas *c1 = new TCanvas("c1","c1");
  c1->cd();
  TH1F* h1 = new TH1F("h1","Consecutive days of increasing/decreasing temperature", 19, 1., 20.);

  for (const auto &data_pts : res) {
    h1->Fill(data_pts.cons_days);
  }
  // h1->Draw("HIST");
  h1->Fit("expo","","",1,20);
  h1->Draw();
  c1->SaveAs("results/cons_test.pdf");
}

// void plotConsDaysYear() {
//   // Plot most consecutive days of increasing and decreasing 
//   // temperature for each year and do different color for
//   // each e.g. red/blue for increasing/decreasing
//   T1HI* h1 = new T1HI("h1","Consecutive");
// }

bool isLeapYear(int year) {
  if ((year % 4 == 0)) {
    if ((year % 100 == 0) && (year % 400 !=0)) {
      return false;
    }
    else {
      return true;
    }
  }
  else {
    return false;
  }
}

int getDaysPerMonth(int year, int month) {
  // If month is one of January, March, May, July, August, October, or December is has 31 days
  if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
    return 31;
  }
  // If month is February it has 29 if it is a leap year, otherwise it has 28
  else if (month == 2) {
    if (isLeapYear(year)) {
      return 29;
    }
    else {
      return 28;
    }
  }
  // All other months have 30 days
  else {
    return 30;
  }
}

bool isNextDay(Date current_date, Date previous_date) {
  int previous_day{previous_date.day};
  int previous_month{previous_date.month};
  int previous_year{previous_date.year};

  int current_day{current_date.day};
  int current_month{current_date.month};
  int current_year{current_date.year};

  bool new_day{(current_day == previous_day + 1)
              && (current_month == previous_month)
              && (current_year == previous_year)};
  bool new_month{(current_day == previous_day % getDaysPerMonth(previous_year, previous_month) + 1) 
                && (current_month == previous_month + 1) 
                && (current_year == previous_year)};
  bool new_year{(current_day == previous_day % getDaysPerMonth(previous_year, previous_month) + 1)
              && (current_month == previous_month % 12 + 1)
              && (current_year == previous_year + 1)};
  
  if (new_day || new_month || new_year) {
    return true;
  }
  else {
    return false;
  }
}