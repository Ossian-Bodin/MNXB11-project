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
#include <map>
#include <iostream>
#include <string>
#include <filesystem>

/*emma: (analysis 2) - average daily temperature from 2005 to 2023
 - (on my birthday feb 1 to most recent birthday data, using Uppsala data)
 - x-axis: day of year, 1-366
 - y-axis: temperature in C -15 -> +25
 - color purple
 - output a ROOT file
 - force error bars like in Figure 2 of example instructions*/

//-------------------------------------------------------------------------------------------
//make yearmonthday into values 1-366 for my histogram (like figure 2)
//numerical value of days to speed up
//so 0 january, 31 february, etc
//please note: i asked chat gpt to count the days LOL
//-------------------------------------------------------------------------------------------

//#1 give numbers to the days of the year
int calc_day_num(int year, int month, int day) {
    static const int days_before_month[] = 
        {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

        //add something to handle a Leap Year (aka make february 1 day longer and 366 instead of 365)
        //leap years include 2008, 2012, 2016, 2020
    int dayofyear = days_before_month[month - 1] + day;

    /*leapyr
    - divisible by 4
    - if divisble by 100, may NOT be a leap year unless ALSO divisible by 400
    - divisible by 4 divisible by 400, are leap years
    - if divisible by 100, but NOT 400, its not a leap year*/
    bool is_leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    if (is_leap && month > 2) ++dayofyear;
    return dayofyear;

    //if it IS INDEED A LEAP YEAR:
    //months agter february has to be given +1 day value because there is now 366 days and not 365
}

//-----------------------------------------------------------------------------------------
//FUNCTION TO RUN MY PART OF THE PROJECT :)
//-----------------------------------------------------------------------------------------
void analysis2(const std::vector<Measurement>& data, const std::string& output_filename){
//vector cant be changed passed by reference not copy
//read the data, never modify the original data
//outputs in the file

//2) group temperature by day of the year
//The suggested outline for my code was to use a map
//I thought that was excessive. Originally I did use a map, I learned what to write, but it felt too clunky.
//Like it served no purpose in this specific dataset, since its pretty straight forward. I thought a vector would work fine
//So i went back and re-wrote it simpler(oct 30)

//std::map<int, std::vector<double>> tempsonday; (old code for map)
const int nDays = 366; //has to be 366 because of leap years
std::vector<double> tempsonday[nDays];

for (const auto& m : data){
    //keep feb 1 2005 --> feb 1 2023, discard rest of data
    //because its my birthday :)
    if (m.getYear() < 2005 || (m.getYear() == 2005 && (m.getMonth() < 2 || (m.getMonth() == 2 && m.getDay() < 1)))) continue;
    if (m.getYear() > 2023 || (m.getYear() == 2023 && (m.getMonth() > 2 || (m.getMonth() == 2 && m.getDay() > 1)))) continue;

    int dayIndex = calc_day_num(m.getYear(), m.getMonth(), m.getDay() - 1);
    tempsonday[dayIndex].push_back(m.getTemperature());}

//3) make my histogram (one bin = one day)
TH1F* AvgTemp = new TH1F("AvgTemp", "Mean Daily Temperature (February 1 2005 - February 1 2023);Day of Year;Temperature [C]", nDays, 0.5, nDays + 0.5);

//4) get mean temperature and get standard deviation for every day
//also fill the histogram with data using fill template

//ROOT has stuff for this, TMath header
for (int day = 0; day < nDays; ++day) {
    
        if (tempsonday[day].empty()) continue; //check if day, and days temp, exists bc of leap year

        const auto& temps = tempsonday[day]; //temperatures for every instance of the same date
        //calculate mean and standarddeviation
        double meantemp = TMath::Mean(temps.size(), temps.data());
        double standarddeviation = TMath::StdDev(temps.size(), temps.data());

        //use figure 2 hint in project instructions for error bars
        AvgTemp->SetBinContent(day + 1, meantemp);
        AvgTemp->SetBinError(day + 1, standarddeviation);
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
AvgTemp->GetXaxis()->SetNdivisions();
//give month names while keeping day of year functionality 
//do beginning of the month for clarity?
//got the values for the bin i want to label from chat
AvgTemp->GetXaxis()->SetBinLabel(1, "Jan");
AvgTemp->GetXaxis()->SetBinLabel(32, "Feb");
AvgTemp->GetXaxis()->SetBinLabel(60, "Mar");
AvgTemp->GetXaxis()->SetBinLabel(91, "Apr");
AvgTemp->GetXaxis()->SetBinLabel(121, "May");
AvgTemp->GetXaxis()->SetBinLabel(152, "Jun");
AvgTemp->GetXaxis()->SetBinLabel(182, "Jul");
AvgTemp->GetXaxis()->SetBinLabel(213, "Aug");
AvgTemp->GetXaxis()->SetBinLabel(244, "Sep");
AvgTemp->GetXaxis()->SetBinLabel(274, "Oct");
AvgTemp->GetXaxis()->SetBinLabel(305, "Nov");
AvgTemp->GetXaxis()->SetBinLabel(335, "Dec");
AvgTemp->GetXaxis()->LabelsOption("h"); //i wanted the words to be flat not tall 

//6) draw the histogram and then save it //this is from the slides basically
TCanvas* analysis2canvas = new TCanvas("Analysis2Canvas", "Mean Daily Temperature", 1000, 600);
AvgTemp->Draw("HIST"); //make line first
AvgTemp->SetLineColorAlpha(kViolet - 2, 0.5);//slightly transparent to improve readability
AvgTemp->Draw("E SAME"); //make error bars
analysis2canvas->Update();

// Determine output directory
//asked chat gpt for assistance because i was so confused why there was no saved files outputted
//i tested it so many times and recompiled like a billion times and got NOTHING
//finally realized that the real issue was my header file named incorrectly...
//so my code never ran and it wasnt the output that was the problem!! oops!
std::filesystem::path outPath(output_filename);
std::filesystem::path outDir = outPath.parent_path();
if (outDir.empty()) outDir = ".";
if (!std::filesystem::exists(outDir)) std::filesystem::create_directories(outDir);

// Save path for latex use (not sure if i need that but chat said it was a good idea to have that as well, so I saved it as a png)
std::filesystem::path pdf_file = outDir / "Analysis2_Histogram.pdf";
analysis2canvas->SaveAs(pdf_file.c_str());

TFile outFile(output_filename.c_str(), "UPDATE"); //changed to UPDATE because we want 1 file with all analyses
AvgTemp->Write();
outFile.Close();

std::cout << "Saved Histogram for Analysis 2 as pdf (for latex) and to ROOT file" << '\n';
}

//the end!! 