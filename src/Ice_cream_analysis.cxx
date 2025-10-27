#include <fstream>
#include <sstream>
#include <iostream>

#include "Ice_cream_analysis.h"




IceCreamData readIceCreamCSV()
{

    IceCreamData data;
    const std::string& filename = "datasets/ice_cream.csv";
    


    std::ifstream file(filename); // open file(filename) with ifstream
    if (!file.is_open())
    {
        std::cerr << "Error: cannot open file with name" << filename << std::endl;
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

        if (std::getline(ss, dateStr, ',') && std::getline(ss, salesStr)) 
        {
            data.dates.push_back(dateStr);
            data.sales.push_back(std::stod(salesStr));  // stod::convert string to double
        }
    }

    file.close();
    return data;
}




/* 
TTree* tree dynamic_cast<TTree*>(file->Get("weatherdata"));
TTreeReader reader{tree};
TTreeReaderValue<int> month{reader, "month"};
TTreeReaderValue<double> temp{reader, "temp"};

int minyear{1970};
int maxyear{2020};
int bins{maxyear - minyear 1};


*/