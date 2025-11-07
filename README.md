# MNXB11-project
## About
This is the repository for team 9's MNXB11 project.

## Directory structure

The root directory contains the following:

* `datasets/` 
    * This directory holds the (compressed) datasets that are used for the analysis. Instructions for how to extract the weather data sets are in the `README.md` in the directory and for analysis 5 (ice cream sales) you find instructions for how to extract it below.
* `include/`
    * Contains all the header files specific to this project.
* `src/`
    * Contains all the source files (except `main.cxx`) specific to this project.
* `scripts/`
    * Contains a Python script that cleans the weather data files.
* `main.cxx`
    * Main source file that combines the different analyses and handles CLI parsing.

## Building external libraries

### Argumentum

The `main.cxx` file makes use of the CLI parsing library `argumentum`. Follow these instructions to build the library. The following commands should be run starting from the root directory.
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

## Building the project

The `Makefile` can be used to compile the source files.

By default, the `all` target will be run which simply compiles `main`. To compile individual source files into object files one can run `make src/%.o` where `%` is replaced by the name of source file (excluding .cxx extension).

You can run the `clean` target to remove any object files that have been produced as well as the `main` executable.

## Running the program

Once the `argumentum` library has been installed, the data sets have been extracted, and the program compiled, the program can be run from the root directory using `./main -i <SMHI datafile path> -a ANALYSIS-CHOICE`. Running simply `./main` will show info on the the different arguments.

### 5. Ice Cream Analysis
To be able to run the analysis, unzip the `/datasets/ice_cream_dataset.tgz` by running the following command from the root directory:

  `tar -xvzf ./datasets/ice_cream_dataset.tgz -C ./datasets`

To run the analysis use the following executable call:

  `./main -i <SMHI datafile path> -a 5`

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

## Data preprocessing

Before extracting the data from the data sets it has to be preprocessed to remove any metadata and also be written in a form that can later be easily parsed. This data cleaning step is handled by the Python script `datacleaner.py` that can be found under `scripts/`. The script is supplied with a path to an (uncleaned) data CSV file, i.e. if ran from within the `scripts` directory one would run `datacleaner.py ../datasets/<SMHI data file>`. **Note:** the script requires the user to have a working python installation. Depending on your installation, the command above might not work and you can instead try invoking the python interpreter directly by running `python3 datacleaner.py ../datasets/<SMHI data file>`.

The actual "cleaning" happens within the `main` function within the `datacleaner.py` script. In the SMHI data sets the first lines are devoted to metadata, for instance the name of the weather station and measurement height. Since this data is not used directly in the analysis code it is stripped away. For example, below are the first lines in the Falsterbo data set:

```
Stationsnamn;Stationsnummer;Stationsnät;Mäthöjd (meter över marken)
Falsterbo;52230;SMHIs stationsnät;2.0

Parameternamn;Beskrivning;Enhet
Lufttemperatur;momentanvärde, 1 gång/tim;celsius

Tidsperiod (fr.o.m);Tidsperiod (t.o.m);Höjd (meter över havet);Latitud (decimalgrader);Longitud (decimalgrader)
1880-01-01 00:00:00;2023-10-01 06:20:09;1.541;55.3837;12.8167

Datum;Tid (UTC);Lufttemperatur;Kvalitet;;Tidsutsnitt:
1880-01-01;07:00:00;0.4;G;;Kvalitetskontrollerade historiska data (utom de senaste 3 mån)
1880-01-01;13:00:00;1.0;G;;Tidsperiod (fr.o.m.) = 1880-01-01 00:00:00 (UTC)
1880-01-01;20:00:00;1.7;G;;Tidsperiod (t.o.m.) = 2023-07-01 06:00:00 (UTC)
...
```

In addition, there is some additional metadata to the right of the first few measurements. To strip away the first lines devoted solely to metadata we look for the line begining with "Datum" (Swedish word for date). This is is done in the following code block:

```
with open(filename, mode='r') as f:
    print("Finding the first line containing 'datum'...")
    for i, line in enumerate(f):
        if "Datum" in line:
            startline = i+1
            break
```

First, we open the input file in read-only mode and assign it to the variable f. In Python it is then possible to loop over the individual lines within the file while also having a counter i keep track of which line it is currently on. This is contained in the third line above. It then checks for each line (interpreted as a single string) if it contains the substring "Datum", indicating that we are in the row above the measurements. If the substring is contained within that line we can therefore assign that line's index + 1 to a variable we call startline, which is simply the index of the line where the measurement data starts, and thereafter break the loop.


Having found the line where the measurement data starts, we can now loop through the file once again but now with knowledge of where to start reading the data. This is done in the following block:
```
with open(filename, mode='r') as in_file, \
    open(output_folder + f"/baredata_{os.path.basename(filename)}", "w+") as out_file:
    print(f"Removing the first {startline} lines, selecting only the relevant columns and replacing ; with spaces.")
    for i, line in enumerate(in_file):
    if i>=startline:
        date, time, temp, quality = line.rstrip('\n').split(";")[:4]
        out_file.write(f"{date} {time} {temp} {quality}\n")

    print(f"Cleanup was successful. Result can be found in {os.path.relpath(out_file.name)}.")
    return 0
```
Once again, the input file is opened in read-only mode assigned this time to a variable called in_file. At the same time, we open (and create if it does not already exist) a new file (out_file) in read and write mode in the same directory as the input file and with the same filename as the input file but prepended with "baredata_". This will be our ouput file. 

Looping over the lines in the input file as before we now check if the index is equal to or greater than the startline value we found earlier. If it is, we first strip the line (string) of any linebreaks ('\n'). In the example data above we can see that the data is separated using semi-colons. Hence, we then use the split function to turn it into a list containing the substrings separated by semi-colons (";"), i.e. the data. Lastly, to get rid of the extra metadata that appears in the first few lines we slice the list so as to only contain the first four columns corresponding to the date, time, temperature, and measurement quality. These are then assigned to corresponding variables. These values are then written to a single line in the output file in the same order as before but this time separated by spaces instead of semi-colons. After having looped through all the lines it returns 0 to indicate that the program executed succesfully. The above code blocks are themselves contained within a try block which in case of errors will divert to an except block which will print a message stating that the input file could not be read and then exit the program with exit code 1, indicating an error. Additionally, before ever calling the `main` function the script checks that the user actually inputted a file, and that they did not input a directory. If either of these fail it prints some usage instructions before exitting with error code 1.