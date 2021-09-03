#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>

// SyCL specific includes
#include <CL/sycl.hpp>
#include <array>
#include <sys/time.h>
#include <stdlib.h>

#define DATA_TYPE unsigned int // TODO : try with unsigned int
using data_type = DATA_TYPE;
//using data_type_sum = unsigned long long;
enum sycl_mode {shared_USM, device_USM, host_USM, accessors, glibc};
//enum dataset_type {implicit_USM, device_USM, host_USM, accessors};

unsigned long long PARALLEL_FOR_SIZE;// = 1024 * 32 * 8;// = M ; work items number
unsigned long long VECTOR_SIZE_PER_ITERATION;// = 1; // = L ; vector size per workitem (i.e. parallel_for task) = nb itérations internes par work item

sycl_mode CURRENT_MODE = sycl_mode::device_USM;

int MEMCOPY_IS_SYCL = 1;
int SIMD_FOR_LOOP = 1;
constexpr int USE_NAMED_KERNEL = 1; // Sandor does not support anonymous kernels.
constexpr bool KEEP_SAME_DATASETS = true; 
int USE_HOST_SYCL_BUFFER_DMA = 0; 

// faire un repeat sur les mêmes données pour essayer d'utiliser le cache
// hypothèse : les données sont évincées du cache avant de pouvoir y avoir accès
// observation : j'ai l'impression d'être un peu en train de me perdre dans les explorations,
// avoir une liste pour prioriser ce que je dois faire et 


// SEE main on bench.cpp
// SEE main on bench.cpp
// SEE main on bench.cpp
// SEE main on bench.cpp
// SEE main on bench.cpp
// SEE main on bench.cpp
// SEE main on bench.cpp


// number of iterations - no realloc to make it go faster
int REPEAT_COUNT_REALLOC;// défini dans le main (3)
int REPEAT_COUNT_ONLY_PARALLEL; // défini dans le main (0)

bool FORCE_EXECUTION_ON_NAMED_DEVICE = true;
std::string MUST_RUN_ON_DEVICE_NAME = "Intel(R) UHD Graphics 620 [0x5917]"; //std::string("s");

// How many times the sum should be repeated
// (to test caches and data access speed)
uint REPEAT_COUNT_SUM = 1;

//#define OUTPUT_FILE_NAME "sh_output_bench_h53.shared_txt"
//#define OUTPUT_FILE_NAME "msi_h60_L_M_128MiB_O0.t"

//#define OUTPUT_FILE_NAME "msi_L_M_512MiB_O2_SIMD_2.t"
//#define OUTPUT_FILE_NAME "sandor_L_M_6GiB_O2_SIMD_2.t"
//#define OUTPUT_FILE_NAME "msi_L_M_128MiB_O2_SIMD.t"
//#define OUTPUT_FILE_NAME "sandor_L_M_6GiB_O2.t"

//#define OUTPUT_FILE_NAME "msi_simd_1GiB_O2.t"
//#define OUTPUT_FILE_NAME "msi_simd_1GiB_O2_debug.temp"
//#define OUTPUT_FILE_NAME "sandor_simd_6GiB_O2.t"
//#define OUTPUT_FILE_NAME "sandor_simd_6GiB_O2_debug_simd_temp.t"
//#define OUTPUT_FILE_NAME "sandor_simd_8GiB_O2_debug_simd_temp.t"

// A device name that can be used to identify the computer
// the program is running on
std::string DEVICE_NAME_ON_THINKPAD   = "Intel(R) UHD Graphics 620 [0x5917]";
std::string DEVICE_NAME_ON_MSI_INTEL  = "???";
std::string DEVICE_NAME_ON_MSI_NVIDIA = "NVIDIA GeForce GTX 960M";
std::string DEVICE_NAME_ON_SANDOR     = "Quadro RTX 5000";

//std::string BENCHMARK_VERSION = "v06D";
std::string BENCHMARK_VERSION = "v05_TEMP"; // Sandor compatible
std::string DISPLAY_VERSION   = "v05_TEMP - TRACCC-009";

std::string TRACCC_OUT_FNAME = "tracccMemLocStrat7_sansGraphPtr";

// nombre de fois qu'il faut répéter le chargement des données
unsigned int traccc_repeat_load_count = 10;

uint currently_running_on_computer_id = 0; // 1 thinkpad, 2 msi Intel (dpcpp), 3 msi Nvidia (syclcc), 4 sandor
// les valeurs 2 et 3 sont équivalentes ici.
/*
case 1 : return "T580";
case 2 : return "MSI Intel";
case 3 : return "MSI Nvidia";
case 4 : return "SANDOR";
*/

// OUTPUT_FILE_NAME is now obsolete
std::string OUTPUT_FILE_NAME = "thinkpad_dma_1GiB_O2.t";
//#define OUTPUT_FILE_NAME "msi_dma_1GiB_O2.t"
//#define OUTPUT_FILE_NAME "sandor_dma_1GiB_O2.t"
//#define OUTPUT_FILE_NAME "msi_dma_512MiB_O2.t"

//#define OUTPUT_FILE_NAME "msi_alloc_1GiB_O2.t"
//#define OUTPUT_FILE_NAME "sandor_alloc_6GiB_O2.t"

//#define OUTPUT_FILE_NAME "sandor_h60_L_M_4GiB_O2.t"
//#define OUTPUT_FILE_NAME "msi_h60_alloclib_1GiB_O2.t"
//#define OUTPUT_FILE_NAME "msi_h60_simd_1GiB_O2_20pts.t"
//#define OUTPUT_FILE_NAME "T580_h60_L_M_128MiB.t"
//#define OUTPUT_FILE_NAME "T580_h60_simd_128MiB.t"

//const long long total_elements = 1024L * 1024L * 256L * 8L; // 8 GiB
//const long long total_elements = 1024L * 1024L * 256L * 6L; // 6 GiB
long long total_elements = 1024L * 1024L * 256L; // 1 GiB
//const long long total_elements = 1024L * 1024L * 128L; // 512 MiB
// 256 => 1 GiB 
// 128 => 512 MiB ; 
// 32  => 128 MiB ; 
// 256 * 4 bytes = 1   GiB.
// 32  * 4 bytes = 128 MiB.

// /!\ WARNING : do not forget to change to the desired function in the main
// of bench.cpp !

std::string ver_indicator = std::string("13d");

// ver_prefix is now obsolete
std::string ver_prefix = OUTPUT_FILE_NAME + std::string(" - " + ver_indicator); // "X42"


#define DATA_VERSION 7
#define DATA_VERSION_TRACCC 103

// number of diffrent datasets
#define DATASET_NUMBER 1

#define CHECK_SIMD_CPU false


#define INPUT_DATA_LENGTH PARALLEL_FOR_SIZE * VECTOR_SIZE_PER_ITERATION
#define OUTPUT_DATA_LENGTH PARALLEL_FOR_SIZE


#define INPUT_DATA_SIZE INPUT_DATA_LENGTH * sizeof(DATA_TYPE)
#define OUTPUT_DATA_SIZE OUTPUT_DATA_LENGTH * sizeof(DATA_TYPE)

std::string get_computer_name(int computer_id) {
    switch (computer_id) {
    case 1 : return "Thinkpad";
    case 2 : return "MSI_Intel";
    case 3 : return "MSI_Nvidia";
    case 4 : return "Sandor";
    default : return "unknown computer";
    }
}

/// For output file name.
std::string get_computer_name_ofile(int computer_id) {
    switch (computer_id) {
    case 1 : return "thinkpad";
    case 2 : return "msiIntel";
    case 3 : return "msiNvidia";
    case 4 : return "sandor";
    default : return "unknownComputer";
    }
}