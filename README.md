# MNXB11-project
# About
This is the repository for team 9's MNXB11 project.

# Directory structure

The base directory here contains your "main" file, i.e. the C++ file that you
will be using as your starting point in your project. The code in this file should ideally be short and just make use of the functionality that you've
prepared in your other translation units. The majority of your code should be placed in  the two folders where you define your translation units, `src/` and `include/`.

Do remember to add your include guards to your header files, otherwise the compiler will
get multiple definitions if you end up loading two source files that include the
same header.

In the template you can find a small translation unit called Example (in [src/Example.cxx](src/Example.cxx) and [include/Example.h](include/Example.h)) that shows some commented reminders of how the syntax for some C++ constructs work. Feel free to use this as a reference to remind yourself of how to do something while working on it but make sure to remove it from your final project version!

There is a demonstration of a toy project you can use for inspiration at [EinarElen/MNXB11-project-demo](https://github.com/EinarElen/MNXB11-project-demo). You should not copy code from this repository. There are some intentional bugs hiding in there, see if you can spot them. 

We have also included three special files in the base of the repository 
- [.gitignore](.gitignore)
  - This file contains regular expressions that git tells git that it shouldn't add certain file to your repository. 
  - Your git repository should generally not contain binary files like object files or executables nor should it contain build artefacts like external libraries. 
- [.clang-format](.clang-format)
  - This file holds the configuration for the clang-format tool that you can use to format your code consistently 
  - It is a good idea to keep your code formatted in a consistent manner, especially when working in groups but doing it manually is a waste of your time. Use a tool for it!
  ```
  # Show what the src/Example.cxx file would look like if formatted
  clang-format src/Example.cxx 
  # Carry out the formatting in the file directly 
  clang-format src/Example.cxx -i
  ```
  - The `.clang-format` file holds the configuration that clang-format will use to determine how to format your code. By default, it will be formatted according to Google's style but you can pick any that you like from https://clang.llvm.org/docs/ClangFormatStyleOptions.html
- [rootlogon.C](rootlogon.C)
  - This file contains code that ROOT will execute automatically whenever you start it, a good place to place general style choices you want to make or anything else you always want to run! 
  - Be careful to not include anything that depends on your particular machine here (e.g. absolute paths)
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



## Analyses

### 5 Ice Cream Analysis
To be able to run the analysis, unzip the /datasets/ice_cream.tgz by the following command:

  tar -xvzf ./datasets/ice_cream.tgz

To run the analysis use the following executable call:

  ./main -i \<SMHI datafile path\> -a 5

Produces a plot (under /results) of monthly ice cream sales (IPN) and average monthly temperature recorded in the corresponding SMHI location. 

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










