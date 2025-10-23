#ifndef MEANTEMP_H
#define MEANTEMP_H

#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TH1D.h>

void tempReader(const std::string& filename);

#endif // MEANTEMP_H