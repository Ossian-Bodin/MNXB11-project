#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <numeric>

#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TLine.h"
#include "TGaxis.h"

#include "Ice_cream_analysis.h"


IceCreamData readIceCreamCSV()
{

    IceCreamData data;
    const std::string& filename = "datasets/ice_cream.csv"; // hardcoded but fine for now. We only have this one dataset
    
    std::ifstream file(filename); // open file(filename) with ifstream
    if (!file.is_open())
    {
        std::cerr << "Error: cannot open file with name " << filename << std::endl;
        return data;
    }

    std::string line;
    std::getline(file, line); // skip header (first line of csv)

    while (std::getline(file, line)) // loop to read lines
    {
        if (line.empty())
            continue;
        
        std::stringstream ss(line);
        std::string dateStr, salesStr;

        if (std::getline(ss, dateStr, ',') && std::getline(ss, salesStr))  // push ','-separated items to dates and sales vectors
        {
            data.dates.push_back(dateStr);
            data.sales.push_back(std::stod(salesStr));  // stod::convert string to double
        }
    }

    file.close();
    return data;
}


IceCreamData filterIceCreamByYear(const IceCreamData& fullData, int startYear, int stopYear)
{
    IceCreamData filtered;
    for (size_t i = 0; i < fullData.dates.size(); ++i)
    {
        const std::string& dateStr = fullData.dates[i];
        if (dateStr.size() < 7) 
            continue;

        int year = std::stoi(dateStr.substr(0, 4));
        if (year >= startYear && year <= stopYear)
        {
            filtered.dates.push_back(dateStr);
            filtered.sales.push_back(fullData.sales[i]);
        }
    }
    return filtered;
}



TemperatureData computeMonthlyTemp(const std::vector<Measurement>& measurements, int startyear, int stopyear)
{
    TemperatureData tempData; // create empty struct to store year, month, tempAvg

    // for each key (month, year) associate a vector containing all (hourly) temperature readings that (month, year)
    std::map<std::pair<int,int>, std::vector<double>> tempMonthBox; // to be averaged

    for (const auto& m: measurements)
    {
        // get data from Measurement class
        int year = m.getYear();
        int month = m.getMonth();
        double temp = m.getTemperature(); 

        if (year >= startyear && year <= stopyear)
        {
            tempMonthBox[{year, month}].push_back(temp);  // fill the map with temps (for averaging)
        }
    }

    for (int year = startyear; year <= stopyear; ++year)
    {
        for (int month = 1; month <= 12; ++month)
        {
            auto key = std::make_pair(year, month);
            auto it = tempMonthBox.find(key); // iterator over key (GPT showed me iterators)

            double avg = 0.0;
            if (it != tempMonthBox.end() && !it->second.empty())
            {
                double sum = std::accumulate(it->second.begin(), it->second.end(), 0.0);
                avg = sum / it->second.size(); // compute the average temp (per month)
            }

            // fill the struct
            tempData.years.push_back(year);
            tempData.months.push_back(month);
            tempData.avgTemps.push_back(avg);
        }
    }

    return tempData; 

}

void plotTemperatureOnly(const TemperatureData& temp) 
{

    // Create histogram with N bins = number of months
    int nBins = temp.avgTemps.size();
    TH1D* hist = new TH1D("tempHist", "Average Monthly Temperature;Month index;Temperature (C)", nBins, 0, nBins);

    for (int i = 0; i < nBins; ++i) 
    {
        hist->SetBinContent(i + 1, temp.avgTemps[i]); // bins start at 1 for some reason
    }

    auto canv = new TCanvas("canv", "Monthly Temperature", 1200, 600);
    
    hist->SetLineColor(kBlue);
    hist->SetLineWidth(2);
    hist->SetFillColorAlpha(kBlue - 3, 0.3);
    hist->GetXaxis()->SetAxisColor(kBlack);
    hist->SetTitle("Average Monthly Temperature;Temperature (C)");
    
    hist->Draw("HIST");

    canv->SaveAs("results/temp_only.png");
    canv->SaveAs("results/temp_only.root");

}


void plotTempVsSales(const std::vector<Measurement>& measurements, int startyear, int stopyear) 
{
    std::cout << "Plotting ice crem sales vs monnthly temperature "
    << "for years " << startyear << "-" << stopyear << std::endl;

    // read the data to the corresponding structs
    TemperatureData temp = computeMonthlyTemp(measurements, startyear, stopyear);
    IceCreamData iceCreamFull = readIceCreamCSV();
    IceCreamData iceCream = filterIceCreamByYear(iceCreamFull, startyear, stopyear); // fill iceCream struct with data between startyear - stopyear

    

    // check that data sets have equal number of months read.
    size_t n = std::min(temp.avgTemps.size(), iceCream.sales.size()); // collect smallest data set length

    if (temp.avgTemps.size() != iceCream.sales.size()) {
        std::cout << "Warning: datasets do not contain the same number of months.\n"
        << "Using " << n << " common months." << std::endl;
    }

    // makes sure the longer data set is resized to # common months
    temp.avgTemps.resize(n);
    iceCream.sales.resize(n);


    //---------------------- print debug alignment -----------------------------------

    std::cout << "\n--- Data Alignment Debug ---\n";
    std::cout << "After resize: "
            << temp.avgTemps.size() << " temperature entries vs. "
            << iceCream.sales.size() << " sales entries.\n\n";

    std::cout << std::left
            << std::setw(15) << "IceCreamDate"
            << std::setw(12) << "Sales"
            << std::setw(12) << "TempDate"
            << std::setw(10) << "AvgTemp"
            << "\n";
    std::cout << "-------------------------------------------------------------\n";

    // Print first 10 entries for context
    for (int N = 0; N < 10; ++N)
    {
        std::string iceDate = iceCream.dates[N];
        double sale = iceCream.sales[N];

        int year = temp.years[N];
        int month = temp.months[N];
        double avg = temp.avgTemps[N];

        std::ostringstream ym;
        ym << year << "-" << std::setw(2) << std::setfill('0') << month;

        std::cout << std::left
                << std::setw(15) << iceDate
                << std::setw(12) << sale
                << std::setw(12) << ym.str()
                << std::setw(10) << avg
                << "\n";
    }

    // Print last two entries
    std::cout << "...\n";
    for (size_t N = temp.years.size() - 2; N < temp.years.size(); ++N)
    {
        std::string iceDate = iceCream.dates[N];
        double sale = iceCream.sales[N];

        int year = temp.years[N];
        int month = temp.months[N];
        double avg = temp.avgTemps[N];

        std::ostringstream ym;
        ym << year << "-" << std::setw(2) << std::setfill('0') << month;

        std::cout << std::left
                << std::setw(15) << iceDate
                << std::setw(12) << sale
                << std::setw(12) << ym.str()
                << std::setw(10) << avg
                << "\n";
    }

    std::cout << "-------------------------------------------------------------\n";
    std::cout << "Printed first 10 and last 2 aligned entries.\n\n";

    // -------------------------------------------------------------------------------------------
    


    // cast # bins to common # months
    int nBins = static_cast<int>(n);


    TH1D* tempHist = new TH1D("tempHist", "Temperature VS Ice Cream Sales", nBins, 0, nBins);
    TH1D* salesHist = new TH1D("salesHist", "Sales", nBins, 0, nBins);


    for (int i = 0; i < nBins; ++i)
    {
        tempHist->SetBinContent(i + 1, temp.avgTemps[i]);
        salesHist->SetBinContent(i + 1, iceCream.sales[i]); 
    }


    auto canv = new TCanvas("canv", "Monthly Temperature VS Ice cream sales", 1200, 600);

    tempHist->SetStats(0); // remove stats box
    salesHist->SetStats(0);

    // main hist: tempHist
    tempHist->SetLineColor(kBlue);
    tempHist->SetFillColorAlpha(kBlue - 3, 0.3);
    tempHist->Draw("HIST");

    // set axes
    std::string Xtitle = "Months from " + std::to_string(startyear) + " to " + std::to_string(stopyear);
    tempHist->GetXaxis()->SetTitle(Xtitle.c_str());
    tempHist->GetYaxis()->SetTitle("Temperature [#circC]"); 
    tempHist->GetXaxis()->CenterTitle();
    tempHist->GetYaxis()->CenterTitle();

    // create second pad for salesHist:

    double leftMax = tempHist->GetMaximum();
    double rightMax = 1.1 * salesHist->GetMaximum();

    double scale = leftMax / rightMax;

    
    salesHist->Scale(scale);
    salesHist->SetLineColor(kRed);
    salesHist->SetFillColorAlpha(kRed - 3, 0.4);
    salesHist->Draw("HIST SAME");


    canv->Update();
    double xminPad = gPad->GetUxmax();
    double yZero = 0.0; // start sales axis at y = 0 baseline
    double ymaxPad = tempHist->GetMaximum();

    // add second y-axis (sales)
    TGaxis* axis = new TGaxis(xminPad, yZero,
                              xminPad, ymaxPad,
                              0, rightMax, 510, "+L"); // (?)

    axis->SetLineColor(kRed);
    axis->SetLabelColor(kRed);
    axis->SetTitleColor(kRed);
    axis->SetTitle("Ice Cream Sales [IPN31152N]");
    axis->CenterTitle(true);

    axis->SetLabelSize(tempHist->GetYaxis()->GetLabelSize());
    axis->SetTitleSize(tempHist->GetYaxis()->GetTitleSize());
    axis->SetLabelFont(tempHist->GetYaxis()->GetLabelFont());
    axis->SetTitleFont(tempHist->GetYaxis()->GetTitleFont());

    axis->Draw();

    

    // Drawing a y=0 line
    double xmin = tempHist->GetXaxis()->GetBinLowEdge(1);
    double xmax = tempHist->GetXaxis()->GetBinUpEdge(tempHist->GetNbinsX());

    TLine* line = new TLine(xmin, 0, xmax, 0);
    line->SetLineColor(kBlack);
    line->SetLineWidth(2);
    line->Draw("SAME"); 


    // legend
    auto legend = new TLegend(0.75, 0.75, 0.9, 0.9);
    legend->AddEntry(tempHist, "Average monthly temperature", "f");
    legend->AddEntry(salesHist, "Monthly ice cream sales", "f");
    legend->Draw();

    canv->SaveAs("results/temp_vs_sales.pdf");
    canv->SaveAs("results/temp_vs_sales.root");

    std::cout << "Saved to results/temp_vs_sales.pdf\n";
}
 