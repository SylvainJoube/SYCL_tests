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
// #include "intel_noinit_fix.h"

const int ACAT_START_TEST_INDEX  = 1;
const int ACAT_STOP_TEST_INDEX   = 2;
const int ACAT_RUN_COUNT         = 1;
const int ACAT_REPEAT_LOAD_COUNT = 100;

#define DATA_TYPE unsigned int // TODO : try with unsigned int
using data_type = DATA_TYPE;
//using data_type_sum = unsigned long long;
enum sycl_mode {shared_USM, device_USM, host_USM, accessors, glibc};
//enum dataset_type {implicit_USM, device_USM, host_USM, accessors};

unsigned long long PARALLEL_FOR_SIZE;// = 1024 * 32 * 8;// = M ; work items number
unsigned long long VECTOR_SIZE_PER_ITERATION;// = 1; // = L ; vector size per workitem (i.e. parallel_for task) = nb itérations internes par work item
unsigned long long BASE_VECTOR_SIZE_PER_ITERATION; // updated in list_devices()

sycl_mode CURRENT_MODE = sycl_mode::device_USM;

int MEMCOPY_IS_SYCL = 1;
int SIMD_FOR_LOOP = 1;
constexpr int USE_NAMED_KERNEL = 1; // Sandor does not support anonymous kernels.
constexpr bool KEEP_SAME_DATASETS = true; 
int USE_HOST_SYCL_BUFFER_DMA = 0; 

#define DATA_VERSION 7
#define DATA_VERSION_TRACCC 108 // 105

// number of diffrent datasets
#define DATASET_NUMBER 1

#define CHECK_SIMD_CPU false


#define INPUT_DATA_LENGTH PARALLEL_FOR_SIZE * VECTOR_SIZE_PER_ITERATION
#define OUTPUT_DATA_LENGTH PARALLEL_FOR_SIZE


#define INPUT_DATA_SIZE INPUT_DATA_LENGTH * sizeof(DATA_TYPE)
#define OUTPUT_DATA_SIZE OUTPUT_DATA_LENGTH * sizeof(DATA_TYPE)

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
// Warmup count : nombre d'itérations non comptabilisées pour ne pas mesurer
// les évènements réalisés en lazy.
int REPEAT_COUNT_ONLY_PARALLEL_WARMUP_COUNT = 0; // 4 défini dans le main (0)

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

std::string DEVICE_NAME_ON_BLOP_INTEL  = "???";
std::string DEVICE_NAME_ON_BLOP_NVIDIA = "NVIDIA GeForce GTX 780";

//std::string BENCHMARK_VERSION = "v06D";
std::string BENCHMARK_VERSION = "ubench" + std::to_string(DATA_VERSION); // Sandor compatible  v05
std::string BENCHMARK_VERSION_TRACCC = "sccl" + std::to_string(DATA_VERSION_TRACCC);
std::string DISPLAY_VERSION = BENCHMARK_VERSION_TRACCC + " - TRACCC-015";

// Not used anymore
//std::string TRACCC_OUT_FNAME = "tracccMemLocStrat7_sansGraphPtr";

// nombre de fois qu'il faut répéter le chargement des données
unsigned int base_traccc_repeat_load_count = 1; // actualisé dans utils.h : selector_list_devices
unsigned int traccc_repeat_load_count = 1;
const unsigned int traccc_repeat_load_count_ON_MSI_INTEL = 1;
const unsigned int traccc_repeat_load_count_ON_MSI_NVIDIA = 100;
const unsigned int traccc_repeat_load_count_ON_SANDOR = 100;
const unsigned int traccc_repeat_load_count_ON_THINKPAD = 1;
const unsigned int traccc_repeat_load_count_ON_BLOP_INTEL = 1;
const unsigned int traccc_repeat_load_count_ON_BLOP_NVIDIA = 100;

int traccc_SPARSITY_MIN = 0;
int traccc_SPARSITY_MAX = 100000;
bool traccc_sparsity_ignore = true;



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
std::string g_size_str = "0MiB";
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






struct s_computer {
    std::string fullName, toFileName, deviceName;
    uint repeat_load_count = 0;
    long long total_elements = 0;
    std::string size_str = "0MiB";
    uint L = 1;
};


const uint g_computer_count = 6;
s_computer g_computers[g_computer_count];

void init_computers() {
    s_computer * c;
    uint ci = 0;

    // 1
    c = &g_computers[ci++];
    c->fullName   = "Thinkpad";
    c->toFileName = "thinkpad";
    c->deviceName = "Intel(R) UHD Graphics 620 [0x5917]";
    c->repeat_load_count = 1;
    c->total_elements = 1024L * 1024L * 128L; // 128 milions elements * 4 bytes => 512 MiB
    c->size_str = "512MiB";
    c->L = 128;

    // 2
    c = &g_computers[ci++];
    c->fullName   = "MSI_Intel";
    c->toFileName = "msiIntel";
    c->deviceName = "???";
    c->repeat_load_count = 1;
    c->total_elements = 1024L * 1024L * 128L; // 128 milions elements * 4 bytes => 512 MiB
    c->size_str = "512MiB";
    c->L = 128;


    // 3
    c = &g_computers[ci++];
    c->fullName   = "MSI_Nvidia";
    c->toFileName = "msiNvidia";
    c->deviceName = "NVIDIA GeForce GTX 960M";
    c->repeat_load_count = 1;
    c->total_elements = 1024L * 1024L * 128L; // 128 milions elements * 4 bytes => 512 MiB
    c->size_str = "512MiB";
    c->L = 128;


    // 4
    c = &g_computers[ci++];
    c->fullName   = "Sandor";
    c->toFileName = "sandor";
    c->deviceName = "Quadro RTX 5000";
    c->repeat_load_count = 100; // TEMP ACAT
    c->total_elements = 1024L * 1024L * 256L * 6L; // 256 milions elements * 4 bytes => 1 GiB ; *6 => 6 GiB
    c->size_str = "6GiB";
    c->L = 128;


    // 5
    c = &g_computers[ci++];
    c->fullName   = "Blop_Intel";
    c->toFileName = "blopIntel";
    c->deviceName = "????";
    c->repeat_load_count = 1;
    c->total_elements = 1024L * 1024L * 128L; // 128 milions elements * 4 bytes => 512 MiB
    c->size_str = "512MiB";
    c->L = 128;

    // 6
    c = &g_computers[ci++];
    c->fullName   = "Blop_Nvidia";
    c->toFileName = "blopNvidia";
    c->deviceName = "GeForce GTX 780";//"NVIDIA GeForce GTX 780";
    c->repeat_load_count = 10; // benchs ACAT
    c->total_elements = 1024L * 1024L * 128L; // 128 milions elements * 4 bytes => 512 MiB
    c->size_str = "512MiB";
    c->L = 128;

}

std::string get_computer_name(uint computer_id) {
    if ( (computer_id > g_computer_count) || (computer_id == 0) )
        return "unknown_computer_id" + std::to_string(computer_id);
    
    return g_computers[computer_id - 1].fullName;
    
    /*switch (computer_id) {
    case 1 : return "Thinkpad";
    case 2 : return "MSI_Intel";
    case 3 : return "MSI_Nvidia";
    case 4 : return "Sandor";
    case 5 : return "Blop_Intel";
    case 6 : return "Blop_Nvidia";
    default : return "unknown_computer";
    }*/
}

/// For output file name.
std::string get_computer_name_ofile(uint computer_id) {
    if ( (computer_id > g_computer_count) || (computer_id == 0) )
        return "unknownComputerId" + std::to_string(computer_id);
    
    return g_computers[computer_id - 1].toFileName;

    /*switch (computer_id) {
    case 1 : return "thinkpad";
    case 2 : return "msiIntel";
    case 3 : return "msiNvidia";
    case 4 : return "sandor";
    case 5 : return "blopIntel";
    case 6 : return "blopNvidia";
    default : return "unknownComputer";
    }*/
}

std::string get_computer_device_name(uint computer_id) {
    if ( (computer_id > g_computer_count) || (computer_id == 0) )
        return "unknown_device_name_computer_id" + std::to_string(computer_id);
    
    return g_computers[computer_id - 1].deviceName;
}

uint get_computer_repeat_load_count(uint computer_id) {
    if ( (computer_id > g_computer_count) || (computer_id == 0) )
        return 0;
    
    return g_computers[computer_id - 1].repeat_load_count;
}
