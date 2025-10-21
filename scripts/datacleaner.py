#!/usr/bin/python3

import sys
import os

def usage(cmd: str):
  print("--------------------")
  print("This script is called using")
  print(f"{cmd} '<path-to-datafile>'")
  print("Example:")
  print(f"{cmd} './datasets/smhi-opendata_1_105370_20231007_154742_Falun.csv'")
  print("--------------------")

def main(filename: str):
  print(f"Cleaning up input file {filename}")

  output_folder = os.path.dirname(os.path.abspath(filename))

  try:
    with open(filename, mode='r') as f:
      print("Finding the first line containing 'datum'...")
      for i, line in enumerate(f):
        if "Datum" in line:
          startline = i+1
          break

    with open(filename, mode='r') as in_file, \
      open(output_folder + f"/baredata_{os.path.basename(filename)}", "w+") as out_file:
      print(f"Removing the first {startline} lines, selecting only the relevant columns and replacing ; with spaces.")
      for i, line in enumerate(in_file):
        if i>=startline:
          date, time, temp, quality = line.rstrip('\n').split(";")[:4]
          out_file.write(f"{date} {time} {temp} {quality}\n")

      print(f"Cleanup was successful. Result can be found in {os.path.relpath(out_file.name)}.")

  except:
      print("Input file could not be read, exiting...")
      sys.exit(1)


  return None

if __name__=="__main__":
  argv = sys.argv
  _cmd = argv[0]

  if len(argv) < 2:
    print("Missing input file parameter, exiting...")
    usage(_cmd)
    sys.exit(1)

  _filename = argv[1]

  if os.path.isdir(_filename):
    print("Input file is a directory and not a file, exiting...")
    usage(_cmd)
    sys.exit(1)

  main(_filename)
