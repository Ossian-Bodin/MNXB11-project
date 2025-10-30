
#include "analysis3.h"
#include "Measurement.h"
#include <TFile.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <iostream>
#include <map>
#include <TColor.h>

void analysis3(const std::string& filename) {
    auto file = TFile::Open(filename.c_str(), "READ");
    TTree* tree = (TTree*)file->Get("weatherdata");

    TTreeReader reader{tree};
    TTreeReaderValue<int> year{reader, "year"};
    TTreeReaderValue<int> month{reader, "month"};
    TTreeReaderValue<double> temp{reader, "temp"};

    int startYear = 1950;
    int endYear   = 2020;

    // Histogram: X = month (1–12), Y = year, Z = mean temperature
    TH2D* heatmapMean = new TH2D("heatmapMean", "Mean Monthly Temperature;Month;Year;Temperature (°C)", 12, 0.5, 12.5, endYear-startYear+1, startYear-0.5, endYear+0.5);

    // Map to accumulate sum and count for each (year, month)
    std::map<std::pair<int,int>, std::pair<double,int>> tempData; 
    // key = (year, month), value = (sumTemp, count)

    while (reader.Next()) {
        auto key = std::make_pair(*year, *month);
        if (tempData.find(key) == tempData.end()) {
            tempData[key] = std::make_pair(*temp, 1);
        } else {
            tempData[key].first  += *temp;
            tempData[key].second += 1;
        }
    }

    // Fill histogram with mean temperatures
    for (auto& entry : tempData) {
        int y = entry.first.first;    // year
        int m = entry.first.second;   // month
        double meanTemp = entry.second.first / entry.second.second;
        heatmapMean->Fill(m, y, meanTemp);
    }

    // Draw
    TCanvas* c = new TCanvas("c", "Mean Monthly Temperature Heatmap", 1200, 600);

    heatmapMean->Draw("COLZ");
    c->SaveAs("results/heatmapMean.pdf");

    std::cout << "Heatmap saved to results/heatmapMean.pdf" << std::endl;
}




