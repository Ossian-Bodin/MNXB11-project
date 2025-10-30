#include <argumentum/argparse.h>

// #include "Analysis.h"
#include "DataExtraction.h"
#include "ConsDays.h"

#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
  
  // Create the parser object
  auto parser = argumentum::argument_parser{};

  // Initialize the name and a description for the program
  const std::string program_name{argv[0]};
  const std::string program_description{"Team 9's program for the MNXB11 project"};
  
  // Configure the parser with the given name and description
  auto parser_configuration{parser.config()};
  parser_configuration.program(program_name);
  parser_configuration.description(program_description);

  // Create the parameters object for our parser
  auto parameters = parser.params();

  // Initialize the string object that stores the input file path
  std::string input_file{};


  // Create the object for the input file parameter
  auto input_file_parameter{
      parameters.add_parameter(input_file, "-i", "--input-file")};
  // Tell the input file parameter that it is a required parameter
  input_file_parameter.required(true);
  // Tell the input file paratmeter that it should expect one and only one parameter
  input_file_parameter.nargs(1);
  input_file_parameter.help("The (cleaned up) data file with weather data");

  // Create a second parameter that lets the user specify which analysis is run
  int analysis_choice{};
  auto analysis_choice_parameter{
      parameters.add_parameter(analysis_choice, "-a", "--analysis")};
  analysis_choice_parameter.required(true);

  analysis_choice_parameter.nargs(1);
  analysis_choice_parameter.metavar("ANALYSIS-CHOICE");
  analysis_choice_parameter.help(
      "Choose which of the project's steps to run: 0 -> Run data extraction "
      "only and persist the raw data, 1 -> Run analysis 1 (),"
      " 2 -> Run analysis 2 () -> Run analysis 3 (),"
      " 4 -> Run analysis 4 () , 5 -> Run analysis 5 (),"
      "6 -> Run all five analyses");
  
  // Create a third parameter that lets the user specify the output file (not required)
  std::string output_file{};
  auto output_file_parameter{
    parameters.add_parameter(output_file, "-o", "--output")};
  output_file_parameter.nargs(1);
  output_file_parameter.default_value("results/output.root");
  output_file_parameter.help("Specify the output directory and output file name (not required).");

  bool successful_parse{parser.parse_args(argc, argv)};

  if (!successful_parse) {
    std::cout << "Something went wrong with the argument parsing" << std::endl;
    std::exit(1);
  }

  // Check if input_file is a cleaned data file, if not replace it with the clean file
  if (input_file.find("baredata_") == std::string::npos) {
    input_file = clean_data(input_file);
  }

  std::string output_directory{fs::path(output_file).parent_path()};

  if (!fs::is_directory(output_directory) || !fs::exists(output_directory)) {
    std::cout << "Creating output directory..." << std::endl;
    fs::create_directories(output_directory);
  }

  std::cout << "Running data extraction!" << std::endl;
  const auto measurements{read_measurements(input_file)};
  switch (analysis_choice) {
    case 0: {
      std::cout << "Persisting raw measurement data to file " << output_file
                << std::endl;
      persist_measurements(measurements, output_file);
      break;
    }
    case 1: {
      std::cout << "Running a first analysis" << std::endl;
      // some_analysis(measurements, output_file);
      break;
    }
    case 2: {
      std::cout << "Running a second analysis" << std::endl;

      auto res{getConsDays(measurements)};

      plotConsDaysHist(res);

      break;
    }
    case 3: {
      std::cout << "Running a third analysis" << std::endl;
      // some_other_analysis(measurements, output_file);
      break;
    }
    case 4: {
      std::cout << "Running a fourth analysis" << std::endl;
      // some_other_analysis(measurements, output_file);
      break;
    }
    case 5: {
      std::cout << "Running a third analysis" << std::endl;
      // some_other_analysis(measurements, output_file);
      break;
    }
    case 6: {
      std::cout << "Running all analyzes!" << std::endl;
      // some_analysis(measurements, output_file);
      // some_other_analysis(measurements, output_file);
      break;
    }
    default: {
      // We got an invalid analysis choice
      std::cerr << "Analysis choice must be one of either 0, 1, 2, 3, 5, or 6"
                << std::endl;
      std::exit(2);
    }
  }


  return 0;
}
