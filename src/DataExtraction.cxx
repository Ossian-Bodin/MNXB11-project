#include "DataExtraction.h"
#include <filesystem>

namespace fs = std::filesystem;

std::string clean_data(std::string &raw_file) {
    // Define path to clean file
    std::string clean_file{fs::path(raw_file).relative_path().parent_path().u8string() + "/baredata_" + fs::path(raw_file).filename().u8string()};

    // Check if clean file already exists, if not clean it
    if (!fs::exists(clean_file)) {
        std::string command{"./scripts/datacleaner.py "};
        command += raw_file;
        int exitcode = system(command.c_str());
        if (exitcode != 0) {
            std::cerr << "Data cleaning script failed with exit code " << exitcode << std::endl;
            std::exit(exitcode);
        }
    }
    // Return path of clean file
    return clean_file;
}
std::vector<Measurement> read_measurements(
    const std::string& filename) {  // constructor initialization
  std::ifstream file{filename};
  std::vector<Measurement> measurements;
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  double temp;
  std::string quality;
  char dash;
  char colon;

  while (file >> year >> dash >> month >> dash >> day >> hour >> colon >>
         minute >> colon >> second >> temp >> quality) {
    Measurement measurement{year,   month,  day,  hour,
                            minute, second, temp, quality};
    measurements.push_back(measurement);
  }
  return measurements;
}

// function to read the data in datafiles and store them in a ROOT TTree
void persist_measurements(
    const std::vector<Measurement>& measurements,
    const std::string& output_filename) {  // constructor initialization
  TFile output_file{output_filename.c_str(), "RECREATE"};
  TTree tree{"weatherdata", "weatherdata"};

  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  double temp;
  std::string quality;
  tree.Branch("year", &year);
  tree.Branch("month", &month);
  tree.Branch("day", &day);
  tree.Branch("hour", &hour);
  tree.Branch("minute", &minute);
  tree.Branch("second", &second);
  tree.Branch("temp", &temp);
  tree.Branch("quality", &quality);

  for (const auto& measurement : measurements) {
    year = measurement.getYear();
    month = measurement.getMonth();
    day = measurement.getDay();
    hour = measurement.getHour();
    minute = measurement.getMinute();
    second = measurement.getSecond();
    temp = measurement.getTemperature();
    quality = measurement.isValid();
    tree.Fill();
  }
  tree.Write();
}
