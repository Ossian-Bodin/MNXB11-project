#include "analysis3.h"
#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH2D.h>
#include <TLegend.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <iostream>
#include <TStyle.h>
#include "Measurement.h"

// Gaussian function for fitting
double Gaussian(double* x, double* par) {
    return par[0] * exp(-0.5 * pow((x[0] - par[1])/par[2], 2));
}

// Gaussian function for fitting wrapped distribution
double WrappedGaussian(double* x, double* par) {
    double val = 0.0;
    for (int k = -1; k <= 1; ++k) {
        double xwrap = x[0] + 365.0 * k;
        val += exp(-0.5 * pow((xwrap - par[1]) / par[2], 2));
    }
    return par[0] * val;
}

void analysis3(const std::string& filename) {
    // Opens file in read mode 
    auto file = TFile::Open(filename.c_str(), "READ");
    TTree* tree = (TTree*)file->Get("weatherdata");

    // Reads TTree (the relevant parameters "years", "days", and "temp")
    TTreeReader reader{tree};
    TTreeReaderValue<int> year{reader, "year"};
    TTreeReaderValue<int> day{reader, "day"};
    TTreeReaderValue<double> temp{reader, "temp"};

    // Creates a canvas
    auto *c3 = new TCanvas("Canvas", "Canvas");

    // Two histograms, one for hot a and one for cold
    TH1D* hotHist = new TH1D("hotHist", "Warmest and coldest day of the year", 365, 1, 366);
    TH1D* coldHist = new TH1D("coldHist", "Warmest and coldest day of the year", 365, 1, 366);

    // Checks the first year in the file
    reader.Next();
    int this_year{*year};
    reader.Restart();

    // Initializes variables
    double maxTemp{-50};
    double minTemp{50};
    int hottestDay{1};
    int coldestDay{1};
    int currentDay{1};
    int histogramDay{1};

    // Loop that fills histogram
    while(reader.Next()) {
        // If it is a new year, the hottest and coldes day of the
        // previous year gets added the histogram 
        if (*year != this_year) {
            hotHist->Fill(hottestDay);
            coldHist->Fill(coldestDay);

            this_year += 1;
            maxTemp = -50;
            minTemp = 50;
            currentDay = 1;
            histogramDay = 1;
        }

        // If it is a new day, the "currentDay" variable is updated
        if (*day != currentDay) {

            // Days changing within the same month 
            if (*day > currentDay) {
                currentDay += 1;
            }  

            // If it is a new month, currentDay resets to 1
            else {
                currentDay = 1;
            }
            
            // Always goes up by 1, all the way to 365 
            histogramDay += 1;
        }

        //Compares the current temp with the previous hottest temp
        if (*temp > maxTemp) {
            maxTemp = *temp;
            hottestDay = histogramDay;
        }

        //Compares the current temp with the previous lowest temp
        if (*temp < minTemp) {
            minTemp = *temp;
            coldestDay = histogramDay;
        }
}

    // Created functions for fitting
    TF1* gaussHot = new TF1("GaussianHot", Gaussian, 1, 366, 3);
    TF1* gaussCold = new TF1("GaussianCold", WrappedGaussian, 1, 366, 3);

    // Initial parameters for fitting
    gaussHot->SetParameters(4, 200, 30); 
    gaussCold->SetParameters(4, 15, 20);

    // Fits the histograms
    hotHist->Fit(gaussHot, "Q0");
    coldHist->Fit(gaussCold, "Q0");

    // Creates a legend
    gStyle->SetOptStat(0); // Remove hist stat default legend
    TLegend *legend = new TLegend(0.75, 0.75, 0.89, 0.89);
    legend->AddEntry(hotHist, "Warmest day", "F"); 
    legend->AddEntry(coldHist, "Coldest day", "F"); 
    
    // Set x and y axis
    hotHist->GetXaxis()->SetTitle("Days of the year");
    hotHist->GetYaxis()->SetTitle("Counts");
    hotHist->GetYaxis()->SetTitleOffset(0.7);

    // Colors the hotest histogram red
    hotHist->SetFillColor(kRed);
    hotHist->SetLineColor(kRed);
    
    // Colors the coldest histogram blue
    coldHist->SetFillColor(kBlue);
    coldHist->SetLineColor(kBlue);

    // Set color and style for the fits
    gaussHot->SetLineColor(kBlack);
    gaussCold->SetLineColor(kBlack);
    gaussCold->SetLineStyle(5);

    // Drawing the histograms and fits
    hotHist->Draw();
    coldHist->Draw("SAME");
    gaussHot->Draw("SAME");
    gaussCold->Draw("SAME");
    legend->Draw(); 

    c3->SaveAs("results/HotCold.pdf");
}
