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

    while (std::getline(file,line)) // loop through the line of (filename)
    {
        if (line.empty())
            continue;
        
        std::stringstream ss(line);     
        double temp;
        std::string date, time;
        std::string quality;

        ss >> date >> time >> temp >> quality;  // parsing the line structure yyyy-mm-dd hh:mm:ss temp quality (ignoring time of day)


        int year = std::stoi(date.substr(0, 4));    // substr(start char, len)
        int month = std::stoi(date.substr(5, 2));   // stoi -> cast string to int


        if (year < startyear || year > stopyear)
            continue;

        
        tempMonthBox[{year, month}].push_back(temp); // collect monthly temperature readings in the keyed boxes
    }

    // testing the map
    {

        int testYear = 1970;
        int testMonth = 1; 

        auto key = std::make_pair(testYear, testMonth);
        auto it = tempMonthBox.find(key);

        std::cout << "Found " << it->second.size()
                << " temperature readings for "
                << testYear << "-" << testMonth << ":\n";

        for (double t : it->second) 
        {
            std::cout << t << "\n";
        }
        std::cout << std::endl;
        std::cout << "end of test" << std::endl;
    }  


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




// void plotHist(const std::string& filename) 
// {
//     // note that we assume existence of tree file (generated at ./main -i "filename" -> results/output.root)
//     auto file = TFile::Open(filename.c_str(), "READ");

//     if(!file) // catch faulty file-input
//     {
//         std::cerr << "Error opening file with name " << filename << std::endl;
//         return;
//     }

//     TTree* tree = dynamic_cast<TTree*>(file->Get("weatherdata"));
//     TTreeReader reader{tree}; // initialize reader with tree
//     TTreeReaderValue<int> year{reader, "year"};
//     TTreeReaderValue<int> month{reader, "month"};
//     TTreeReaderValue<double> temp{reader, "temp"};

//     int startyear{1970};
//     int stopyear{2020};

//     int bins{stopyear - startyear + 1};
//     auto tempVsSalesHist = new TH1D("tempVsSalesHist", "Temperature vs ice cream sales (monthly)", bins, startyear, stopyear + 1);

//     reader.Next();
    

// }

/* 
TTree* tree dynamic_cast<TTree*>(file->Get("weatherdata"));
TTreeReader reader{tree};
TTreeReaderValue<int> month{reader, "month"};
TTreeReaderValue<double> temp{reader, "temp"};

int minyear{1970};
int maxyear{2020};
int bins{maxyear - minyear 1};


*/