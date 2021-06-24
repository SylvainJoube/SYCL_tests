CXX = dpcpp
CXXFLAGS = -O0 -g -std=c++17 # -O2 -std=c++17

DIRS = bin
RUN_EXE_NAME = bin/memory_benchmark_file_output
#IMPLICIT_SOURCES = memory_benchmark.cpp

all: fileout_build

terminalout_build:
	$(CXX) $(CXXFLAGS) -o bin/memory_benchmark memory_benchmark.cpp

fileout_build:
	$(CXX) $(CXXFLAGS) -o bin/memory_benchmark_file_output memory_benchmark_file_output.cpp
	
#	$(CXX) $(CXXFLAGS) -o $(IMPLICIT_EXE_NAME) $(IMPLICIT_SOURCES)



gcc_build :
	g++ -O0 -g -std=c++17 -o bin/simple_stupid_gcc simple_stupid_gcc.cpp

run: 
	./$(RUN_EXE_NAME)

clean: 
	rm -f $(RUN_EXE_NAME)


# Creates needed directories
$(shell mkdir -p $(DIRS))