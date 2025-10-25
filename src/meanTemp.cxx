#include <meanTemp.h>



//Fetch data from TTree, do mean temperature on data for each year, create new root file with histogram.
void tempReader(const std::string& filename){
    //Assuming TTree file already exists.
    auto file = TFile::Open(filename.c_str(), "READ");
       if (!file || file->IsZombie()) {
        std::cerr << "Error opening file, is it a TTree root file? " << filename << std::endl;
      return;}
    TTree *tree = dynamic_cast<TTree*>(file->Get("weatherdata"));
    TTreeReader reader{tree};
    TTreeReaderValue<int> year{reader, "year"};
    TTreeReaderValue<int> day{reader, "day"};
    TTreeReaderValue<double> temp{reader, "temp"};
    
    int minyear{1780};
    int maxyear{2023};
    int bins{maxyear - minyear + 1};
    auto averageTempHist = new TH1D("averageTempHist", "Average Yearly Temperature", bins, minyear, maxyear+1);
    
    
    reader.Next(); //Need this for initial value
    int currentyear{*year};
    double FetchedTemp{0.0};
     int i{0};
    while (reader.Next()){
            if (*year != currentyear){
                FetchedTemp = FetchedTemp/i;
                averageTempHist->Fill(*year, FetchedTemp);
                i = 0;
                FetchedTemp = 0.0;
                currentyear = *year;
                
            }
            else{
              FetchedTemp += *temp;
              i++;
            }
        }
    averageTempHist->GetXaxis()->CenterTitle();
	averageTempHist->GetYaxis()->CenterTitle();
	averageTempHist->GetYaxis()->SetTitle("Average Temperature (C)");
	averageTempHist->GetXaxis()->SetTitle("Year");
    averageTempHist->GetXaxis()->SetTitleOffset(1.2);
    averageTempHist->GetYaxis()->SetTitleOffset(1.1);
    averageTempHist->SetAxisRange(-5,30,"Y");
    TFile *outputFile = new TFile("results/meanTemperature.root", "RECREATE");
    outputFile->cd();
    averageTempHist->Write();

};




