#ifndef MEANTEMP_H
#define MEANTEMP_H

#include <TCanvas.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1D.h>
#include <TStyle.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include <iostream>

void tempReader(const std::string& filename);

#endif  // MEANTEMP_H