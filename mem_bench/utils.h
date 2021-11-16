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
#include "constants.h"

/*
Here are some structs and useful functions that are not meant to change
very often.
*/



int mode_to_int(sycl_mode m) {
    switch (m) {
    case shared_USM : return 0;
    case device_USM : return 1;
    case host_USM : return 2;
    case accessors : return 3;
    case glibc : return 20;
    }
    return -1;
}

std::string mode_to_string(sycl_mode m) {
    switch (m) {
    case shared_USM : return "shared_USM";
    case device_USM : return "device_USM";
    case host_USM : return "host_USM";
    case accessors : return "accessors";
    case glibc : return "glibc";
    }
    return "unknown";
}





// SyCL asynchronous exception handler
// Create an exception handler for asynchronous SYCL exceptions
static auto exception_handler = [](cl::sycl::exception_list e_list) {
  for (std::exception_ptr const &e : e_list) {
    try {
      std::rethrow_exception(e);
    }
    catch (std::exception const &e) {
#if _DEBUG
      std::cout << "Failure" << std::endl;
#endif
      std::terminate();
    }
  }
};

class stime_utils {
private :
    std::chrono::_V2::steady_clock::time_point _start, _stop;

public :
    
    void start() {
        _start = std::chrono::steady_clock::now();
    }

    // Gets the us count since last start or reset.
    uint64_t reset() {
        std::chrono::duration<int64_t, std::nano> dur = std::chrono::steady_clock::now() - _start;
        _start = std::chrono::steady_clock::now();
        int64_t ns = dur.count();
        int64_t us = ns / 1000;
        return us;
    }

};

/*uint64_t get_ms() {
    auto tm = std::chrono::steady_clock::now();
    std::chrono::duration<double> s = tm - tm;


    struct timeval tp;
    gettimeofday(&tp, NULL);
    uint64_t ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    return ms;
}*/

void log(std::string str) {
    std::cout << str << std::endl;
}
void logs(std::string str) {
    std::cout << str << std::flush;
}

// level :
// 0 : important
// 1 : info
// 2 : flood

const int MAX_SHOWN_LEVEL = 1;

void log(std::string str, int level) {
    if (level <= MAX_SHOWN_LEVEL) {
        std::cout << str << std::endl;
    }
}

// Memory intensive operation (read only)
/*int compute_sum(int* array, int size) {
    int sum = 0;
    for (int i = 0; i < size; ++i) {
        sum += array[i];
    }
    return sum;
}*/


//static sycl_mode mode = sycl_mode::device_USM;
// static bool wait_queue = true;

struct host_dataset {
    //bool need_copy; // USM device needs a copy, but not host or shared.
    data_type *data_input;
    data_type *data_output;
    data_type final_result_verif = 0;
    unsigned int seed;
    // Memory allocated on the device : USM only
    data_type *device_input = nullptr;
    data_type *device_output = nullptr;
    // Buffers on the device for accessors-buffers
    // Those are pointers to be created during the allocation phase
    cl::sycl::buffer<data_type, 1> *buffer_input = nullptr;
    cl::sycl::buffer<data_type, 1> *buffer_output = nullptr;
};
unsigned int global_t_data_generation_and_ram_allocation = 0;

struct gpu_timer {
    uint64_t t_data_generation_and_ram_allocation = 0;
    uint64_t t_queue_creation = 0;
    uint64_t t_allocation = 0;

    // used if USE_HOST_SYCL_BUFFER_DMA = true
    uint64_t t_sycl_host_alloc = 0;
    uint64_t t_sycl_host_copy = 0;

    // if USE_HOST_SYCL_BUFFER_DMA, this is malloc_host -> shared/device/host
    // otherwise this is (classic buffer allocated with new) -> shared/device/host
    uint64_t t_copy_to_device = 0;

    uint64_t t_sycl_host_free = 0;

    uint64_t t_parallel_for = 0;
    uint64_t t_read_from_device = 0;
    uint64_t t_free_gpu = 0;
};

const bool SHOW_TIME_STATS = false;

void print_timer_iter_alloc(gpu_timer& time) {
    if ( ! SHOW_TIME_STATS ) return;
    uint64_t t_gpu;
    t_gpu = time.t_allocation + time.t_copy_to_device + time.t_read_from_device
            + time.t_parallel_for + time.t_free_gpu;
    std::cout 
            << "t_gpu - - - - - - - - - -  = " << t_gpu << std::endl
            //<< "t_queue_creation           = " << t_queue_creation << std::endl
            << "t_allocation - - - - - - - = " << time.t_allocation << std::endl
            << "t_copy_to_device           = " << time.t_copy_to_device << std::endl
            << "t_parallel_for - - - - - - = " << time.t_parallel_for << std::endl
            << "t_read_from_device         = " << time.t_read_from_device << std::endl
            << "t_free_gpu - - - - - - - - = " << time.t_free_gpu << std::endl
            ;

    log("");
}

void print_timer_iter(gpu_timer& time) {
    if ( ! SHOW_TIME_STATS ) return;
    //uint64_t t_gpu;
    //t_gpu = time.t_read_from_device + time.t_parallel_for;
    std::cout 
            << "t_parallel_for - - - - - - = " << time.t_parallel_for << std::endl
            << "t_read_from_device         = " << time.t_read_from_device << std::endl
            ;

    log("");
}

void print_timer_alloc(gpu_timer& time) {
    if ( ! SHOW_TIME_STATS ) return;
    uint64_t t_alloc_and_free;
    t_alloc_and_free = time.t_allocation + time.t_copy_to_device + time.t_free_gpu;
    std::cout 
            << "t_alloc_and_free - - - - - = " << t_alloc_and_free << std::endl
            << "t_allocation - - - - - - - = " << time.t_allocation << std::endl
            << "t_copy_to_device           = " << time.t_copy_to_device << std::endl
            << "t_free_gpu - - - - - - - - = " << time.t_free_gpu << std::endl
            ;

    log("");
}

host_dataset* generate_datasets(uint a_DATASET_NUMBER, uint a_INPUT_DATA_LENGTH,
                                uint a_OUTPUT_DATA_LENGTH, bool a_CHECK_SIMD_CPU,
                                uint a_PARALLEL_FOR_SIZE, uint a_VECTOR_SIZE_PER_ITERATION) {

    log("Generating data...", 1);
    stime_utils chrono;
    chrono.start();

    host_dataset *hdata = new host_dataset[a_DATASET_NUMBER];

    for (int i = 0; i < a_DATASET_NUMBER; ++i) {
        host_dataset *hd = &hdata[i];

        hd->data_input = new data_type[a_INPUT_DATA_LENGTH];
        hd->data_output = new data_type[a_OUTPUT_DATA_LENGTH];
        hd->seed = 452 + i * 68742;

        srand(hd->seed);

        // Fills the array with random data
        for (int i = 0; i < a_INPUT_DATA_LENGTH; ++i) {
            data_type v = rand();
            hd->data_input[i] = v;
            hd->final_result_verif += v;
        }

        // Perform SMID-like operations to verify the sum algorithm
        if (a_CHECK_SIMD_CPU) {
            data_type sum_simd_check_cpu = 0;
            // SIMD-like check
            for (int ip = 0; ip < a_PARALLEL_FOR_SIZE; ++ip) {
                for (int it = 0; it < a_VECTOR_SIZE_PER_ITERATION; ++it) {
                    int iindex = ip + it * a_PARALLEL_FOR_SIZE;
                    sum_simd_check_cpu += hd->data_input[iindex];
                }
            }

            // SMID-like OKAY VALLID - total sum = -1553315753
            if (sum_simd_check_cpu == hd->final_result_verif) {
                std::cout << "SMID-like OKAY VALLID - total sum = " << sum_simd_check_cpu << std::endl;
                std::cout << "SMID-like OKAY VALLID - total sum = " << sum_simd_check_cpu << std::endl;
                std::cout << "SMID-like OKAY VALLID - total sum = " << sum_simd_check_cpu << std::endl;
            } else {
                std::cout << "SMID-like ERROR should be " << hd->final_result_verif << " but is " << sum_simd_check_cpu << " - ERROR ERROR" << std::endl;
                std::cout << "SMID-like ERROR should be " << hd->final_result_verif << " but is " << sum_simd_check_cpu << " - ERROR ERROR" << std::endl;
                std::cout << "SMID-like ERROR should be " << hd->final_result_verif << " but is " << sum_simd_check_cpu << " - ERROR ERROR" << std::endl;
            }
        }
    }

    global_t_data_generation_and_ram_allocation = chrono.reset(); //get_ms() - t_start;

    return hdata;
}


void delete_datasets(host_dataset* hdata, uint a_DATASET_NUMBER) {
    if (hdata == nullptr) return;

    for (int i = 0; i < a_DATASET_NUMBER; ++i) {
        host_dataset *hd = &hdata[i];
        delete[] hd->data_input;
        delete[] hd->data_output;
    }
    delete[] hdata;
}

int total_main_seq_runs = 1;
//int current_main_seq_runs = 0;
int current_iteration_count = 0;

void init_progress() {
    total_main_seq_runs = 1;
    current_iteration_count = 0;
}

void print_total_progress() {
    const int total_iteration_count_per_seq = DATASET_NUMBER * (REPEAT_COUNT_REALLOC + REPEAT_COUNT_ONLY_PARALLEL
    + ( (REPEAT_COUNT_ONLY_PARALLEL == 0) ? 0 : REPEAT_COUNT_ONLY_PARALLEL_WARMUP_COUNT) );

    /*logs( "total_iteration_count_per_seq(" + std::to_string(total_iteration_count_per_seq) + ")"
    + " " +  );*/
    // dataset number * (number of iterations total, with warmup time if applicable)
    int total_iteration_count = total_iteration_count_per_seq * total_main_seq_runs;
    int progress = 100 * double(current_iteration_count) / double(total_iteration_count);
    logs( std::to_string(progress) + "% ");
}

// Only show once
static bool only_show_once_right_device_found_has_been_found = false;

/* Classes can inherit from the device_selector class to allow users
 * to dictate the criteria for choosing a device from those that might be
 * present on a system. This example looks for a device with SPIR support
 * and prefers GPUs over CPUs. */
class custom_device_selector : public cl::sycl::device_selector {
private:
    cl::sycl::default_selector def_selector;
public:
    custom_device_selector() : cl::sycl::device_selector() {}

    /* The selection is performed via the () operator in the base
    * selector class.This method will be called once per device in each
    * platform. Note that all platforms are evaluated whenever there is
    * a device selection. */
    int operator()(const cl::sycl::device& device) const override {
        
        if ( ! FORCE_EXECUTION_ON_NAMED_DEVICE ) {
            // Use the default recommended device
            return def_selector(device);
        } else {
            // Use the specified device
            // Multiple devices may have the same name but not the same score
            // so I return the device score if it has the right name
            // and -1 (i.e. never choose this one) otherwise.
            std::string devName =  device.get_info<cl::sycl::info::device::name>();
            if (devName.compare(MUST_RUN_ON_DEVICE_NAME) == 0) {
                int devScore = def_selector(device);
                if ( ! only_show_once_right_device_found_has_been_found) {
                    log("Right device found, score(" + std::to_string(devScore) + ") - " + devName);
                    only_show_once_right_device_found_has_been_found = true;
                }
                return devScore;
            }
            return -1;
        }
    }
};

class selector_list_devices : public cl::sycl::device_selector {
private:
    cl::sycl::default_selector def_selector;
public:
    selector_list_devices() : cl::sycl::device_selector() {}

    /* The selection is performed via the () operator in the base
    * selector class.This method will be called once per device in each
    * platform. Note that all platforms are evaluated whenever there is
    * a device selection. */
    int operator()(const cl::sycl::device& device) const override {
        
        // List device names and return the default score for the device
        std::string devName =  device.get_info<cl::sycl::info::device::name>();
        logs("    " + devName);

        std::string devType = "";
        switch (device.get_info<cl::sycl::info::device::device_type>()) {
        case cl::sycl::info::device_type::cpu :  devType = "cpu"; break;
        case cl::sycl::info::device_type::gpu :  devType = "gpu"; break;
        case cl::sycl::info::device_type::host : devType = "host"; break;
        default : devType = "unknown type"; break;
        }
        logs(" (" + devType + ")");

        int defaultScore = def_selector(device);

        log(" - score " + std::to_string(defaultScore));

        for (uint ic = 0; ic < g_computer_count; ++ic) {
            s_computer * c = & g_computers[ic];
            //log("Compare : " + devName + " <-> " + c->deviceName);
            if (devName.compare(c->deviceName) == 0) {
                currently_running_on_computer_id = ic + 1;
                MUST_RUN_ON_DEVICE_NAME = c->deviceName;
                base_traccc_repeat_load_count = c->repeat_load_count;

                total_elements = c->total_elements;
                g_size_str = c->size_str;
                BASE_VECTOR_SIZE_PER_ITERATION = c->L;

                log("==> Setting L(" + std::to_string(c->L) + ") total_elements(" + std::to_string(total_elements) + ") g_size_str(" + g_size_str + ")");
                break;
            }
        }

        /*if (devName.compare(DEVICE_NAME_ON_THINKPAD) == 0) {
            currently_running_on_computer_id = 1; // 1 Thinkpad
            MUST_RUN_ON_DEVICE_NAME = DEVICE_NAME_ON_THINKPAD;
            base_traccc_repeat_load_count = traccc_repeat_load_count_ON_THINKPAD;
        }
        if (devName.compare(DEVICE_NAME_ON_MSI_INTEL) == 0) {
            currently_running_on_computer_id = 2; // MSI Intel (no Nvidia device is visible with dpcpp)
            MUST_RUN_ON_DEVICE_NAME = DEVICE_NAME_ON_MSI_INTEL;
            base_traccc_repeat_load_count = traccc_repeat_load_count_ON_MSI_INTEL;
        }
        if (devName.compare(DEVICE_NAME_ON_MSI_NVIDIA) == 0) {
            currently_running_on_computer_id = 3; // MSI Nvidia (no Intel device is visible when using syclcc)
            MUST_RUN_ON_DEVICE_NAME = DEVICE_NAME_ON_MSI_NVIDIA;
            base_traccc_repeat_load_count = traccc_repeat_load_count_ON_MSI_NVIDIA;
        }
        if (devName.compare(DEVICE_NAME_ON_SANDOR) == 0) {
            currently_running_on_computer_id = 4; // 4 Sandor
            MUST_RUN_ON_DEVICE_NAME = DEVICE_NAME_ON_SANDOR;
            base_traccc_repeat_load_count = traccc_repeat_load_count_ON_SANDOR;
        }*/

        // Return the default device score
        return defaultScore;
    }
};

void list_devices(std::function<void(cl::sycl::exception_list)> func) {
    log("== List of available devices ==");
    selector_list_devices dev_list_select;
    cl::sycl::queue temp_queue(dev_list_select, func);
}


/*
Taken from : https://github.com/codeplaysoftware/computecpp-sdk/blob/master/samples/custom-device-selector.cpp#L46
pointed by the answer https://stackoverflow.com/questions/59061444/how-do-you-make-sycl-default-selector-select-an-intel-gpu-rather-than-an-nvidi

//class example_kernel;

// Classes can inherit from the device_selector class to allow users
// to dictate the criteria for choosing a device from those that might be
// present on a system. This example looks for a device with SPIR support
// and prefers GPUs over CPUs.
class custom_selector : public cl::sycl::device_selector {
private:
    cl::sycl::default_selector def_selector;
public:
    custom_selector() : cl::sycl::device_selector() {}

    // The selection is performed via the () operator in the base
    // selector class.This method will be called once per device in each
    // platform. Note that all platforms are evaluated whenever there is
    // a device selection.
    int operator()(const cl::sycl::device& device) const override {
        
        if ( ! FORCE_EXECUTION_ON_NAMED_DEVICE ) {
            // Use the recommended device
            return def_selector(device);
        } else {
            // Use the specified device
            // declared on constants.h : MUST_RUN_ON_DEVICE_NAME
            std::string devName =  device.get_info<cl::sycl::info::device::name>();
            if (devName.compare(MUST_RUN_ON_DEVICE_NAME) == 0) {
                return 100;
            }
            return -1;
        }

        // We only give a valid score to devices that support SPIR.
        if (device.has_extension(cl::sycl::string_class("cl_khr_spir")) ||
            device.has_extension(cl::sycl::string_class("cl_khr_il_program"))) {
            if (device.get_info<cl::sycl::info::device::device_type>() ==
                cl::sycl::info::device_type::cpu) {
                return 50;
            }

            if (device.get_info<cl::sycl::info::device::device_type>()
                == cl::sycl::info::device_type::gpu) {
                return 100;
            }
        }

        std::string devName =  device.get_info<cl::sycl::info::device::name>();

        logs("Device name = " + devName);
        if (devName.compare("Intel(R) UHD Graphics 620 [0x5917]") == 0) logs(" is my boooy and");

        if (device.get_info<cl::sycl::info::device::device_type>()
                == cl::sycl::info::device_type::gpu) {
                log(" is GPU.");
                return 100;
            }
        
        if (device.get_info<cl::sycl::info::device::device_type>()
                == cl::sycl::info::device_type::cpu) {
                log(" is CPU.");
                return 10;
            }
        log(" is something I don't know.");
        // Devices with a negative score will never be chosen.
        return -1;
    }
};

int example_custom_selector() {
  const int dataSize = 64;
  int ret = -1;
  float data[dataSize] = {0.f};

  cl::sycl::range<1> dataRange(dataSize);
  cl::sycl::buffer<float, 1> buf(data, dataRange);

  // We create an object of custom_selector type and use it
  // like any other selector.
  custom_selector selector;
  cl::sycl::queue myQueue(selector);

  myQueue.submit([&](cl::sycl::handler& cgh) {
    auto ptr = buf.get_access<cl::sycl::access::mode::read_write>(cgh);

    cgh.parallel_for<example_kernel>(dataRange, [=](cl::sycl::item<1> item) {
      size_t idx = item.get_linear_id();
      ptr[item.get_linear_id()] = static_cast<float>(idx);
    });
  });

  // A host accessor can be used to force an update from the device to the
  // host, allowing the data to be checked.
  cl::sycl::accessor<float, 1, cl::sycl::access::mode::read_write, cl::sycl::access::target::host_buffer>
      hostPtr(buf);

  if (hostPtr[10] == 10.0f) {
    ret = 0;
  }

  return ret;
}*/

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

long GetFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

inline bool file_exists_test0 (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}