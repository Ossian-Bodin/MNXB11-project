

#include <TCanvas.h>
#include <TF1.h>
#include <TGaxis.h>
#include <TH1I.h>
#include <TLegend.h>
#include <TString.h>
#include <TStyle.h>

#include "DataExtraction.h"
#include "Measurement.h"
#include "ConsDays.h"

namespace ConsecutiveDays {

std::vector<ConsDays> getConsDays(
    const std::vector<Measurement>& measurements) {
  // TODO: There is a bug where consecutive days of 1 are being added
  // to the final data set and very long streaks above 50 days

  // Calculate the average temperature of each day with measurements and save
  // the date
  std::vector<double> avg_temp_day;
  std::vector<int> days;
  std::vector<int> months;
  std::vector<int> years;

  double avg_temp{0.};

  int num_measurements{0};
  int current_day{-1};
  int current_month{-1};
  int current_year{-1};

  for (const auto& measurement : measurements) {
    if (current_day != measurement.getDay() ||
        current_month != measurement.getMonth() ||
        current_year != measurement.getYear()) {
      // If there were measurements for the previous day,
      // calculate the average and add to vector
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

  int n_len{int(avg_temp_day.size())};

  int cons_days{2};

  // We start from second measurement day because we cannot know if
  // temperature increased or decreased from the day before the
  //
  double previous_temp{avg_temp_day[1]};
  Date previous_date;
  previous_date.day = days[1];
  previous_date.month = months[1];
  previous_date.year = years[1];

  Date current_date;

  bool nondecreasing{avg_temp_day[1] >= avg_temp_day[0]};

  // Iterate over the average temperatures starting from
  // the third day so we can can now if the
  for (int i{2}; i < n_len; ++i) {
    current_date.day = days[i];
    current_date.month = months[i];
    current_date.year = years[i];

    // Check if the next measurement is the day after the previous one
    if (isNextDay(current_date, previous_date)) {
      // Check if temperature is nondecreasing
      if (avg_temp_day[i] >= previous_temp) {
        // If it was nondecreasing already, add 1 to cons_days counter.
        // Otherwise the decreasing streak is broken and we append the previous
        // values to the vectors and set nondecreasing to false and reset
        // counter
        if (nondecreasing) {
          cons_days += 1;
        } else {
          ConsDays new_val;
          new_val.nondecreasing = false;
          new_val.cons_days = cons_days;
          new_val.date = current_date;

          res.push_back(new_val);

          nondecreasing = true;
          cons_days = 1;
        }
      } else {
        if (nondecreasing) {
          ConsDays new_val;
          new_val.nondecreasing = true;
          new_val.cons_days = cons_days;
          new_val.date = current_date;

          res.push_back(new_val);

          nondecreasing = false;
          cons_days = 1;
        } else {
          cons_days += 1;
        }
      }
    }
    // If the next measurement day does not follow the previous one we
    // have to restart and skip forward two days for the same reason as
    // previously
    else {
      // Missing measurments the day after the last so
      // assume the streak ended but make sure it is
      // longer than one otherwise days where it changed
      // but have a missing day after may be added
      if (cons_days > 1) {
        ConsDays new_val;
        new_val.nondecreasing = nondecreasing;
        new_val.cons_days = cons_days;
        new_val.date = current_date;

        res.push_back(new_val);
      }

      // Now we are in the same situation as the start where we have to
      // skip over a day to know if it was increasing or decreasing so
      // we start by making sure the two next days are following each other
      while (!(isNextDay(current_date, previous_date))) {
        previous_date = current_date;

        // Check so that we do not go outside the vectors range
        if (i + 1 >= n_len) {
          break;
        } else {
          i += 1;
          current_date.day = days[i];
          current_date.month = months[i];
          current_date.year = years[i];
        }
      }

      // Now we should have two days that are following each other
      // so we reset as we did in the start
      nondecreasing = (avg_temp_day[i] >= avg_temp_day[i - 1]);
      cons_days = 2;
    }

    // Iterate the new date forward
    previous_date = current_date;
  }

  return res;
}

void plotConsDaysHist(std::vector<ConsDays>& res) {
  TCanvas* c1 = new TCanvas("c1", "");
  c1->cd();
  double xmin{1.5};
  double xmax{50.5};
  int nbins{int(xmax - xmin)};

  TH1F* h1 = new TH1F("h1", "", nbins, xmin, xmax);
  TH1F* h2 = new TH1F("h2", "", nbins, xmin, xmax);

  int min_year{10000};
  int max_year{0};

  for (const auto& data_pts : res) {
    if (data_pts.date.year > max_year) {
      max_year = data_pts.date.year;
    }
    if (data_pts.date.year < min_year) {
      min_year = data_pts.date.year;
    }
    if (data_pts.nondecreasing) {
      h1->Fill(data_pts.cons_days);
    } else {
      h2->Fill(data_pts.cons_days);
    }
  }

  TString hist_text{
      Form("#splitline{Consecutive days of nondecreasing/decreasing "
           "daily}{average temperature between the years %d-%d}",
           min_year, max_year)};

  h1->SetTitle(hist_text);
  gPad->SetTopMargin(0.15);

  TF1* fit1 = new TF1("fit1", "[0]*TMath::Power([1], x)", xmin, xmax);
  fit1->SetParameters(h1->GetBinContent(1), 0.5);
  TF1* fit2 = (TF1*)fit1->Clone("fit2");
  fit1->SetParameters(h1->GetBinContent(2), 0.5);

  h1->Fit("fit1", "RNq");
  fit1->SetLineColor(kRed);
  fit1->SetLineWidth(1);
  fit1->SetLineStyle(2);
  double a1{fit1->GetParameter("p0")};
  double b1{fit1->GetParameter("p1")};
  TString fit1_text{Form("Nondecreasing fit: %.0f#times %.2f^{n}", a1, b1)};

  h2->Fit("fit2", "RNq");
  fit2->SetLineColor(kBlue);
  fit2->SetLineWidth(1);
  fit2->SetLineStyle(2);
  double a2{fit2->GetParameter("p0")};
  double b2{fit2->GetParameter("p1")};
  TString fit2_text{Form("Decreasing fit: %.0f#times %.2f^{n}", a2, b2)};

  gStyle->SetOptStat(0);

  double xmax1 = h1->GetMaximum();
  double xmax2 = h2->GetMaximum();
  double ymax1 = h1->GetMaximum();
  double ymax2 = h2->GetMaximum();
  h1->SetMaximum(std::max(ymax1, ymax2) * 1.1);
  h1->GetXaxis()->SetRangeUser(xmin, std::max(xmax1, xmax2) * 1.1);

  h1->ClearUnderflowAndOverflow();

  h1->GetXaxis()->SetTitle("Consecutive days");
  h1->GetYaxis()->SetTitle("Entries");
  h1->SetLineColor(kRed);
  h2->SetLineColor(kBlue);
  h1->Draw();
  h2->Draw("same");
  fit1->Draw("same");
  fit2->Draw("same");

  h1->GetXaxis()->SetNdivisions(nbins);

  auto legend = new TLegend(0.5, 0.65, 0.88, 0.82);
  legend->AddEntry(h1, "Nondecreasing", "l");
  legend->AddEntry(fit1, fit1_text, "l");
  legend->AddEntry(h2, "Decreasing", "l");
  legend->AddEntry(fit2, fit2_text, "l");
  legend->SetTextSize(0.03);
  legend->Draw();

  c1->SaveAs("results/ConsDaysHist.pdf");
}

// void plotConsDaysYear(std::vector<ConsDays>& res) {
//   // Plot most consecutive days of increasing and decreasing
//   // temperature for each year and do different color for
//   // each e.g. red/blue for increasing/decreasing
//   TCanvas *c1 = new TCanvas("c1","");
//   c1->cd();

// }

bool isLeapYear(int year) {
  if ((year % 4 == 0)) {
    if ((year % 100 == 0) && (year % 400 != 0)) {
      return false;
    } else {
      return true;
    }
  } else {
    return false;
  }
}

int getDaysPerMonth(int year, int month) {
  // If month is one of January, March, May, July,
  // August, October, or December is has 31 days
  if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 ||
      month == 10 || month == 12) {
    return 31;
  }
  // If month is February it has 29 days if it is a leap year,
  // otherwise it has 28
  else if (month == 2) {
    if (isLeapYear(year)) {
      return 29;
    } else {
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

  bool new_day{(current_day == previous_day + 1) &&
               (current_month == previous_month) &&
               (current_year == previous_year)};
  bool new_month{
      (current_day ==
       (previous_day + 1) % getDaysPerMonth(previous_year, previous_month)) &&
      (current_month == previous_month + 1) && (current_year == previous_year)};
  bool new_year{
      (current_day ==
       (previous_day + 1) % getDaysPerMonth(previous_year, previous_month)) &&
      (current_month == (previous_month + 1) % 12) &&
      (current_year == previous_year + 1)};

  if (new_day || new_month || new_year) {
    return true;
  } else {
    return false;
  }
}
}  // namespace ConsecutiveDays