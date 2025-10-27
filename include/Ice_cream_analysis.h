#ifndef ICE_CREAM_ANALYSIS_H
#define ICE_CREAM_ANALYSIS_H

#include <iostream>
#include <vector>
#include <string>

#include "DataExtraction.h"


struct IceCreamData
{
    std::vector<std::string> dates;
    std::vector<double> sales;
};


IceCreamData readIceCreamCSV(); 



#endif // ICE_CREAM_ANALYSIS_H


