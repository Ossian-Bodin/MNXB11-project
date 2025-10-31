#include <meanTemp.h>

// Fetch data from TTree, do mean temperature on data for each year, create new
// root file with histogram.
void tempReader(const std::string& filename) {
  // Assuming TTree file already exists.
  auto file = TFile::Open(filename.c_str(), "READ");
  if (!file || file->IsZombie()) {
    std::cerr << "Error opening file, is it a TTree root file? " << filename
              << std::endl;
    return;
  }
  TTree* tree = dynamic_cast<TTree*>(file->Get("weatherdata"));
  TTreeReader reader{tree};
  TTreeReaderValue<int> year{reader, "year"};
  TTreeReaderValue<int> day{reader, "day"};
  TTreeReaderValue<int> hour{reader, "hour"};
  TTreeReaderValue<double> temp{reader, "temp"};

  int minyear = INT_MAX;
  int maxyear = INT_MIN;
  int bincount{0};
  while (reader.Next()) {
    maxyear = std::max(maxyear, *year);
    minyear = std::min(minyear, *year);
  }
  bincount = maxyear - minyear + 1;
  reader.Restart();  // Restart reader to beginning of TTree};
  auto averageTempHist =
      new TH1D("averageTempHist", "Average Yearly Temperature", bincount,
               minyear, maxyear + 1);

  while (reader.Next()) {
    if (*year == minyear) {
      break;
    }
  }
  std::cout <<"Starting on year: " << minyear <<" and ending on year: " << maxyear << std::endl;
  int currentyear{*year};
  double FetchedTemp{0.0};
  double totaltemp{0.0};
  int i{0};
  int toti{0};
  while (reader.Next()) {        // Check through all entries in TTree
    if (*year != currentyear) {  // Fill histogram when year changes
      FetchedTemp = FetchedTemp / i;
      averageTempHist->Fill(currentyear, FetchedTemp);
      i = 0;
      FetchedTemp = 0.0;
      currentyear = *year;  // Reset parameters and go on to next year
      toti++;
      totaltemp += *temp;  // For total average

    } else{ 
      FetchedTemp += *temp;  // Sum temperatures for the year
      totaltemp += *temp;    // For total average
      i++;
      toti++;
    }
    
  }

  double totalaverageTemp = totaltemp / toti;
  std::cout << "Total average temperature: " << totalaverageTemp << " C"
            << std::endl;
  averageTempHist->GetXaxis()->CenterTitle();
  averageTempHist->GetYaxis()->CenterTitle();
  averageTempHist->GetYaxis()->SetTitle("Average Temperature [#circC]");
  averageTempHist->GetXaxis()->SetTitle("Year");
  averageTempHist->GetXaxis()->SetTitleOffset(1.2);
  averageTempHist->GetYaxis()->SetTitleOffset(1.1);
  averageTempHist->SetAxisRange(-5, 30, "Y");


  TFile* outputFile = new TFile("results/meanTemperature.root", "RECREATE");
  auto* c1 =
      new TCanvas("Average Temperature Graph", "Average Temperature Graph");

    averageTempHist->SetTitle("Average Yearly Temperature");
  outputFile->cd();
  gStyle->SetOptStat(0); // Remove hist stat default legend
  averageTempHist->Draw("P");
  c1->SaveAs("results/MeanYearlyTemperature.pdf");
  c1->Write();
  outputFile->Close();

};
