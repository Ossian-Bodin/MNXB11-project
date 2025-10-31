# Workplan for MNXB11 project

Team number: 9
Team members: 5

## Plan

### Data preparation

 - [x] Write a script to clean up the data

### Data extraction 

 - [x] Write a function that extracts the necessary data in `main.cxx` which can be used by everyone.

### Analysis

Developers write their analysis in separate source/header files and test it in their `main.cxx` (should not be commited). The code should produce nice figures that will go in the report/presentation.

 1. The number of consecutive number of days that average temperature increases/decreases (Ossian)
 2. The temperature for every day of the year (Emma)
 3. The warmest and coldest day of each year (Nellie)
 4. The mean temperature of each year (Hugo)
 5. The number of ice creams sold versus temperature (Axel)

### Putting it together

We write a `main.cxx` that runs all of our individual analysis and create a Makefile that will 

  1. Put together the individual anlaysis into a main file
  2. Create a `Makefile` that compiles the entire program

### Submission 

Each person prepares their own analysis part for the presentation and writes it up in the report.

 1. Put together a presentation.
 2. Write together a report.


## GitHub workflow

Each developer does `git fetch` + `git merge` (`git pull`) from the `main` branch of `upstream` to their local `main`. Each developer writes their code in a `dev` branch (or branches off `dev` that they later merge with it). When they feel that a change is implemented they can `git push` to their remote repository and make a pull request from the remote `dev` branch into the `main` branch on upstream. Commits should only include source/header files and possibly changes to the README.md file if e.g. using external libraries that need to be built locally. The build directory and raw data files should not be committed except possibly compressed data files if the analysis uses some other data set.