#ifndef ANALYSIS2_H
#define ANALYSIS_2

#include <vector>
#include <string>
#include "Measurement.h"

//anaylsis function declaration
//this is analysis2!
/*I want to compute the daily average temperature:
I want to write it between my birthday and the most recent
birthday date in the file (2023 02 01)
Put that info into the histogram!
I want my histogram to be purple :)*/

void analysis2(const std::vector<Measurement>& measurements, const std::string& output_filename);

//measurement: vector of measurement objects that comes from dataextraction
//output_file: path to ROOT file output :)

#endif //ANALYSIS2_H