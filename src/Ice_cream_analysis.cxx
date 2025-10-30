#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <numeric>

#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TLine.h"

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


// void plotTempVsSales(const std::string& tempFileName, int startyear, int stopyear) 
// {
//     std::cout << "Plotting sales vs temperature data..." << std::endl;

//     // read the data to the corresponding structs
//     TemperatureData temp = computeMonthlyTemp(tempFileName, startyear, stopyear);
//     IceCreamData iceCream = readIceCreamCSV();


//     // testing iceCream container
//     for (int i = 0; i < 10; ++i)
//     {
//         std::cout << iceCream.sales[i] << std::endl;
//     }


//     // trim the temp data set to have same number of elements as sales (sales data set is fixed, i.e. we want to use all its data)
//     if (temp.avgTemps.size() > iceCream.sales.size()) {
//         temp.avgTemps.resize(iceCream.sales.size());
//     }

//     if (temp.avgTemps.size() != iceCream.sales.size()) {
//         std::cerr << "ERROR: Still mismatched after trimming!\n";
//         return;
//     }
    
//     int nBins = temp.avgTemps.size();

//     TH1D* tempHist = new TH1D("tempHist", "Temperature VS Ice Cream Sales", nBins, 0, nBins);
//     TH1D* salesHist = new TH1D("salesHist", "Sales", nBins, 0, nBins);


//     for (int i = 0; i < nBins; ++i)
//     {
//         tempHist->SetBinContent(i + 1, temp.avgTemps[i]);
//         salesHist->SetBinContent(i + 1, 0.1*iceCream.sales[i]); // may need offset bc |sales[i]| >> |temp[i]| (arb 0.1* for now)
//     }

//     auto canv = new TCanvas("canv", "Monthly Temperature VS Ice cream sales", 1200, 600);

//     tempHist->SetLineColor(kBlue);
//     tempHist->SetFillColorAlpha(kBlue - 3, 0.3);
//     tempHist->Draw("HIST");

//     // set axes
//     tempHist->GetXaxis()->SetTitle("Months from 1972-01 to 2020-01");
//     tempHist->GetYaxis()->SetTitle("Temperature");
//     tempHist->GetXaxis()->CenterTitle();
//     tempHist->GetYaxis()->CenterTitle();

    

//     salesHist->SetLineColor(kRed);
//     salesHist->SetFillColorAlpha(kRed - 3, 0.4);
//     salesHist->Draw("HIST SAME");

    

//     // Drawing a y=0 line
//     double xmin = tempHist->GetXaxis()->GetBinLowEdge(1);
//     double xmax = tempHist->GetXaxis()->GetBinUpEdge(tempHist->GetNbinsX());

//     TLine* line = new TLine(xmin, 0, xmax, 0);
//     line->SetLineColor(kBlack);
//     line->SetLineWidth(2);
//     line->Draw("SAME"); 

//     canv->SaveAs("results/temp_vs_sales_test.png");
//     canv->SaveAs("results/temp_vs_sales_test.root");

//     std::cout << "Saved to results/temp_vs_sales_overlay_scaled.png\n";
// }
 