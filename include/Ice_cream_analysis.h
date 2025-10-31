#ifndef ICE_CREAM_ANALYSIS_H
#define ICE_CREAM_ANALYSIS_H

#include <TCanvas.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1D.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include <iostream>
#include <string>
#include <vector>

#include "DataExtraction.h"

// struct for handling ice cream data
struct IceCreamData {
  std::vector<std::string> dates;
  std::vector<double> sales;
};

IceCreamData readIceCreamCSV();
IceCreamData filterIceCreamByYear(const IceCreamData& fullData, int startYear,
                                  int stopYear);

// struct for storing (year, month) <--> avgTemp
struct TemperatureData {
  std::vector<int> years;
  std::vector<int> months;
  std::vector<double> avgTemps;
};

TemperatureData computeMonthlyTemp(const std::vector<Measurement>& meas,
                                   int startyear, int stopyear);

// Plotting function
void plotTempVsSales(const std::vector<Measurement>& measurements,
                     int startyear, int stopyear);

#endif  // ICE_CREAM_ANALYSIS_H
