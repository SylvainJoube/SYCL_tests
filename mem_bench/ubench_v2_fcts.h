#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>

// file
#include <sys/stat.h>
#include <unistd.h>
#include <string>

// SyCL specific includes
#include <CL/sycl.hpp>
#include <array>
#include <sys/time.h>
#include <stdlib.h>

#include "utils.h"
#include "constants.h"

// Regroupe des fonctions & structures utiles
// Je n'ai pas changé les noms des variables, pour arriver plus vite au résultat voulu.
namespace ubench_v2 {

    #define UBENCH2_VERSION 1
    //#define UBENCH2_VERSION_STR "1"
    const std::string UBENCH2_VERSION_FILE_PREFIX = "ubench2_" + std::to_string(static_cast<int>(UBENCH2_VERSION));

    /*
    - alloc native (device, accessors, shared copy)
    - alloc sycl (tous)
    - fill
    - copy (device, shared copy)
    - kernel 1 .. 4
    - read
    - dealloc sycl
    - dealloc native
    */

    const unsigned int microseconds = 0;

    // Nombre de data_type
    const unsigned long b_INPUT_DATA_LENGTH   = 1L * 1024L * 1024L * 1024L / sizeof(data_type);
    //const unsigned long b_INPUT_DATA_LENGTH   = 6L * 1024L * 1024L * 1024L / sizeof(data_type);
    const unsigned long b_INPUT_OUTPUT_FACTOR = 128; // taille des sommes partielles
    const unsigned long b_OUTPUT_DATA_LENGTH  = b_INPUT_DATA_LENGTH / b_INPUT_OUTPUT_FACTOR;

    // Taille en octets calculées
    // unsigned long b_INPUT_DATA_SIZE;
    // unsigned long b_OUTPUT_DATA_SIZE;

    //data_type s;

    const uint TRACCC_LOG_LEVEL = 0; // Seulement afficher les infos du log 0

    //bool ignore_allocation_times;// = false;
    // bool ignore_pointer_graph_benchmark;
    // bool ignore_flatten_benchmark;

    const unsigned int in_total_size  = b_INPUT_DATA_LENGTH  * sizeof(data_type);
    const unsigned int out_total_size = b_OUTPUT_DATA_LENGTH * sizeof(data_type);

    enum mem_strategy { pointer_graph, flatten };

    std::string mem_strategy_to_str(mem_strategy m) {
        switch (m) {
            case pointer_graph : return "pointer_graph";
            case flatten : return "flatten";
            default : return "inconnu";
        }
    }

    unsigned int mem_strategy_to_int(mem_strategy m) {
        switch (m) {
            case pointer_graph : return 1;
            case flatten : return 2;
            default : return 0;
        }
    }

    struct traccc_chrono_results {
        // alloc et fill sont utiles en flatten uniquement, 
        // ça n'a pas grand sens en graphe de ponteur
        // (vu que la structure change)
        //uint t_alloc_fill, t_flatten_alloc, t_flatten_fill, t_copy_kernel, t_read, t_free_mem, t_alloc_only, t_fill_only;

        // Nouveau timer
        int t_alloc_native, t_alloc_sycl, t_fill, t_copy, t_read, t_dealloc_sycl, t_dealloc_native;
        static const uint kernel_count = 2;
        int t_kernel[kernel_count];
    };

    class bench_variables {
    public:

        // Mémoire native
        data_type * native_input  = nullptr;
        data_type * native_output = nullptr;

        // Mémoire SYCL USM
        data_type * sycl_input  = nullptr;
        data_type * sycl_output = nullptr;

        // Accessors/buffers
        cl::sycl::buffer<data_type, 1> * buffer_input  = nullptr;
        cl::sycl::buffer<data_type, 1> * buffer_output = nullptr;

        data_type expected_sum; // calculée une fois, sur CPU

        sycl_mode mode;
        bool explicit_copy = false;

        cl::sycl::queue sycl_q;
        //mem_strategy mstrat;// = flatten;

        traccc_chrono_results c;

        // -1 signifie "n'a pas de sens dans ce contexte"
        void reset_timer() {
            //traccc_chrono_results & chres = c;
            c.t_alloc_native = -1;
            c.t_alloc_sycl = -1;
            c.t_fill = -1;
            c.t_copy = -1;
            c.t_read = -1;
            c.t_dealloc_sycl = -1;
            c.t_dealloc_native = -1;
            for (uint i = 0; i < c.kernel_count; ++i) {
                c.t_kernel[i] = -1;
            }
        }

        bench_variables() {
            reset_timer();
        }
    };


    bool is_using_native_memory(bench_variables const& b) {
        if ( (b.mode == sycl_mode::shared_USM) && b.explicit_copy ) return true;
        if ( (b.mode == sycl_mode::host_USM)   && b.explicit_copy ) return true;
        if ( b.mode == sycl_mode::accessors )  return true;
        if ( b.mode == sycl_mode::glibc )      return true;
        if ( b.mode == sycl_mode::device_USM ) return true;
        return false;
    }

    bool need_explicit_copy(bench_variables const& b) {
        if ( (b.mode == sycl_mode::shared_USM) && b.explicit_copy ) return true;
        if ( (b.mode == sycl_mode::host_USM)   && b.explicit_copy ) return true;
        if (  b.mode == sycl_mode::device_USM ) return true;
        return false;
    }

    bool is_using_usm(bench_variables const& b) {
        if (b.mode == sycl_mode::host_USM)   return true;
        if (b.mode == sycl_mode::device_USM) return true;
        if (b.mode == sycl_mode::shared_USM) return true;
        return false;
    }

    // Alloc native + alloc SYCL
    void allocation(bench_variables & b) {
        log("allocation");
        stime_utils chrono;
        //shared_USM, device_USM, host_USM, accessors, glibc};

        chrono.start();

        // Alloc native si besoin
        if (is_using_native_memory(b)) {
            b.native_input  = new data_type[b_INPUT_DATA_LENGTH];
            b.native_output = new data_type[b_OUTPUT_DATA_LENGTH];
            b.c.t_alloc_native = chrono.reset();
        }

        switch(b.mode) {
        case shared_USM:
            b.sycl_input  = cl::sycl::malloc_shared<data_type>(b_INPUT_DATA_LENGTH,  b.sycl_q);
            b.sycl_output = cl::sycl::malloc_shared<data_type>(b_OUTPUT_DATA_LENGTH, b.sycl_q);
            b.sycl_q.wait_and_throw();
            b.c.t_alloc_sycl = chrono.reset();
            break;
        
        case host_USM:
            b.sycl_input  = cl::sycl::malloc_host<data_type>(b_INPUT_DATA_LENGTH,  b.sycl_q);
            b.sycl_output = cl::sycl::malloc_host<data_type>(b_OUTPUT_DATA_LENGTH, b.sycl_q);
            b.sycl_q.wait_and_throw();
            b.c.t_alloc_sycl = chrono.reset();
            break;

        case device_USM:
            // Alloc native + sycl
            b.sycl_input  = cl::sycl::malloc_device<data_type>(b_INPUT_DATA_LENGTH,  b.sycl_q);
            b.sycl_output = cl::sycl::malloc_device<data_type>(b_OUTPUT_DATA_LENGTH, b.sycl_q);
            b.sycl_q.wait_and_throw();
            b.c.t_alloc_sycl = chrono.reset();
            break;

        case accessors:
            // Alloc native + sycl
            b.buffer_input  = new cl::sycl::buffer<data_type, 1> (b.native_input,   cl::sycl::range<1>(b_INPUT_DATA_LENGTH));
            b.buffer_output = new cl::sycl::buffer<data_type, 1> (b.native_output,  cl::sycl::range<1>(b_OUTPUT_DATA_LENGTH));
            b.sycl_q.wait_and_throw();
            b.c.t_alloc_sycl = chrono.reset();
            break;

        case glibc: // alloc native déjà réalisée
            break;
        }
    }

    data_type g_expected_sum;

    // Pour la vérification des résultats
    void compute_expected_sum() {
        data_type sum = 0;
        for (size_t i = 0; i < b_INPUT_DATA_LENGTH; ++i) {
            sum += i % 20;
        }
        g_expected_sum = sum;
    }

    void fill(bench_variables & b) {
        log("fill");
        stime_utils chrono;
        chrono.start();
        if (is_using_native_memory(b)) {
            // Fill native memory
            for (size_t i = 0; i < b_INPUT_DATA_LENGTH; ++i) {
                b.native_input[i] = i % 20;
            }
        } else {
            // Fill SYCL memory
            for (size_t i = 0; i < b_INPUT_DATA_LENGTH; ++i) {
                b.sycl_input[i] = i % 20;
            }
        }
        b.c.t_fill = chrono.reset();
    }

    void copy(bench_variables & b) {
        log("copy");
        stime_utils chrono;
        chrono.start();
        if (need_explicit_copy(b)) {
            b.sycl_q.memcpy(b.sycl_input, b.native_input, b_INPUT_DATA_LENGTH * sizeof(data_type));
            b.c.t_copy = chrono.reset();
        }
    }

    void kernel_iteration(bench_variables & b, uint kernel_id) {
        stime_utils chrono;
        chrono.start();

        const size_t pfsize = b_INPUT_DATA_LENGTH / b_INPUT_OUTPUT_FACTOR;

        const uint local_b_INPUT_OUTPUT_FACTOR = b_INPUT_OUTPUT_FACTOR;

        // Mémoire USM
        if ( is_using_usm(b) ) {
            data_type * s_input = b.sycl_input;;
            data_type * s_output = b.sycl_output;
        
            auto e = b.sycl_q.parallel_for<class MyKernel_b>(cl::sycl::range<1>(pfsize), [=](cl::sycl::id<1> chunk_index) {
                int cindex = chunk_index[0];
                data_type sum = 0;

                for (int it = 0; it < local_b_INPUT_OUTPUT_FACTOR; ++it) {
                    int iindex = cindex + it * pfsize;
                    sum += s_input[iindex];
                }
                s_output[cindex] = sum;
            });
            e.wait_and_throw();
        }

        // glibc
        if ( b.mode == sycl_mode::glibc ) {
            data_type * n_input  = b.native_input;;
            data_type * n_output = b.native_output;

            // Ne somme pas dans le même ordre qu'en kernel SYCL
            unsigned long long ci = 0; // <- current_index
            for (size_t iop = 0; iop < pfsize; ++iop) {
                data_type sum = 0;
                for (size_t cindex = 0; cindex < b_INPUT_OUTPUT_FACTOR; ++cindex) {
                    sum += n_input[ci];
                    ++ci;
                }
                n_output[iop] = sum;
            }
        }

        // accessors
        if ( b.mode == sycl_mode::accessors ) {
            cl::sycl::buffer<data_type, 1> *b_input   = b.buffer_input;  // wraps b.native_input
            cl::sycl::buffer<data_type, 1> *b_output  = b.buffer_output; // wraps b.native_output

            b.sycl_q.submit([&](cl::sycl::handler &h) {
                cl::sycl::accessor a_input (*b_input,  h, cl::sycl::read_only);
                cl::sycl::accessor a_output(*b_output, h, cl::sycl::write_only, cl::sycl::no_init);

                h.parallel_for<class MyKernel_b>(cl::sycl::range<1>(pfsize), [=](cl::sycl::id<1> chunk_index) {
                    int cindex = chunk_index[0];
                    data_type sum = 0;

                    for (int it = 0; it < local_b_INPUT_OUTPUT_FACTOR; ++it) {
                        int iindex = cindex + it * pfsize;
                        sum += a_input[iindex];
                    }
                    a_output[cindex] = sum;
                });
            }).wait_and_throw();
        }
        b.c.t_kernel[kernel_id] = chrono.reset();
    }

    void kernel(bench_variables & b) {
        log("kernels");
        for (uint kernel_id = 0; kernel_id < b.c.kernel_count; ++kernel_id) {
            kernel_iteration(b, kernel_id);
        }
    }

    data_type read(bench_variables & b) {
        log("read");
        stime_utils chrono;
        chrono.start();

        if ( b.mode == sycl_mode::accessors ) {
            (*b.buffer_output).get_access<cl::sycl::access::mode::read>();
            b.sycl_q.wait_and_throw();
        }

        // Forcément mémoire USM si copie explicite
        if (need_explicit_copy(b)) {
            log("read - explicit copy...");
            b.sycl_q.memcpy(b.native_output, b.sycl_output, b_OUTPUT_DATA_LENGTH * sizeof(data_type));
            b.sycl_q.wait_and_throw();
            log("ok");
        }

        data_type sum = 0;
        if (is_using_native_memory(b)) {
            log("read - use native memory, summing...");
            for (size_t i = 0; i < b_OUTPUT_DATA_LENGTH; ++i) {
                sum += b.native_output[i];
            }
            log("ok");
        } else {
            log("read - use sycl memory, summing...");
            for (size_t i = 0; i < b_OUTPUT_DATA_LENGTH; ++i) {
                sum += b.sycl_output[i];
            }
            log("ok");
        }
        
        b.c.t_read = chrono.reset();
        return sum;
    }

    void dealloc(bench_variables & b) {
        log("dealloc");
        stime_utils chrono;
        chrono.start();

        if (is_using_native_memory(b)) {
            delete[] b.native_input;
            delete[] b.native_output;
            b.c.t_dealloc_native = chrono.reset();
        }

        if (b.mode == sycl_mode::accessors) {
            delete b.buffer_input;
            delete b.buffer_output;
            b.buffer_input  = nullptr;
            b.buffer_output = nullptr;
            b.c.t_dealloc_sycl = chrono.reset();
        }

        if (is_using_usm(b)) {
            cl::sycl::free(b.sycl_input,  b.sycl_q);
            cl::sycl::free(b.sycl_output, b.sycl_q);
            b.sycl_input  = nullptr;
            b.sycl_output = nullptr;
            b.c.t_dealloc_sycl = chrono.reset();
        }
        log("Iteration OK.");
    }


    traccc_chrono_results traccc_bench(sycl_mode mode, bool explicit_copy) {

        custom_device_selector d_selector;
        try {
            //chrono.reset(); //t_start = get_ms();
            cl::sycl::queue sycl_q(d_selector, exception_handler);
            sycl_q.wait_and_throw();

            bench_variables bench;
            bench.reset_timer();
            bench.mode = mode;
            bench.explicit_copy = explicit_copy;
            bench.sycl_q = sycl_q;

            allocation(bench);
            fill(bench);
            copy(bench);
            kernel(bench);
            data_type sum = read(bench);
            dealloc(bench);

            if (sum != g_expected_sum) {
                log("ERROR ERROR ERROR : sum(" + std::to_string(sum) + ") != expected_sum(" + std::to_string(g_expected_sum));
                log("   ----> for " + mode_to_string(mode) + (explicit_copy ? " explicit_copy" : " auto_copy"));
                std::terminate();
            }

            return bench.c; // résultats chronométrés            
            
        } catch (cl::sycl::exception const &e) {
            std::cout << "An exception has been caught while processing SyCL code.\n";
            std::terminate();
        }
    }

    void traccc_main_sequence(std::ofstream& write_file, sycl_mode mode, bool explicit_copy) {
        log("\n\n==== Mode(" + mode_to_string(mode) + ")  " + (explicit_copy ? "explicit_copy" : "auto_copy") + " ====");

        write_file 
        << in_total_size << " " // INPUT_DATA_SIZE
        << out_total_size << " " // OUTPUT_DATA_SIZE
        << b_INPUT_OUTPUT_FACTOR << " "
        << REPEAT_COUNT_REALLOC << " " // ------ utile, nombre de fois que le test doit être lancé (défini dans le main)
        << mode_to_int(mode) << " " // ------ utile
        << (explicit_copy ? "1" : "0") << " " // 1 copie explicite ; 0 copie automatique
        << "\n";

        // Allocation and free on device, for each iteration
        for (int rpt = 0; rpt < REPEAT_COUNT_REALLOC; ++rpt) {
            log("Iteration " + std::to_string(rpt+1) + " on " + std::to_string(REPEAT_COUNT_REALLOC), 2);

            traccc_chrono_results cres;

            cres = traccc_bench(mode, explicit_copy);

            write_file
            << cres.t_alloc_native << " "
            << cres.t_alloc_sycl << " "
            << cres.t_fill << " "
            << cres.t_copy << " "
            << cres.t_read << " "
            << cres.t_dealloc_sycl << " "
            << cres.t_dealloc_native << " "
            << cres.kernel_count << " ";
            for (uint ik = 0; ik < cres.kernel_count; ++ik) {
                write_file << cres.t_kernel[ik] << " ";
            }
            write_file << "\n";

            ++current_iteration_count;
            print_total_progress();

            int fdiv = 1000; // ms
            logs(
                "\n       t_alloc_native(" + std::to_string(cres.t_alloc_native / fdiv) + ") "
                + "t_alloc_sycl(" + std::to_string(cres.t_alloc_sycl / fdiv) + ") "
                + "t_fill(" + std::to_string(cres.t_fill / fdiv) + ") "
                + "t_copy(" + std::to_string(cres.t_copy / fdiv) + ") "
                + "t_read(" + std::to_string(cres.t_read / fdiv) + ") "
                + "t_dealloc_sycl(" + std::to_string(cres.t_dealloc_sycl / fdiv) + ") "
                + "t_dealloc_native(" + std::to_string(cres.t_dealloc_native / fdiv) + ") ");
            
            for (uint ik = 0; ik < cres.kernel_count; ++ik) {
                logs("ker" + std::to_string(ik) + "(" + std::to_string(cres.t_kernel[ik] / fdiv) + ") ");
            }
            log("");
        }
        log("\n");
    }


    // void bench_mem_location_and_strategy(std::ofstream& myfile) {

    //     //log("============    - L = VECTOR_SIZE_PER_ITERATION = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
    //     //log("============    - M = PARALLEL_FOR_SIZE = " + std::to_string(PARALLEL_FOR_SIZE));

    //     // Implicit copy
    //     traccc_main_sequence(myfile, sycl_mode::shared_USM, false);
    //     traccc_main_sequence(myfile, sycl_mode::host_USM,   false);
    //     traccc_main_sequence(myfile, sycl_mode::accessors,  false);
    //     traccc_main_sequence(myfile, sycl_mode::glibc,      false);

    //     // (USM) explicit copy
    //     traccc_main_sequence(myfile, sycl_mode::device_USM, true);
    //     traccc_main_sequence(myfile, sycl_mode::shared_USM, true);
    //     traccc_main_sequence(myfile, sycl_mode::host_USM,   true);

    // }

    int main_of_bench_v2(std::string fname) { //std::function<void(std::ofstream &)> bench_function) {
        std::ofstream myfile;
        std::string wdir_tmp = std::filesystem::current_path();
        std::string wdir = wdir_tmp + "/output_bench/";
        std::string output_file_path = wdir + std::string(fname);

        if ( file_exists_test0(output_file_path) ) {
            log("\n\n\n\n\nFILE ALREADY EXISTS, SKIPPING TEST");
            log("NAME = " + fname + "\n");
            log("FULL PATH = " + output_file_path + "\n\n\n\n\n");
            return 4;
        }

        myfile.open(output_file_path);
        log("");

        log("output_directory  = " + wdir);
        log("output_file_path  = " + output_file_path);

        if (myfile.is_open()) {
            log("OK, fichier bien ouvert.");
        } else {
            log("ERREUR : échec de l'ouverture du fichier en écriture.");
            std::terminate();
            return 10;
        }
        log("\n");
        log("Version du fichier : " + std::to_string(UBENCH2_VERSION));
        log("\n");

        myfile << UBENCH2_VERSION << "\n";

        log("============================");
        log("  SYCL BENCH V2 benchmark.  ");
        log("============================");
        
        std::cout << OUTPUT_FILE_NAME << std::endl;
        log("");
        log("-------------- " + ver_indicator + " --------------");
        log("");

        //log("-----> traccc_repeat_load_count(" + std::to_string(traccc_repeat_load_count) + ")");
        //if ( ignore_allocation_times ) log("-----> Ignore allocation times.");
        //else                           log("-----> Count allocation times.");

        list_devices(exception_handler);

        //REPEAT_COUNT_ONLY_PARALLEL_WARMUP_COUNT = 0;


        init_progress();

        compute_expected_sum();

        total_main_seq_runs = 7;

        // (USM) explicit copy
        traccc_main_sequence(myfile, sycl_mode::device_USM, true);
        traccc_main_sequence(myfile, sycl_mode::shared_USM, true);
        traccc_main_sequence(myfile, sycl_mode::host_USM,   true);
        
        // Implicit copy
        traccc_main_sequence(myfile, sycl_mode::shared_USM, false);
        traccc_main_sequence(myfile, sycl_mode::host_USM,   false);
        traccc_main_sequence(myfile, sycl_mode::accessors,  false);
        traccc_main_sequence(myfile, sycl_mode::glibc,      false);


        //bench_function(myfile);
        
        myfile.close();
        log("OK, done.");

        return 0;
    }

    std::string input_size_to_str() {
        ulong sz = in_total_size;
        if (sz >= (1024L * 1024L * 1024L)) {
            return std::to_string(sz / (1024L * 1024L * 1024L)) + "GiB";
        } else {
            return std::to_string(sz / (1024L * 1024L)) + "MiB";
        }
    }

    void run_ubench2_single_test(std::string const computer_name, uint run_id) {
        OUTPUT_FILE_NAME = UBENCH2_VERSION_FILE_PREFIX + "_" + input_size_to_str() + "_RUN" + std::to_string(run_id) + ".t";
        log("OUTPUT_FILE_NAME = " + OUTPUT_FILE_NAME);
        main_of_bench_v2(OUTPUT_FILE_NAME);
    }

    void run_ubench2_tests(std::string const computer_name, uint run_number) {
        log("run_ubench2_tests RUN");
        log("run_ubench2_tests RUN");
        log("run_ubench2_tests RUN");
        for (uint i = 1; i <= run_number; ++i) {
            run_ubench2_single_test(computer_name, i);
        }
    }

}
