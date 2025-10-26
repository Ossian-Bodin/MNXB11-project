#include "../include/Analysis2.h"
#include "../include/Measurement.h"
#include "../include/DataExtraction.h"

//all root includes
#include <TH1.h> //histogram
#include <TCanvas.h> //canvas
#include <TFile.h> //to output file ROOT
#include <TStyle.h> //for my graph to look dope!

#include <cmath>
#include <map>
#include <numeric>
#include <iostream>
#include <string>
#include <filesystem>

//emma: (analysis 2) - average daily temperature from 2005 to 2023
//on my birthday feb 1 to most recent birthday data
//using Uppsala data
/*What I want to do
x-axis: day of year, 1-365 
y-axis: temperature in C -15 -> +30
and i want it to be my favorite color purple
and it should output a ROOT file with TH1F histogram
i want to force error bars like in Figure 2 of example instructions*/

//-------------------------------------------------------------------------------------------
//make year_month_day into values 1-365 for my histogram (like figure 2)
//requires numerical value of every day in each month, so 31 is january, 59 is february, etc
//and im too lazy to count that so i asked chat gpt to count the days :p 
//-------------------------------------------------------------------------------------------
int day_of_year(int year, int month, int day) {
    static const int days_before_month[] = 
        {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

        //add something to handle a Leap Year (aka make february 1 day longer and 366 instead of 365)
        //leap years include 2008, 2012, 2016, 2020
    int dayofyear = days_before_month[month - 1] + day;

    //full disclosure i needed assistance for the bool to calculate a leap year so i asked chat for an outline
    bool is_leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    if (is_leap && month > 2) ++dayofyear;
    return dayofyear;
}

//-----------------------------------------------------------------------------------------
//THIS IS MY ACTUAL FUNCTION THAT INCLUDES EVERYTHING NEEDED TO RUN MY PART OF THE PROJECT!
//-----------------------------------------------------------------------------------------
void analysis2(const std::vector<Measurement>& measurements, const std::string& output_filename){
    
    std::vector<Measurement> data;

//input my data (Uppsala.csv)
if (measurements.empty()){
    std::string input_file = "../datasets/baredata_smhi-opendata_1_97530_20231007_155803_Uppsala.csv";
    data = read_measurements(input_file); //use my uppsala file?
} else {
    data = measurements;
}

//1) group temperature by day of the year
std::map<int, std::vector<double>> dailyTemps;

for (const auto& m : data){
    //keep feb 1 2005 --> feb 1 2023, discard rest of data
    if (m.getYear() < 2005 || (m.getYear() == 2005 && (m.getMonth() < 2 || (m.getMonth() == 2 && m.getDay() < 1)))) continue;
    if (m.getYear() > 2023 || (m.getYear() == 2023 && (m.getMonth() > 2 || (m.getMonth() == 2 && m.getDay() > 1)))) continue;

    int dayIndex = day_of_year(m.getYear(), m.getMonth(), m.getDay());
    dailyTemps[dayIndex].push_back(m.getTemperature());}

//2) make my histogram (one bin = one day)
const int nDays = 366; //has to be 366 because of leap years
TH1F* AvgTemp = new TH1F("AvgTemp", "Mean Daily Temperature (February 1 2005 - February 1 2023);Day of Year;Temperature [C]", nDays, 0.5, nDays + 0.5);

//3) get mean temperature and get standard deviation for every day 
//also fill the histogram with data using fill template
for (int day = 1; day <= nDays; ++day){
    auto it = dailyTemps.find(day);
    if (it == dailyTemps.end() || it->second.empty()) continue;

        const auto& temps = it->second;
        double sum = 0.0;
        for (double t : temps) sum += t;
        double mean = sum / temps.size(); //average
        
        double sq_sum = 0.0;
        for (double t : temps) sq_sum += (t - mean) * (t - mean);
        double standarddeviation = std::sqrt(sq_sum / temps.size());

        //use figure 2 hint in project instructions for error?
        AvgTemp->SetBinContent(day, mean);
        AvgTemp->SetBinError(day, standarddeviation);
    }

//4) make my histogram MINE and also cool
//optional changes
gStyle->SetOptStat(0);
AvgTemp->SetLineColor(kViolet + 1);
AvgTemp->SetLineWidth(2);
AvgTemp->GetXaxis()->CenterTitle(true);
AvgTemp->GetXaxis()->SetTitleSize(0.05);
AvgTemp->GetXaxis()->SetLabelSize(0.05);
AvgTemp->GetYaxis()->CenterTitle(true);
AvgTemp->GetYaxis()->SetTitleSize(0.05);
AvgTemp->GetYaxis()->SetLabelSize(0.05);
//readability and legibility
AvgTemp->GetYaxis()->SetRangeUser(-15,30); //temperature -15 to 30
AvgTemp->GetXaxis()->SetNdivisions(12, 5, kFALSE); //12 major ticks for each month and 5 in between?
//give month names while keeping day of year functionality 
//do middle of the month instead of beginning of the month
AvgTemp->GetXaxis()->SetBinLabel(15, "Jan");
AvgTemp->GetXaxis()->SetBinLabel(46, "Feb");
AvgTemp->GetXaxis()->SetBinLabel(74, "Mar");
AvgTemp->GetXaxis()->SetBinLabel(105, "Apr");
AvgTemp->GetXaxis()->SetBinLabel(135, "May");
AvgTemp->GetXaxis()->SetBinLabel(166, "Jun");
AvgTemp->GetXaxis()->SetBinLabel(196, "Jul");
AvgTemp->GetXaxis()->SetBinLabel(227, "Aug");
AvgTemp->GetXaxis()->SetBinLabel(258, "Sep");
AvgTemp->GetXaxis()->SetBinLabel(288, "Oct");
AvgTemp->GetXaxis()->SetBinLabel(319, "Nov");
AvgTemp->GetXaxis()->SetBinLabel(349, "Dec");
AvgTemp->GetXaxis()->LabelsOption("h");

//5) draw the histogram and then save it?
TCanvas* c1 = new TCanvas("c1", "Mean Daily Temperature", 1000, 600);
AvgTemp->Draw("HIST"); //make line first
AvgTemp->SetLineColorAlpha(kViolet - 2, 0.5);//slightly transparent to improve readability
AvgTemp->Draw("E SAME"); //make error bars
c1->Update();
//c1->SaveAs("results/Analysis2_histogram.png"); //save for latex use

// Determine output directory
std::filesystem::path outPath(output_filename);
std::filesystem::path outDir = outPath.parent_path();
if (outDir.empty()) outDir = ".";
if (!std::filesystem::exists(outDir)) std::filesystem::create_directories(outDir);

// PNG path for latex?
std::filesystem::path png_file = outDir / "Analysis2_Histogram.png";
c1->SaveAs(png_file.c_str());

TFile outFile(output_filename.c_str(), "RECREATE");
AvgTemp->Write();
outFile.Close();

std::cout << "Saved Histogram for Analysis 2 as png and ROOT file" << '\n';
}