#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>

// SyCL specific includes
#include <CL/sycl.hpp>
#include <array>
#include <sys/time.h>
#include <stdlib.h>

#include "utils.h"
#include "constants.h"


// Comparison of various USM flavours


void generic_USM_compute(cl::sycl::queue &sycl_q, host_dataset* dataset,
                          gpu_timer& timer, sycl_mode mode) {
    stime_utils chrono;
    chrono.start();

    //  data_type* ddata_input, data_type* ddata_output,
    data_type* ddata_input = dataset->device_input;
    data_type* ddata_output = dataset->device_output;

    //data_type* ddata_output_verif = static_cast<data_type *> (cl::sycl::malloc_device(OUTPUT_DATA_SIZE, sycl_q));

    const unsigned int local_VECTOR_SIZE_PER_ITERATION = VECTOR_SIZE_PER_ITERATION;
    const unsigned int local_PARALLEL_FOR_SIZE = PARALLEL_FOR_SIZE;

    // Sandor does not support anonymous kernels.

    class MyKernel_a;
    class MyKernel_b;

    if (SIMD_FOR_LOOP == 0) {
        //logs(" -NOT simd- ");
        // Starts a kernel - traditional for loop
        auto e = sycl_q.parallel_for<MyKernel_a>(cl::sycl::range<1>(PARALLEL_FOR_SIZE), [=](cl::sycl::id<1> chunk_index) {
            int cindex = chunk_index[0];
            int start_index = cindex * local_VECTOR_SIZE_PER_ITERATION;
            int stop_index = start_index + local_VECTOR_SIZE_PER_ITERATION;
            data_type sum = 0;

            for (int i = start_index; i < stop_index; ++i) {
                sum += ddata_input[i];
            }

            ddata_output[cindex] = sum;
            //ddata_output_verif[cindex] = sum;
        });
        e.wait();
    } else {
        //logs(" -IS simd- ");
        // Starts a kernel - SIMD optimized for loop
        auto e = sycl_q.parallel_for<MyKernel_b>(cl::sycl::range<1>(PARALLEL_FOR_SIZE), [=](cl::sycl::id<1> chunk_index) {
            int cindex = chunk_index[0];
            data_type sum = 0;

            for (int it = 0; it < local_VECTOR_SIZE_PER_ITERATION; ++it) {
                int iindex = cindex + it * local_PARALLEL_FOR_SIZE;
                sum += ddata_input[iindex];
            }

            ddata_output[cindex] = sum;
            //ddata_output_verif[cindex] = sum;
        });
        e.wait();
    }

    sycl_q.wait_and_throw();
    timer.t_parallel_for = chrono.reset();

    //std::cout << ver_prefix + " - COMPUTE MODE = " + mode_to_string(mode) << std::endl;
    if ( mode == sycl_mode::device_USM ) {
        //std::cout << "MODE = DEVICE USM OK" << std::endl;
        //if (MEMCOPY_IS_SYCL) 
        sycl_q.memcpy(dataset->data_output, ddata_output, OUTPUT_DATA_SIZE);
        sycl_q.wait_and_throw();
    }

    //sycl_q.memcpy(dataset->data_output, ddata_output_verif, OUTPUT_DATA_SIZE);
    //sycl_q.wait_and_throw();

    data_type sum_simd_check_cpu = 0;

    // Value verification
    data_type total_sum = 0;
    if ( mode == sycl_mode::device_USM ) {
        // device USM : memory has been copied to dataset->data_output
        // because data in dataset->device_output is only accessible by the device
        for (int i = 0; i < OUTPUT_DATA_LENGTH; ++i) {
            total_sum += dataset->data_output[i]; //ddata_output_verif[i];//dataset->data_output[i];
        }
    }

    if ( (mode == sycl_mode::host_USM)
    ||   (mode == sycl_mode::shared_USM) ) {
        // memory accessible by the host : data already in dataset->device_output
        // and since data is shared, it can be accessed directly
        for (int i = 0; i < OUTPUT_DATA_LENGTH; ++i) {
            total_sum += dataset->device_output[i]; //ddata_output_verif[i];//dataset->data_output[i];
        }
    }

    //cl::sycl::free(ddata_output_verif, sycl_q);
    timer.t_read_from_device = chrono.reset();

    if (total_sum == dataset->final_result_verif) {
        //log("VALID - Right data size ! (" + std::to_string(total_sum) + ")", 1);
    } else {
        log("ERROR on compute - expected size " + std::to_string(dataset->final_result_verif) + " but found " + std::to_string(total_sum) + ".", 1);
    }
}





void generic_USM_allocation(cl::sycl::queue &sycl_q, host_dataset *dataset, gpu_timer& timer, sycl_mode mode) {
    stime_utils chrono;
    chrono.start();
    
    //std::cout << ver_prefix + " - ALLOC MODE = " + mode_to_string(mode) << std::endl;
    switch (mode) {
    case sycl_mode::device_USM :
        dataset->device_input = static_cast<data_type *> (cl::sycl::malloc_device(INPUT_DATA_SIZE, sycl_q));
        dataset->device_output = static_cast<data_type *> (cl::sycl::malloc_device(OUTPUT_DATA_SIZE, sycl_q));
        break;
    case sycl_mode::host_USM :
        dataset->device_input = static_cast<data_type *> (cl::sycl::malloc_host(INPUT_DATA_SIZE, sycl_q));
        dataset->device_output = static_cast<data_type *> (cl::sycl::malloc_host(OUTPUT_DATA_SIZE, sycl_q));
        break;
    case sycl_mode::shared_USM :
        dataset->device_input = static_cast<data_type *> (cl::sycl::malloc_shared(INPUT_DATA_SIZE, sycl_q));
        dataset->device_output = static_cast<data_type *> (cl::sycl::malloc_shared(OUTPUT_DATA_SIZE, sycl_q));
        break;
    default : break; // TODO : add buffers/accessors
    }
    
    sycl_q.wait_and_throw();
    timer.t_allocation = chrono.reset();

    if (USE_HOST_SYCL_BUFFER) {
        //logs("SYCL hbuffer");
        // Copy from glibc buffer to SYCL malloc_host
        // and then from this malloc_host buffer to shared/device/host buffer

        // Only supports MEMCOPY_IS_SYCL, so I'll assume (MEMCOPY_IS_SYCL == true)

        // USM
        if ( (mode == sycl_mode::device_USM)
        ||   (mode == sycl_mode::host_USM)
        ||   (mode == sycl_mode::shared_USM) ) {
            // copy from glibc buffer to SYCL buffer malloc_host
            data_type* sycl_host_ds = static_cast<data_type *> (cl::sycl::malloc_host(INPUT_DATA_SIZE, sycl_q));
            timer.t_sycl_host_alloc = chrono.reset();

            sycl_q.memcpy(sycl_host_ds, dataset->data_input, INPUT_DATA_SIZE).wait();
            sycl_q.wait_and_throw();
            timer.t_sycl_host_copy = chrono.reset();

            // Copy from host SYCL buffer malloc_host to shared/device/host buffer
            sycl_q.memcpy(dataset->device_input, sycl_host_ds, INPUT_DATA_SIZE).wait();
            sycl_q.wait_and_throw();
            timer.t_copy_to_device = chrono.reset();

            cl::sycl::free(sycl_host_ds, sycl_q);
            sycl_q.wait_and_throw();
            timer.t_sycl_host_free = chrono.reset();
        }

    } else { // directly copy from glibc buffer to SYCL
        //logs("classic buffer");
        if ( (mode == sycl_mode::device_USM)
        ||   (mode == sycl_mode::host_USM)
        ||   (mode == sycl_mode::shared_USM) ) {

            // The only way to copy data to the device is to use sycl_q.memcpy
            // For shared memory (host and shared), glibc memcpy can be used.
            if ( (MEMCOPY_IS_SYCL == 1) || (mode == sycl_mode::device_USM) ) {
                log("MEM - SYCL MEMCOPY ----", 2);
                sycl_q.memcpy(dataset->device_input, dataset->data_input, INPUT_DATA_SIZE).wait();
                sycl_q.wait_and_throw();
            } else {
                log("MEM - GLIBC MEMCOPY ----", 2);
                // Probably works with host and shared, most likely does not work with device
                memcpy(dataset->device_input, dataset->data_input, INPUT_DATA_SIZE);
            }
            timer.t_copy_to_device = chrono.reset();
            //timer.t_sycl_host_alloc = 0;
            //timer.t_sycl_host_copy = 0;
            //timer.t_sycl_host_free = 0;
        }
    }

    // TODO : also copy data from dataset to shared memory.
}




void generic_USM_free(cl::sycl::queue &sycl_q, host_dataset* dataset, gpu_timer& timer, sycl_mode mode) {
    stime_utils chrono;
    chrono.start();
    //std::cout << ver_prefix + " - FREE MODE = " + mode_to_string(mode) << std::endl;
    cl::sycl::free(dataset->device_input, sycl_q);
    cl::sycl::free(dataset->device_output, sycl_q);
    sycl_q.wait_and_throw();
    timer.t_free_gpu = chrono.reset();
    dataset->device_input = nullptr;
    dataset->device_output = nullptr;
}



host_dataset* generate_datasets() {
    return
    generate_datasets(DATASET_NUMBER, INPUT_DATA_LENGTH, OUTPUT_DATA_LENGTH,
                     CHECK_SIMD_CPU, PARALLEL_FOR_SIZE, VECTOR_SIZE_PER_ITERATION);
}

void delete_datasets(host_dataset* hdata) {
    delete_datasets(hdata, DATASET_NUMBER);
}

host_dataset *global_persistent_datasets = nullptr;

void main_sequence(std::ofstream& write_file, sycl_mode mode) {

    // Pointers to allocation, compute and free SYCL functions.
    void (*sycl_allocation)(cl::sycl::queue &, host_dataset *, gpu_timer &, sycl_mode mode);
    void (*sycl_compute)(cl::sycl::queue &, host_dataset *, gpu_timer &, sycl_mode mode);
    void (*sycl_free)(cl::sycl::queue &, host_dataset *, gpu_timer &, sycl_mode mode);

    switch (mode) {
    case sycl_mode::device_USM :
        sycl_allocation = generic_USM_allocation;
        sycl_compute = generic_USM_compute;
        sycl_free = generic_USM_free;
        break;

    case sycl_mode::host_USM :
        sycl_allocation = generic_USM_allocation;
        sycl_compute = generic_USM_compute;
        sycl_free = generic_USM_free;
        break;

    case sycl_mode::shared_USM :
        sycl_allocation = generic_USM_allocation;
        sycl_compute = generic_USM_compute;
        sycl_free = generic_USM_free;
        break;

    default : break; // TODO : add buffers/accessors
    }

    uint64_t t_start, t_start2;
    gpu_timer gtimer;

    stime_utils chrono;

    //log("Generating data...");

    chrono.start();
    //t_start = get_ms();
    host_dataset *hdata;

    if (KEEP_SAME_DATASETS) {
        if (global_persistent_datasets == nullptr) {
            // First generation, if no dataset already present
            global_persistent_datasets = generate_datasets();
        }
        hdata = global_persistent_datasets;
    } else {
        // New dataset for each run of main_sequence
        hdata = generate_datasets();
    }

    gtimer.t_data_generation_and_ram_allocation = global_t_data_generation_and_ram_allocation; //chrono.reset(); //get_ms() - t_start;

    log("Input data size  : " + std::to_string(INPUT_DATA_SIZE)
        + " (" + std::to_string(INPUT_DATA_SIZE / (1024*1024)) + " MiB)", 1);
    log("Output data size : " + std::to_string(OUTPUT_DATA_SIZE)
        + " (" + std::to_string(OUTPUT_DATA_SIZE / (1024*1024)) + " MiB)", 1);

    // The default device selector will select the most performant device.
    cl::sycl::default_selector d_selector;

    try {
        chrono.reset(); //t_start = get_ms();
        cl::sycl::queue sycl_q(d_selector, exception_handler);
        sycl_q.wait_and_throw();
        gtimer.t_queue_creation = chrono.reset();//get_ms() - t_start;

        // Print out the device information used for the kernel code.
        log("--   " + sycl_q.get_device().get_info<cl::sycl::info::device::name>() + "   --");
        
        write_file << DATASET_NUMBER << " "
                    << INPUT_DATA_SIZE << " "
                    << OUTPUT_DATA_SIZE << " "
                    << PARALLEL_FOR_SIZE << " "
                    << VECTOR_SIZE_PER_ITERATION << " "
                    << REPEAT_COUNT_REALLOC << " "
                    << REPEAT_COUNT_ONLY_PARALLEL << " "
                    << gtimer.t_data_generation_and_ram_allocation << " "
                    << gtimer.t_queue_creation << " "
                    << mode_to_int(mode) << " "
                    << MEMCOPY_IS_SYCL << " " // flag to indicate if sycl mem copy or glibc mem copy
                    << SIMD_FOR_LOOP << " " // flag to indicate wether a traditional for loop was used, or a SIMD GPU-specific loop
                    << USE_NAMED_KERNEL << " " // flag to indicate if the named kernel was used or traditional lambda kernel
                    // wether there is a copy from SYCL host to device, or normal (allocated by new) copy to device, to test DMA access
                    << USE_HOST_SYCL_BUFFER 
                    << "\n";

        log("\n######## ALLOCATION, COPY AND FREE FOR EACH ITERATION ########", 2);

        // Allocation, copy and free each time
        for (int ids = 0; ids < DATASET_NUMBER; ++ids) {
            
            host_dataset* dataset = &hdata[ids];
            write_file << dataset->seed << "\n";

            log("------- DATASET SEED " + std::to_string(dataset->seed) + " -------\n", 2);

            // Allocation and free on device, for each iteration
            for (int rpt = 0; rpt < REPEAT_COUNT_REALLOC; ++rpt) {
                log("Iteration " + std::to_string(rpt+1) + " on " + std::to_string(REPEAT_COUNT_REALLOC), 2);

                sycl_allocation(sycl_q, dataset, gtimer, mode);
                sycl_compute(sycl_q, dataset, gtimer, mode);
                sycl_free(sycl_q, dataset, gtimer, mode);

                write_file << gtimer.t_allocation << " "
                            << gtimer.t_sycl_host_alloc << " " // v6
                            << gtimer.t_sycl_host_copy << " " // v6
                            << gtimer.t_copy_to_device << " "
                            << gtimer.t_sycl_host_free << " " // v6
                            // TODO : do the same with alloc/cpy only once
                            << gtimer.t_parallel_for << " " 
                            << gtimer.t_read_from_device << " "
                            << gtimer.t_free_gpu
                            << "\n";
                
                // A new line for each repeat count :
                // t_allocation t_copy_to_device t_parallel_for t_read_from_device t_free_gpu
                print_timer_iter_alloc(gtimer);

                ++current_iteration_count;
                print_total_progress();
            }
        }

        log("\n######## ALLOCATION, COPY AND FREE ONLY ONCE ########", 2);

        // Allocation, copy and free once
        for (int ids = 0; ids < DATASET_NUMBER; ++ids) {
            
            host_dataset* dataset = &hdata[ids];
            write_file << dataset->seed << "\n";

            log("------- DATASET SEED " + std::to_string(dataset->seed) + " -------\n", 2);
            sycl_allocation(sycl_q, dataset, gtimer, mode);

            // Allocation and free on device, for each iteration
            for (int rpt = 0; rpt < REPEAT_COUNT_ONLY_PARALLEL; ++rpt) {
                log("Iteration " + std::to_string(rpt+1) + " on " + std::to_string(REPEAT_COUNT_ONLY_PARALLEL), 2);
                
                sycl_compute(sycl_q, dataset, gtimer, mode);
                
                write_file << gtimer.t_parallel_for << " " 
                            << gtimer.t_read_from_device << " "
                            << "\n";

                // A new line for each repeat count :
                // t_allocation t_copy_to_device t_parallel_for t_read_from_device t_free_gpu
                print_timer_iter(gtimer);

                ++current_iteration_count;
                print_total_progress();
            }
            sycl_free(sycl_q, dataset, gtimer, mode);

            write_file << gtimer.t_allocation << " "
                        << gtimer.t_sycl_host_alloc << " " // v6
                        << gtimer.t_sycl_host_copy << " " // v6
                        << gtimer.t_copy_to_device << " "
                        << gtimer.t_sycl_host_free << " " // v6
                        << gtimer.t_free_gpu
                        << "\n";

            print_timer_alloc(gtimer);
        }

        if (MAX_SHOWN_LEVEL >= 2) {
            std::cout 
                << "t_data_ram_init            = " << gtimer.t_data_generation_and_ram_allocation << std::endl
                << "t_queue_creation           = " << gtimer.t_queue_creation << std::endl
                ;
            log("");
        }



    } catch (cl::sycl::exception const &e) {
        std::cout << "An exception has been caught while processing SyCL code.\n";
        std::terminate();
    }

    if ( ! KEEP_SAME_DATASETS ) {
        // Delete local datasets
        delete_datasets(hdata);
    }

    log("done.");
}

//int percent_div_factor = 1;

void bench_smid_modes(std::ofstream& myfile) {

    //unsigned int total_elements = 1024 * 1024 * 256; // 256 * bytes = 1 GiB.
    VECTOR_SIZE_PER_ITERATION = 128;
    PARALLEL_FOR_SIZE = total_elements / VECTOR_SIZE_PER_ITERATION; // = 131072

    int imode;
    //MEMCOPY_IS_SYCL = 1;
    //SIMD_FOR_LOOP = 0;
    //USE_NAMED_KERNEL = 0;

    log("============    - L = VECTOR_SIZE_PER_ITERATION = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
    log("============    - M = PARALLEL_FOR_SIZE = " + std::to_string(PARALLEL_FOR_SIZE));
    
    total_main_seq_runs = 2 * 3;
    
    //percent_div_factor = 2 * 3;

    for (int imcp = 0; imcp <= 1; ++imcp) {
        SIMD_FOR_LOOP = imcp;

        for (int imode = 0; imode <= 2; ++imode) {
            
            switch (imode) {
            case 0: CURRENT_MODE = sycl_mode::shared_USM; break;
            case 1: CURRENT_MODE = sycl_mode::device_USM; break;
            case 2: CURRENT_MODE = sycl_mode::host_USM; break;
            default : break;
            }
            
            log("Mode(" + mode_to_string(CURRENT_MODE) + ")  SIMD_FOR_LOOP(" + std::to_string(SIMD_FOR_LOOP) + ")");
            main_sequence(myfile, CURRENT_MODE);
            log("");
        }
    }
}

void bench_mem_alloc_modes(std::ofstream& myfile) {

    //unsigned int total_elements = 1024 * 1024 * 256; // 256 * bytes = 1 GiB.
    VECTOR_SIZE_PER_ITERATION = 128;
    PARALLEL_FOR_SIZE = total_elements / VECTOR_SIZE_PER_ITERATION; // = 131072

    // how many times main_sequence will be run
    total_main_seq_runs = 2 * 3;

    int imode;
    //MEMCOPY_IS_SYCL = 1;
    //SIMD_FOR_LOOP = 0;
    //USE_NAMED_KERNEL = 0;

    log("============    - L = VECTOR_SIZE_PER_ITERATION = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
    log("============    - M = PARALLEL_FOR_SIZE = " + std::to_string(PARALLEL_FOR_SIZE));
    
    //percent_div_factor = 2 * 2;

    for (int imcp = 0; imcp <= 1; ++imcp) {
        MEMCOPY_IS_SYCL = imcp;

        for (int imode = 0; imode <= 2; ++imode) {
            
            switch (imode) {
            case 0: CURRENT_MODE = sycl_mode::shared_USM; break;
            case 1: CURRENT_MODE = sycl_mode::device_USM; break;
            case 2: CURRENT_MODE = sycl_mode::host_USM; break;
            default : break;
            }
            log("Mode(" + mode_to_string(CURRENT_MODE) + ")  MEMCOPY_IS_SYCL(" + std::to_string(MEMCOPY_IS_SYCL) + ")");
            //log("============    - L = VECTOR_SIZE_PER_ITERATION = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
            
            main_sequence(myfile, CURRENT_MODE);
            log("");
        }
    }
}

void bench_host_copy_buffer(std::ofstream& myfile) {

    VECTOR_SIZE_PER_ITERATION = 128;
    PARALLEL_FOR_SIZE = total_elements / VECTOR_SIZE_PER_ITERATION;

    // how many times main_sequence will be run
    total_main_seq_runs = 2 * 3;

    int imode;
    //MEMCOPY_IS_SYCL = 1;
    //SIMD_FOR_LOOP = 0;
    //USE_NAMED_KERNEL = 0;

    log("============    - L = VECTOR_SIZE_PER_ITERATION = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
    log("============    - M = PARALLEL_FOR_SIZE = " + std::to_string(PARALLEL_FOR_SIZE));
    
    //percent_div_factor = 2 * 2;

    for (int imcp = 0; imcp <= 1; ++imcp) {
        USE_HOST_SYCL_BUFFER = imcp;

        for (int imode = 0; imode <= 2; ++imode) {
            
            switch (imode) {
            case 0: CURRENT_MODE = sycl_mode::shared_USM; break;
            case 1: CURRENT_MODE = sycl_mode::device_USM; break;
            case 2: CURRENT_MODE = sycl_mode::host_USM; break;
            default : break;
            }
            log("Mode(" + mode_to_string(CURRENT_MODE) + ")  USE_HOST_SYCL_BUFFER(" + std::to_string(USE_HOST_SYCL_BUFFER) + ")");
            //log("============    - L = VECTOR_SIZE_PER_ITERATION = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
            
            main_sequence(myfile, CURRENT_MODE);
            log("");
        }
    }
}



void bench_choose_L_M(std::ofstream& myfile) {

    //long long total_elements = 1024L * 1024L * 256L * 1L; // 256 * bytes = 1 GiB.

    int imode;
    //MEMCOPY_IS_SYCL = 1;
    //SIMD_FOR_LOOP = 0;
    //USE_NAMED_KERNEL = 0;

    long long start_L_size = 1;
    long long stop_M_size = 256; // inclusive
    long long stop_L_size = total_elements / stop_M_size;

    // how many times main_sequence will be run
    total_main_seq_runs = 0;
    for (VECTOR_SIZE_PER_ITERATION = start_L_size; VECTOR_SIZE_PER_ITERATION <= stop_L_size; VECTOR_SIZE_PER_ITERATION *= 2) {
        for (int imode = 1; imode <= 1; ++imode) {
            total_main_seq_runs += 1;
        }
    }
    //current_main_seq_runs = 0;

    for (VECTOR_SIZE_PER_ITERATION = start_L_size; VECTOR_SIZE_PER_ITERATION <= stop_L_size; VECTOR_SIZE_PER_ITERATION *= 2) {
        PARALLEL_FOR_SIZE = total_elements / VECTOR_SIZE_PER_ITERATION;

        for (int imode = 1; imode <= 1; ++imode) {
            
            switch (imode) {
            //case 0: CURRENT_MODE = sycl_mode::shared_USM; break;
            case 1: CURRENT_MODE = sycl_mode::device_USM; break;
            //case 2: CURRENT_MODE = sycl_mode::host_USM; break;
            default : break;
            }
            
            log("========================================= " + ver_prefix);
            log(" - MEMORY = " + mode_to_string(CURRENT_MODE));
            log(" - L = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
            log(" - M = " + std::to_string(PARALLEL_FOR_SIZE));
            main_sequence(myfile, CURRENT_MODE);
            log("");
            //++current_main_seq_runs;
        }
    }
}


int main(int argc, char *argv[])
{
    std::ofstream myfile;
    std::string wdir_tmp = std::filesystem::current_path();
    std::string wdir = wdir_tmp + "/output_bench/";
    std::string output_file_name = wdir + std::string(OUTPUT_FILE_NAME);

    myfile.open(output_file_name);
    log("");

    log("current_path     = " + wdir);
    log("output_file_name = " + output_file_name);

    if (myfile.is_open()) {
        log("OK, fichier bien ouvert.");
    } else {
        log("ERREUR : échec de l'ouverture du fichier en écriture.");
        return 10;
    }
    log("");

    myfile << DATA_VERSION << "\n";

    std::cout << "============================" << std::endl;
    std::cout << "   SYCL memory benchmark.   " << std::endl;
    std::cout << "============================" << std::endl;
    
    std::cout << OUTPUT_FILE_NAME << std::endl;
    log("");
    log("-------------- " + ver_indicator + " --------------");
    log("");
    //bench_mem_alloc_modes(myfile);
    //bench_smid_modes(myfile);
    bench_host_copy_buffer(myfile);
    //bench_choose_L_M(myfile);

    //PARALLEL_FOR_SIZE = 128;//1024;
    //VECTOR_SIZE_PER_ITERATION = 256 * 1024 * 8;

    /*
    1. Fixer L : taille du vecteur par workitem
    2. Fixer M : nombre de workitems
    3. Lancer ni itérations sur nd datasets distincts, pour comparer le temps pris entre

    Nécessaire de trouver l'endroit où les données tiennent en cache et l'endroit où elles ne tiennent plus
    
    */

    // see above, header file 
    //VECTOR_SIZE_PER_ITERATION = 8 * 1024;
    //PARALLEL_FOR_SIZE = 1024 * 32; // work items number (30M items)

    // todo : affichage graphique de L et M sur les graphiques


    // Do that for each mode

    
    /*log("=============== L = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
    log("=============== M = " + std::to_string(PARALLEL_FOR_SIZE));

    main_sequence(myfile, CURRENT_MODE);*/

    /*
    
    
    
    */

    /*

    //PARALLEL_FOR_SIZE = 0; // = M
    for (VECTOR_SIZE_PER_ITERATION = 1; VECTOR_SIZE_PER_ITERATION < 10; ++VECTOR_SIZE_PER_ITERATION) { // = L
        log("----============    - VECTOR_SIZE_PER_ITERATION = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
        // 134217728
        // 268435456
        // 8388608
        for (PARALLEL_FOR_SIZE = 128; PARALLEL_FOR_SIZE < 128 * 8; PARALLEL_FOR_SIZE += 128) {
        //for (PARALLEL_FOR_SIZE = 1; PARALLEL_FOR_SIZE < 256 * 256 * 128; PARALLEL_FOR_SIZE *= 2) {
            log("============    - PARALLEL_FOR_SIZE = " + std::to_string(PARALLEL_FOR_SIZE));
            main_sequence(myfile);
            //PARALLEL_FOR_SIZE *= 2;
            //VECTOR_SIZE_PER_ITERATION /= 2;
        }
    }*/

    myfile.close();
    log("OK, done.");

    if ( KEEP_SAME_DATASETS ) {
        // Delete local datasets
        delete_datasets(global_persistent_datasets);
        global_persistent_datasets = nullptr;
    }

    return 0;

}

/*
To run with syclcc, set those variables :
export HIPSYCL_TARGETS="cuda:sm_35" && \
export HIPSYCL_GPU_ARCH="sm_35" && \
export HIPSYCL_CUDA_PATH="/usr/local/cuda-10.1"


On Sandor :
export HIPSYCL_TARGETS="cuda:sm_75" && \
export HIPSYCL_GPU_ARCH="sm_75" && \
export HIPSYCL_CUDA_PATH="/usr/local/cuda-10.1"

ERROR :

memory_benchmark_file_output_cmp.cpp:177:73: error: Optional kernel lambda naming requires clang >= 10
    auto e = sycl_q.parallel_for(cl::sycl::range<1>(PARALLEL_FOR_SIZE), [=](cl::sycl::id<1> chunk_index) {
                                                                        ^
2 warnings and 1 error generated when compiling for sm_75.

clangs :

clang                        clang-cl-11                  clang-offload-bundler-11
clang++                      clang-cpp                    clang-offload-wrapper-11
clang++-11                   clang-cpp-11                 clang-query
clang-11                     clang-doc-11                 clang-query-11
clang-9                      clang-extdef-mapping         clang-refactor
clang-apply-replacements     clang-extdef-mapping-11      clang-refactor-11
clang-apply-replacements-11  clang-format                 clang-rename
clang-change-namespace-11    clang-import-test            clang-rename-11
clang-check                  clang-include-fixer-11       clang-reorder-fields-11
clang-check-11               clang-move-11                clang-scan-deps
clang-cl                     clang-offload-bundler        clang-scan-deps-11

*/