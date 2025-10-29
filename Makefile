CXX := g++
CXXWARNINGS := -Wall -Wextra -Werror
CXXOPT := -O3
INCLUDES := -Iinclude -Iexternal/include
CXXFLAGS := $(CXXWARNINGS) $(CXXOPT) $(shell root-config --cflags) $(INCLUDES) 
ROOTFLAGS := $(shell root-config --libs)
ARGFLAGS := -Lexternal/lib64 -largumentum
LDFLAGS := $(ROOTFLAGS) $(ARGFLAGS)

.PHONY: all clean

all: main

main: main.cxx src/DataExtraction.cxx src/Measurement.cxx src/analysis3.cxx
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

src/%.o: src/%.cxx
	$(CXX) $(CXXFLAGS) $^ -c -o $@

clean:
	rm -v src/*.o main
