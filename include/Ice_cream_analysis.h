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


// ice cream data set
struct IceCreamData
{
    std::vector<std::string> dates;
    std::vector<double> sales;
};


IceCreamData readIceCreamCSV(); // function defined on struct IceCreamData


// SMHI data set
struct TemperatureData
{
    std::vector<int> years;
    std::vector<int> months;
    std::vector<double> avgTemps;
};

TemperatureData computeMonthlyTemp(const std::string& filename, int startyear, int stopyear);
//void plotHist(const std::string& filename);


// draw Histogram function



#endif // ICE_CREAM_ANALYSIS_H


