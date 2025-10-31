#ifndef MEANTEMP_H
#define MEANTEMP_H

#include <TFile.h>
#include <TH1D.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TStyle.h>

#include <iostream>

void tempReader(const std::string& filename);

#endif  // MEANTEMP_H