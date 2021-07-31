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
// 
//using data_type_sum = unsigned long long;

enum sycl_mode {shared_USM, device_USM, host_USM, accessors};
//enum dataset_type {implicit_USM, device_USM, host_USM, accessors};

/*#define PARALLEL_FOR_SIZE 1024
#define VECTOR_SIZE_PER_ITERATION 200 * 1024*/

unsigned long long PARALLEL_FOR_SIZE;// = 1024 * 32 * 8;// = M ; work items number
unsigned long long VECTOR_SIZE_PER_ITERATION;// = 1; // = L ; vector size per workitem (i.e. parallel_for task) = nb itérations internes par work item

sycl_mode CURRENT_MODE = sycl_mode::device_USM;

int MEMCOPY_IS_SYCL = 1;
int SIMD_FOR_LOOP = 1;
constexpr int USE_NAMED_KERNEL = 1; // Sandor does not support anonymous kernels.
constexpr bool KEEP_SAME_DATASETS = true; 
//bool USE_HOST_SYCL_BUFFER = false; 

// faire un repeat sur les mêmes données pour essayer d'utiliser le cache
// hypothèse : les données sont évincées du cache avant de pouvoir y avoir accès
// observation : j'ai l'impression d'être un peu en train de me perdre dans les explorations,
// avoir une liste pour prioriser ce que je dois faire et 



// number of iterations - no realloc to make it go faster
#define REPEAT_COUNT_REALLOC 3
#define REPEAT_COUNT_ONLY_PARALLEL 0

//#define OUTPUT_FILE_NAME "sh_output_bench_h53.shared_txt"
//#define OUTPUT_FILE_NAME "msi_h60_L_M_128MiB_O0.t"

//#define OUTPUT_FILE_NAME "msi_L_M_512MiB_O2_SIMD_2.t"
//#define OUTPUT_FILE_NAME "sandor_L_M_6GiB_O2_SIMD_2.t"
//#define OUTPUT_FILE_NAME "msi_L_M_128MiB_O2_SIMD.t"
//#define OUTPUT_FILE_NAME "sandor_L_M_6GiB_O2.t"

//#define OUTPUT_FILE_NAME "msi_simd_1GiB_O2.t"
#define OUTPUT_FILE_NAME "msi_simd_1GiB_O2_debug.temp"
//#define OUTPUT_FILE_NAME "sandor_simd_6GiB_O2.t"
//#define OUTPUT_FILE_NAME "sandor_simd_6GiB_O2_debug_simd_temp.t"
//#define OUTPUT_FILE_NAME "sandor_simd_8GiB_O2_debug_simd_temp.t"

//#define OUTPUT_FILE_NAME "msi_alloc_1GiB_O2.t"
//#define OUTPUT_FILE_NAME "sandor_alloc_6GiB_O2.t"

//#define OUTPUT_FILE_NAME "sandor_h60_L_M_4GiB_O2.t"
//#define OUTPUT_FILE_NAME "msi_h60_alloclib_1GiB_O2.t"
//#define OUTPUT_FILE_NAME "msi_h60_simd_1GiB_O2_20pts.t"
//#define OUTPUT_FILE_NAME "T580_h60_L_M_128MiB.t"
//#define OUTPUT_FILE_NAME "T580_h60_simd_128MiB.t"

//const long long total_elements = 1024L * 1024L * 256L * 8L; // 8 GiB
//const long long total_elements = 1024L * 1024L * 256L * 6L; // 6 GiB
const long long total_elements = 1024L * 1024L * 256L; // 1 GiB
// 256 => 1 GiB 
// 128 => 512 MiB ; 
// 32  => 128 MiB ; 
// 256 * 4 bytes = 1   GiB.
// 32  * 4 bytes = 128 MiB.

// /!\ WARNING : do not forget to change to the desired function in the main
// of bench.cpp !

std::string ver_indicator = std::string("11");
std::string ver_prefix = OUTPUT_FILE_NAME + std::string(" - " + ver_indicator); // "X42"


#define DATA_VERSION 5

// number of diffrent datasets
#define DATASET_NUMBER 1

#define CHECK_SIMD_CPU false


#define INPUT_DATA_LENGTH PARALLEL_FOR_SIZE * VECTOR_SIZE_PER_ITERATION
#define OUTPUT_DATA_LENGTH PARALLEL_FOR_SIZE


#define INPUT_DATA_SIZE INPUT_DATA_LENGTH * sizeof(DATA_TYPE)
#define OUTPUT_DATA_SIZE OUTPUT_DATA_LENGTH * sizeof(DATA_TYPE)