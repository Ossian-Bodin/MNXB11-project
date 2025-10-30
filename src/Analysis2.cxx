#include "../include/Analysis2.h"
#include "../include/Measurement.h"
#include "../include/DataExtraction.h"

//all root includes
#include <TH1.h> //histogram
#include <TCanvas.h> //canvas
#include <TFile.h> //to output file ROOT
#include <TStyle.h> //for my graph to look dope!
#include <TMath.h> //i used this for mean and standard devaition :)

#include <cmath>
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
//please note: i asked chat gpt to count the days LOL
//-------------------------------------------------------------------------------------------

//#1 enumerate the days of the year for later use
int day_of_year(int year, int month, int day) {
    static const int days_before_month[] = 
        {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

        //add something to handle a Leap Year (aka make february 1 day longer and 366 instead of 365)
        //leap years include 2008, 2012, 2016, 2020
    int dayofyear = days_before_month[month - 1] + day;

    //full disclosure i needed assistance to navigate the code w/ leap year
    /*basically, i have to determine a few things:
    - is the year divisible by 4, if yes, a leap year.
    - however, if the year is divisble by 100, it may NOT be a leap year unless it is ALSO divisible by 400
    - so that means that years that are divisible by 4, or divisible by 400, are leap years
    - but if its divisible by 100, but NOT 400, its not a leap year */
    bool is_leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    if (is_leap && month > 2) ++dayofyear;
    return dayofyear;

    //then the if statement says that if it IS INDEED A LEAP YEAR
    //any month agter february has to be given +1 day value because there is now 366 days and not 365
}

//-----------------------------------------------------------------------------------------
//THIS IS MY ACTUAL FUNCTION THAT INCLUDES EVERYTHING NEEDED TO RUN MY PART OF THE PROJECT!
//-----------------------------------------------------------------------------------------
void analysis2(const std::vector<Measurement>& data, const std::string& output_filename){
//this means that the vector cant be changed and all the data is passed by reference so we just refer,not copy
//since i just read the data and never modify the original data
//and it outputs in the file

//2) group temperature by day of the year
//The suggested outline for my code was to use a map
//But, I am not familiar with a map. I read through what a map wouldve looked like
//I also tried to learn how to use a map but to me it seemed like a weird way to use an array or vector
//so I thought I would just do

std::map<int, std::vector<double>> dailyTemps;

for (const auto& m : data){
    //keep feb 1 2005 --> feb 1 2023, discard rest of data
    //because its my birthday :)
    if (m.getYear() < 2005 || (m.getYear() == 2005 && (m.getMonth() < 2 || (m.getMonth() == 2 && m.getDay() < 1)))) continue;
    if (m.getYear() > 2023 || (m.getYear() == 2023 && (m.getMonth() > 2 || (m.getMonth() == 2 && m.getDay() > 1)))) continue;

    int dayIndex = day_of_year(m.getYear(), m.getMonth(), m.getDay());
    dailyTemps[dayIndex].push_back(m.getTemperature());}

//3) make my histogram (one bin = one day)
const int nDays = 366; //has to be 366 because of leap years
TH1F* AvgTemp = new TH1F("AvgTemp", "Mean Daily Temperature (February 1 2005 - February 1 2023);Day of Year;Temperature [C]", nDays, 0.5, nDays + 0.5);

//4) get mean temperature and get standard deviation for every day 
//also fill the histogram with data using fill template

//APPARENLTY ROOT has stuff for this already, which makes SO Much sense
//I used TMath header for this 

for (int day = 1; day <= nDays; ++day) {
        auto dailydatatemp = dailyTemps.find(day);
        if (dailydatatemp == dailyTemps.end() || dailydatatemp->second.empty()) continue; //checks if day, and days data, exists

        const auto& temps = dailydatatemp->second; //this is basically each day's temperatures

        double mean = TMath::Mean(temps.size(), temps.data());
        double standarddeviation = TMath::StdDev(temps.size(), temps.data());

        //use figure 2 hint in project instructions for error?
        AvgTemp->SetBinContent(day, mean);
        AvgTemp->SetBinError(day, standarddeviation);
    }

//5) make my histogram MINE and also cool
//optional changes
gStyle->SetOptStat(0);
AvgTemp->SetLineColor(kViolet + 1);
AvgTemp->SetLineWidth(2);
AvgTemp->GetXaxis()->CenterTitle(true);
AvgTemp->GetYaxis()->CenterTitle(true);
//readability and legibility
AvgTemp->GetYaxis()->SetRangeUser(-15,25); //temperature -15 to 25
AvgTemp->GetXaxis()->SetNdivisions(36, kFALSE);
//give month names while keeping day of year functionality 
//do middle of the month instead of beginning of the month?
//got the values for the bin i want to label from chat (number that = middle of the month)
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
AvgTemp->GetXaxis()->LabelsOption("h"); //i wanted the words to be flat not tall 

//6) draw the histogram and then save it //this is from the slides basically
TCanvas* c1 = new TCanvas("c1", "Mean Daily Temperature", 1000, 600);
AvgTemp->Draw("HIST"); //make line first
AvgTemp->SetLineColorAlpha(kViolet - 2, 0.5);//slightly transparent to improve readability
AvgTemp->Draw("E SAME"); //make error bars
c1->Update();
//c1->SaveAs("results/Analysis2_histogram.png"); //save for latex use

// Determine output directory
//asked chat gpt for assistance because i was so confused why there was no saved files outputted
//i tested it so many times and recompiled like a billion times and got NOTHINGGGG
//so i needed a sort of failsafe to save my stuff regardless of whats going on i think
std::filesystem::path outPath(output_filename);
std::filesystem::path outDir = outPath.parent_path();
if (outDir.empty()) outDir = ".";
if (!std::filesystem::exists(outDir)) std::filesystem::create_directories(outDir);

// Save path for latex use (not sure if i need that but chat said it was a good idea to have that as well, so I saved it as a png)
std::filesystem::path png_file = outDir / "Analysis2_Histogram.png";
c1->SaveAs(png_file.c_str());

TFile outFile(output_filename.c_str(), "RECREATE");
AvgTemp->Write();
outFile.Close();

std::cout << "Saved Histogram for Analysis 2 as png and ROOT file" << '\n';
}

//the end!! 