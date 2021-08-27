CXX = syclcc
#dpcpp
CXXFLAGS = -O2 -std=c++17 # -O0 -g     -O2 -std=c++17

FILE_NAME = bench
#memory_benchmark_file_output_cmp_all
#chrono_test
#memory_benchmark_file_output

DIRS = bin
RUN_EXE_NAME = bin/$(FILE_NAME)
#IMPLICIT_SOURCES = memory_benchmark.cpp

all: build run

hello: build_hello run_hello

hello_named: build_hello_named run_hello_named

mbf: build_mbf run_mbf

#terminalout_build:
#	$(CXX) $(CXXFLAGS) -o bin/memory_benchmark memory_benchmark.cpp

build:
	$(CXX) $(CXXFLAGS) -o bin/$(FILE_NAME) $(FILE_NAME).cpp

build_hello:
	$(CXX) $(CXXFLAGS) -o bin/hello_world hello_world.cpp

run_hello: 
	./bin/hello_world


build_hello_named:
	$(CXX) $(CXXFLAGS) -o bin/hello_world_named hello_world_named.cpp


run_hello_named: 
	./bin/hello_world_named

# Testing launch wait and order
hello_sleep:
	$(CXX) $(CXXFLAGS) -o bin/hello_world_named_sleep hello_world_named_sleep.cpp
	./bin/hello_world_named_sleep 1
	./bin/hello_world_named_sleep 2
	./bin/hello_world_named_sleep 3
	./bin/hello_world_named_sleep 4

build_mbf:
	$(CXX) $(CXXFLAGS) -o bin/mbfoca_named mbfoca_named.cpp

run_mbf: 
	./bin/mbfoca_named

#	$(CXX) $(CXXFLAGS) -o $(IMPLICIT_EXE_NAME) $(IMPLICIT_SOURCES)



gcc_build :
	g++ -O0 -g -std=c++17 -o bin/simple_stupid_gcc simple_stupid_gcc.cpp

run: 
	./$(RUN_EXE_NAME)

#run_all:


clean: 
	rm -f $(RUN_EXE_NAME)


# Creates needed directories
$(shell mkdir -p $(DIRS))