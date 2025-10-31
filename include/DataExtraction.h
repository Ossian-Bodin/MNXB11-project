#ifndef DATAEXTRACTION_H
#define DATAEXTRACTION_H
#include <TFile.h>
#include <TTree.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Measurement.h"

std::string clean_data(std::string &raw_file); // Function for cleaning raw data files

std::vector<Measurement> read_measurements(const std::string& csv_filename); //Function for reading the datafiles


void persist_measurements(
    const std::vector<Measurement>& measurements,
    const std::string&
        output_filename);  // Function for taking the read datafiles and putting
                           // them in a TTree

#endif /* DATAEXTRACTION_H */