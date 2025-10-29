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


TemperatureData computeMonthlyTemp(const std::string& filename, int startyear, int stopyear)
{
    TemperatureData tempData; // create empty struct to store year, month, tempAvg

    std::ifstream file(filename); // open temperature file (filename)
    if(!file.is_open())
    {
        std::cerr << "Error opening temperature file " << filename << std::endl;
        return tempData; // if fail open, return the (empty) struct
    }

    // for each key (month, year) associate a vector containing all (hourly) temperature readings that (month, year)
    std::map<std::pair<int,int>, std::vector<double>> tempMonthBox; // to be averaged



    std::string line;

    while (std::getline(file, line)) // loop through the lines of (filename)
    {
        if (line.empty())
            continue;
        
        std::stringstream ss(line);     
        double temp;
        std::string date, time;
        std::string quality;

        ss >> date >> time >> temp >> quality;  // parsing the line structure yyyy-mm-dd hh:mm:ss temp quality (ignoring time of day and quality)
        // can we use readMeasurements instead???

        int year = std::stoi(date.substr(0, 4));    // substr(start char, len)
        int month = std::stoi(date.substr(5, 2));   // stoi -> cast string to int


        if (year < startyear || year > stopyear)
            continue;

        
        tempMonthBox[{year, month}].push_back(temp); // collect monthly temperature readings in the keyed boxes
    }

    // // testing the map
    // {

    //     int testYear = 1970;
    //     int testMonth = 1; 

    //     auto key = std::make_pair(testYear, testMonth);
    //     auto it = tempMonthBox.find(key);

    //     std::cout << "Found " << it->second.size()
    //             << " temperature readings for "
    //             << testYear << "-" << testMonth << ":\n";

    //     for (double t : it->second) 
    //     {
    //         std::cout << t << "\n";
    //     }
    //     std::cout << std::endl;
    //     std::cout << "end of test" << std::endl;
    // }  


    for (int year = startyear; year <= stopyear; ++year)
    {
        for (int month = 1; month <= 12; ++month)
        {
            auto key{std::make_pair(year, month)};
            auto i{tempMonthBox.find(key)}; // initialize the iterator i
            
            double avg = 0.0;

            // using iterators here, thanks ChatGPT, second refers to map.second (i.e. the value, not the key)
            if (i != tempMonthBox.end() && !i->second.empty())
                avg = std::accumulate(i->second.begin(), i->second.end(), 0.0) / i->second.size(); // compute monthly average (sum[i]/count)
        
    

            tempData.years.push_back(year); // fill the struct
            tempData.months.push_back(month);
            tempData.avgTemps.push_back(avg);
        }
    }
    
    file.close();
    return tempData;

}

void plotTemperatureOnly(const std::string& filename, int startyear, int stopyear) 
{
    std::cout << "computing average monthly temperature from " 
    << filename << std::endl;
    std::cout << " for years " << startyear << "-" << stopyear << std::endl;

    TemperatureData temp = computeMonthlyTemp(filename, startyear, stopyear);

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


void plotTempVsSales(const std::string& tempFileName, int startyear, int stopyear) 
{
    std::cout << "Plotting sales vs temperature data..." << std::endl;

    // read the data to the corresponding structs
    TemperatureData temp = computeMonthlyTemp(tempFileName, startyear, stopyear);
    IceCreamData iceCream = readIceCreamCSV();


    // testing iceCream container
    for (int i = 0; i < 10; ++i)
    {
        std::cout << iceCream.sales[i] << std::endl;
    }



    if (temp.avgTemps.size() != iceCream.sales.size())
    {
        std::cerr << "Error: size mismatch.\n" 
        << "Temperature months: " << temp.avgTemps.size() << "\n"
        << "Sales months: " << iceCream.sales.size() << "\n";

    }
    
    int nBins = temp.avgTemps.size();

    TH1D* tempHist = new TH1D("tempHist", "Temperature", nBins, 0, nBins);
    TH1D* salesHist = new TH1D("salesHist", "Sales", nBins, 0, nBins);


    for (int i = 0; i < nBins; ++i)
    {
        tempHist->SetBinContent(i + 1, temp.avgTemps[i]);
        salesHist->SetBinContent(i + 1, 0.2*iceCream.sales[i]); // may need offset bc sales[i] >> temp[i]
    }

    auto canv = new TCanvas("canv", "Monthly Temperature VS Ice cream sales", 1200, 600);

    tempHist->SetLineColor(kBlue);
    tempHist->SetFillColorAlpha(kBlue - 3, 0.3);
    tempHist->Draw("HIST");

    salesHist->SetLineColor(kRed);
    salesHist->SetFillColorAlpha(kRed - 3, 0.4);
    salesHist->Draw("HIST SAME");


    canv->SaveAs("results/temp_vs_sales_test.png");
    canv->SaveAs("results/temp_vs_sales_test.root");

    std::cout << "Saved to results/temp_vs_sales_overlay_scaled.png\n";
}
 