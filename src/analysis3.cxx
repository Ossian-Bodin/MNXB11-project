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
    TTreeReaderValue<double> temp{reader, "temp"};

    auto *c3 = new TCanvas("Canvas", "Canvas");
    c3->cd();

    TH1D* h1 = new TH1D("h1", "dailyMaxHist", 365, 1, 366);
    TH1D* h2 = new TH1D("h2", "dailyMaxHist", 365, 1, 366);

    TH1D* dailyMaxHist = new TH1D("dailyMaxHist", "dailyMaxHist", 365, 1, 366);
    TH1D* dailyMaxHist2 = new TH1D("dailyMaxHist2", "dailyMaxHist", 365, 1, 366);

    reader.Next();
    int this_year{*year};
    reader.Restart();

    double maxTemp{-50};
    double minTemp{50};
    int hottestDay{1};
    int coldestDay{1};
    int currentDay{1};
    int histogramDay{1};


    std::cout << "The year is now set to " << this_year << std::endl;

    while(reader.Next()) {
        if (*year != this_year) {
            dailyMaxHist->Fill(hottestDay);
            dailyMaxHist2->Fill(coldestDay);

            h1->SetBinContent(hottestDay, maxTemp);
            h2->SetBinContent(coldestDay, std::abs(minTemp));

            this_year += 1;
            maxTemp = -50;
            minTemp = 50;
            currentDay = 1;
            histogramDay = 1;
        }

        if (*day != currentDay) {
            if (*day > currentDay) {
                currentDay += 1;
                histogramDay += 1;
            }
            else {
                currentDay = 1;
                histogramDay += 1;
            }
        }        

        if (*temp > maxTemp) {
            maxTemp = *temp;
            hottestDay = histogramDay;
        }

        if (*temp < minTemp) {
            minTemp = *temp;
            coldestDay = histogramDay;
        }
    }

    dailyMaxHist->SetFillColor(kRed);
    dailyMaxHist->SetLineColor(kRed);
    
    dailyMaxHist2->SetFillColor(kBlue);
    dailyMaxHist2->SetLineColor(kBlue); 

    dailyMaxHist->Draw();
    dailyMaxHist2->Draw("SAME");

    // h1->SetFillColor(kRed);
    // h2->SetFillColor(kBlue);

    // h1->SetLineColor(kRed); 
    // h2->SetLineColor(kBlue); 

    // h1->Draw();
    // h2->Draw("SAME");

    c3->SaveAs("results/MY_histogram.pdf");
}



