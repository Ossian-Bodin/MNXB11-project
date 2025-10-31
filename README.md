# MNXB11-project
# About
This is the repository for team 9's MNXB11 project.

# Directory structure

The root directory contains the following:

* `datasets/` 
    * This directory holds the (compressed) datasets that are used for the analysis. Instructions for how to extract the weather data sets are in the `README.md` in the directory and for analysis 5 (ice cream sales) you find instructions for how to extract it below.
* `include/`
    * Contains all the header files specific to this project.
* `src/`
    * Contains all the source files (except `main.cxx`) specific to this project.
* `scripts/`
    * Contains a Python script that cleans the weather data files


# Building the project

The [datasets](datasets) folder contains open data from SMHI and a README.md with further information about it.

The `Makefile` can be used to compile the source files. If you create a new source file that will be used in `main.cxx` you will need to add it as a dependency of the `main` target.

By default, the `all` target will be run which 
- Compiles any `.cxx` files in the `src/` directory into object files 
- Compiles `main.cxx` and links with all the object files in `src/`

You can run the `clean` target to remove any object files that have been produced as well as the `main` executable.

## Building external libraries

### Argumentum

The `main.cxx` file makes use of the CLI parsing library `argumentum`. Follow these instructions to build the library. The following commands should be run from starting from the root directory.
``` sh
# Clone the library 
git clone https://github.com/mmahnic/argumentum # if not specified this automatically puts it into a directory called argumentum

mkdir build/argumentum -pv # -p tells mkdir to create the build/ directory if it does not already exist 
# Enter the build directory
cd build/argumentum

# Configure the build directory with CMake to install files in the external directory
cmake ../../argumentum -DCMAKE_INSTALL_PREFIX=../../external
# Build and install (use -jN to specify that you want to run it on N cores)
make -j4 install # To use four cores
```

## Running the program

Once the above library has been installed. One can run `make` in the root directory to compile the program into an executable called `main`. Once compiled, the program can be run from the root directory using `./main -i \<SMHI datafile path\> -a ANALYSIS-CHOICE`. Running simply `./main` will 

### 5. Ice Cream Analysis
To be able to run the analysis, unzip the `/datasets/ice_cream_dataset.tgz` by running the following command from root directory:

  `tar -xvzf ./datasets/ice_cream_dataset.tgz -C ./datasets`

To run the analysis use the following executable call:

  `./main -i \<SMHI datafile path\> -a 5`

Produces a plot (under `/results`) of monthly ice cream sales (IPN) and average monthly temperature recorded in the corresponding SMHI location. 

Note that the ice cream data set only covers years 1972-2019 (+ 2020 jan). Therefore, calling plotTempVsSales outside the scope startyear=1972 and stopyear=2019 will not work. 
Ideally stopyear=2020 should work, since we plot on a monthly basis, but the edge case bug is still eluding me.

METADATA:
The ice cream data set was collected from Kaggle.com, URL: https://www.kaggle.com/datasets/abdocan/monthly-ice-cream-sales-data-1972-2020

Quoted from Kaggle:
"This dataset represents monthly ice cream sales data recorded between January 1972 and January 2020. The dataset captures trends over time, showing patterns in sales, which can support time series analysis, trend modeling, and forecasting projects."

The .csv file is a comma-separated "yyyy-mm-dd", "IPN31152N", where the second is the industrial production number (IPN) of Industrial Production: Manufacturing: Nondurable Goods: Ice Cream and Frozen Dessert. 

ice_cream.csv:
DATE         The date of observation (in YYYY-MM-DD) format
IPN31152N	   Numeric values representing monthly ice cream sales (units sold)

Note that the IPN is a measure used by the Federal reserve, which likely means the ice_cream sales are recorded in or around the US (science).


TODO: fix the edge case 2020 bug.










