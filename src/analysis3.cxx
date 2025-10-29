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


    auto *c1 = new TCanvas("Canvas", "Canvas");
    c1->cd();
    TH2D* dailyMaxHist = new TH2D("dailyMaxHist", "dailyMaxHist", 365, 1, 366, 20, -30, 30);

    int this_year{2010};
    double maxTemp{-30};
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
        // std::cout << "The max temp is " << maxTemp << std::endl;
        // std::cout << "The *temp is " << *temp << std::endl;
        maxTemp = std::max(maxTemp, *temp);
        // std::cout << maxTemp << std::endl;
        if (*day != currentDay) {
            // std::cout << "Today is " << *day << " And yesterday was " << currentDay << std::endl;
        
            // Skriv in värdet i histogrammet
            dailyMaxHist->Fill(histogramDay, maxTemp);
            // std::cout << "The highest temp was " << maxTemp << std::endl;
            maxTemp = -30;
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

    //  TFile* outputFile = new TFile("results/meanTemperature.root", "RECREATE");
    
    // outputFile->cd();
    dailyMaxHist->Draw("BOX");
    // graph->Draw("SAME");
    c1->SaveAs("results/MY_histogram.pdf");

    // outputFile->Close();
    // TFile myfile = new TFile("file.root")
    // dailyMaxHist->Draw();
    // dailyMaxHist->Write();


}







// ___CODE THAT MAKES A HIST______


// void analysis3(const std::vector<Measurement>& measurements, const std::string& output_file) {

//     TH1D* tempHist = new TH1D("tempHist", "Temperature Distribution;Day (C);Counts", 60, -30, 35);

//     // // Fill histogram with data
//     for (const auto& m : measurements) {
//         tempHist->Fill(m.getTemperature());
//     }

//     // Write histogram to file
//     tempHist->Write("", TObject::kOverwrite);
//     tempHist->Draw();

//     std::cout << "Analysis 3: Wrote temperature histogram to " << output_file << std::endl;
//     // output_file->Close();
// }

//Run by:

// make
// ./main -i _DATAFILE_ -a 3 
// root -l results/output.root
// [] tempHist->Draw()