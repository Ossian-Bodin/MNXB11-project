#include "DataExtraction.h"


std::vector<Measurement> read_measurements(const std::string& filename){ //constructor initialization 
    std::ifstream file{filename};
    int year, int month, int day, int hour, int minute, int second, int temp, std::string quality;
    char space;
    char dash;  
    char colon;


    while(file >> year >> dash >> month >> dash >> day >> space >> hour >> colon >> minute >> colon >> second >> space >> temp >> space >> quality){
        Measurement measurement{year, month, day, hour, minute, temp, quality};
        measurements.push_back(measurement);
    }
    return measurements;
}

// function to read the data in datafiles and store them in a ROOT TTree
void persist_measurements(const std::vector<Measurements>& measurements, const std::string& output_filename){ //constructor initialization
    TFile output_file{output_filename.root, "RECREATE"};
    TTree tree{"weatherdata", "weatherdata"};

    int year, int month, int day, int hour, int minute, int second, int temp, std::string quality;
    tree.Branch("year", &year);
    tree.Branch("month", &month);
    tree.Branch("day", &day);
    tree.Branch("hour", &hour);
    tree.Branch("minute", &minute);
    tree.Branch("second", &second):
    tree.Branch("temp", &temp);
    tree.Branch("quality", &quality);

    for (const auto measurement : measurements){
        year = getYear();
        month = getMonth();
        day = getDay();
        hour = getHour();
        minute = getMinute();
        second = getSecond();
        temp = getTemp();
        quality = isValid();
        tree.Fill();
    }
    tree.Write();

}
