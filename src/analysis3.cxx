#include "analysis3.h"
#include "Measurement.h"
#include <TFile.h>
#include <TH2D.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <iostream>
#include <TLegend.h>

double Gaussian(double* x, double* par) { //A custom function
        return par[0]*exp(-0.5*(x[0]*x[0] - 2*x[0]*par[1] + par[1]*par[1])/(par[2]*par[2]));
    }

double WrappedGaussian(double* x, double* par) {
    double val = 0.0;

    for (int k = -1; k <= 1; ++k) {
        double xwrap = x[0] + 365.0 * k;
        val += exp(-0.5 * pow((xwrap - par[1])/par[2], 2));
    }
    return par[0] * val;
}


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

    while(reader.Next()) {
        if (*year != this_year) {
            dailyMaxHist->Fill(hottestDay);
            dailyMaxHist2->Fill(coldestDay);

            this_year += 1;
            maxTemp = -50;
            minTemp = 50;
            currentDay = 1;
            histogramDay = 1;
        }

        if (*day != currentDay) {
            if (*day > currentDay) {
                currentDay += 1;
            }
            else {
                currentDay = 1;
            }
            histogramDay += 1;
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

    TF1* gaussHot = new TF1("GaussianHot", Gaussian, 1, 366, 3);
    TF1* gaussCold = new TF1("GaussianCold", WrappedGaussian, 1, 366, 3);
    
    gaussHot->SetParameters(4, 200, 50); //Starting values for fitting
    gaussCold->SetParameters(7, 365, 50);

    dailyMaxHist->Fit(gaussHot, "Q0R");
    dailyMaxHist2->Fit(gaussCold, "Q0R");

    std::cout << "The mean is " << gaussHot->GetParameter(1) << " and its uncertainty is " << gaussHot->GetParError(1) << std::endl;
    std::cout << "The mean is " << gaussCold->GetParameter(1) << " and its uncertainty is " << gaussCold->GetParError(1) << std::endl;

    TLegend *legend = new TLegend(0.75, 0.75, 0.89, 0.89);

    legend->SetFillStyle(0); //Hollow fill (transparent)
    legend->AddEntry(dailyMaxHist, "", "F"); //Use object title, draw fill
    legend->AddEntry(dailyMaxHist2, "A title", "F"); //Use custom title
    
    dailyMaxHist->SetFillColor(kRed);
    dailyMaxHist->SetLineColor(kRed);
    
    dailyMaxHist2->SetFillColor(kBlue);
    dailyMaxHist2->SetLineColor(kBlue); 

    dailyMaxHist->Draw();
    dailyMaxHist2->Draw("SAME");
    gaussHot->Draw("SAME");
    gaussCold->Draw("SAME");
    legend->Draw(); //Legends are automatically drawn with "SAME"


    c3->SaveAs("results/MY_histogram.pdf");
}



