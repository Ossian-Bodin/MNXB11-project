#include "analysis3.h"
#include "Measurement.h"
#include <TFile.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <iostream>


void analysis3(const std::string& filename) {

    auto file = TFile::Open(filename.c_str(), "READ");
    TTree* tree = (TTree*)file->Get("weatherdata");

    TTreeReader reader{tree};
    TTreeReaderValue<int> year{reader, "year"};
    TTreeReaderValue<int> month{reader, "month"};
    TTreeReaderValue<int> day{reader, "day"};
    TTreeReaderValue<int> hour{reader, "hour"};
    TTreeReaderValue<int> minute{reader, "minute"};
    TTreeReaderValue<int> second{reader, "second"};
    TTreeReaderValue<double> temp{reader, "temp"};
    TTreeReaderValue<std::string> quality{reader, "quality"};


    auto *c3 = new TCanvas("Canvas", "Canvas");
    c3->cd();
    TH2D* dailyMaxHist = new TH2D("dailyMaxHist", "dailyMaxHist", 365, 1, 366, 20, -30, 30);
    TH2D* dailyMaxHist2 = new TH2D("dailyMaxHist2", "dailyMaxHist", 365, 1, 366, 20, -30, 30);

    int this_year{2010};
    double maxTemp{-50};
    double minTemp{50};
    int currentDay{1};
    int histogramDay{1};
    // int currentMonth{1};

    // Goes up from the first year and stops when the year is 2010
    while (reader.Next()) {
        if (*year == this_year) {
            std::cout << "The year is now set to " << *year << std::endl;
            break;
        }
    }

    //The code goes here
    while (reader.Next()) {
        if (*year == this_year+1) {
            break;
        }
        maxTemp = std::max(maxTemp, *temp);
        minTemp = std::min(minTemp, *temp);
        if (*day != currentDay) {
            // Skriv in värdet i histogrammet
            dailyMaxHist->Fill(histogramDay, maxTemp);
            dailyMaxHist2->Fill(histogramDay, minTemp);
            maxTemp = -50;
            minTemp = 50;
            if (*day > currentDay) {
                currentDay += 1;
                histogramDay += 1;
            }
            else {
                currentDay = 1;
                histogramDay += 1;
            }


        }
    }

    dailyMaxHist->SetMarkerColor(kRed);
    dailyMaxHist->SetMarkerStyle(20);
    dailyMaxHist2->SetMarkerColor(kBlue);
    dailyMaxHist2->SetMarkerStyle(21);
    dailyMaxHist->Draw();
    dailyMaxHist2->Draw("SAME");
    c3->SaveAs("results/MY_histogram.pdf");


}




