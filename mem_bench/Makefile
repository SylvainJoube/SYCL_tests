CXX = syclcc
#dpcpp
CXXFLAGS = -O0 -g -std=c++17 # -O2 -std=c++17

FILE_NAME = memory_benchmark_file_output_cmp_all
#chrono_test
#memory_benchmark_file_output

DIRS = bin
RUN_EXE_NAME = bin/$(FILE_NAME)
#IMPLICIT_SOURCES = memory_benchmark.cpp

all: build run

hello: build_hello run_hello

hello_named: build_hello_named run_hello_named

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

#	$(CXX) $(CXXFLAGS) -o $(IMPLICIT_EXE_NAME) $(IMPLICIT_SOURCES)



gcc_build :
	g++ -O0 -g -std=c++17 -o bin/simple_stupid_gcc simple_stupid_gcc.cpp

run: 
	./$(RUN_EXE_NAME)

clean: 
	rm -f $(RUN_EXE_NAME)


# Creates needed directories
$(shell mkdir -p $(DIRS))