#ifndef ICE_CREAM_ANALYSIS_H
#define ICE_CREAM_ANALYSIS_H

#include <iostream>
#include <vector>
#include <string>
#include <TFile.h>
#include <TH1D.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TGraph.h>
#include <TCanvas.h>


#include "DataExtraction.h"


// struct for handling ice cream data
struct IceCreamData
{
    std::vector<std::string> dates;
    std::vector<double> sales;
};

IceCreamData readIceCreamCSV(); // function defined on struct IceCreamData


// struct for handling SMHI data
struct TemperatureData
{
    std::vector<int> years;
    std::vector<int> months;
    std::vector<double> avgTemps;
};

TemperatureData computeMonthlyTemp(const std::string& filename, int startyear, int stopyear);


// Plotting functions
void plotTemperatureOnly(const std::string& filename, int startyear, int stopyear);
void plotTempVsSales(const std::string& tempFileName, int startyear, int stopyear);




#endif // ICE_CREAM_ANALYSIS_H


