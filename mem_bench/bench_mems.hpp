
#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <random>

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
#include "traccc_fcts.h"
#include "sycl_helloworld.h"
#include "bench_mems.hpp"

// 890

class bench_sycl_glibc_mem_speed_run {
public :
    uint64_t min_time, max_time;
    uint64_t count; // number of runs
    uint64_t total_time;
    void init() {
        min_time = 0;
        max_time = 0;
        count = 0;
        total_time = 0;
    }
    void add(uint64_t time) {
        if (count == 0) { // init
            min_time = time;
            max_time = time;
        } else {
            if (min_time > time) min_time = time;
            if (max_time < time) max_time = time;
        }
        ++count;
        total_time += time;
    }
    void print(std::string name) {
        uint64_t moy = total_time / count;
        uint64_t div = 1000;
        uint64_t tolerate_fact = 140;
        if (min_time * tolerate_fact / 100 >= max_time) {
            log("   " + name + "   " + std::to_string(moy / div));
        } else {
            log("   " + name + "   " + std::to_string(min_time / div)
            + " -> " + std::to_string(max_time / div)
            + " : " + std::to_string(moy / div));
        }
    }
};

class bench_sycl_glibc_mem_speed_main {

private :
    const uint64_t ecount = 1024L * 1024L * 128L * 4L; // 128 MiO * 4 = 512 MiO
    const std::string size_str = "512MiB";
    uint run_count = 3;

    const uint64_t size = ecount * sizeof(DATA_TYPE); // 128 milions elements * 4 bytes => 512 MiB
    custom_device_selector d_selector;
    uint mem_type_src, mem_type_dest;
    bench_sycl_glibc_mem_speed_run t_alloc_src, t_alloc_dest, t_fill, t_copy, t_free_src, t_free_dest;


public :
    void init() {
        

    }

    uint sum(DATA_TYPE* mem) {
        DATA_TYPE s = 0;
        for (uint i = 0; i < ecount; ++i) {
            s += mem[i];
        }
        return s;
    }

    DATA_TYPE* alloc(uint mem_type, cl::sycl::queue sycl_q) {
        switch (mem_type) {
        case 1: return new DATA_TYPE[ecount]; // glibc
        case 2: return static_cast<data_type *> (cl::sycl::malloc_host(INPUT_DATA_SIZE, sycl_q)); // sycl
        default : return nullptr;
        }
    }

    void fill(DATA_TYPE* mem) {
        for (uint i = 0; i < ecount; ++i) {
            mem[i] = i;
        }
    }

    void freemem(uint mem_type, DATA_TYPE* mem, cl::sycl::queue sycl_q) {
        switch (mem_type) {
        case 1: delete[] mem; break; // glibc
        case 2: cl::sycl::free(mem, sycl_q); sycl_q.wait_and_throw(); break; // sycl
        default : break;
        }
    }

    void init_timers() {
        t_alloc_src.init();
        t_fill.init();
        t_alloc_dest.init();
        t_copy.init();
        t_free_src.init();
        t_free_dest.init();
    }

    void run(uint mem_type_src, uint mem_type_dest, uint cpy_type, cl::sycl::queue sycl_q) {

        /*log("run src" + std::to_string(mem_type_src) + " - dest" + std::to_string(mem_type_src)
            + " - ecount" + std::to_string(ecount)
            + " - size" + std::to_string(size)
            );*/

        stime_utils chrono;
        chrono.reset(); //t_start = get_ms();

        DATA_TYPE* mem_src = alloc(mem_type_src, sycl_q);
        t_alloc_src.add(chrono.reset());

        fill(mem_src);
        t_fill.add(chrono.reset());

        DATA_TYPE* mem_dest = alloc(mem_type_dest, sycl_q);
        t_alloc_dest.add(chrono.reset());

        // On a vu que temps de copie SYCL = temps de copie glibc pour host
        
        if (cpy_type == 1) memcpy(mem_dest, mem_src, size);
        if (cpy_type == 2) sycl_q.memcpy(mem_dest, mem_src, size).wait_and_throw();
        t_copy.add(chrono.reset());

        // Si sur l'host :
        logs("    s" + std::to_string(sum(mem_dest)));

        // Si sur le device :
        // faire la somme, en faisant des grosses sommes partielles

        freemem(mem_type_src, mem_src, sycl_q);
        t_free_src.add(chrono.reset());

        freemem(mem_type_dest, mem_dest, sycl_q);
        t_free_dest.add(chrono.reset());


    }

    void multiple_runs(uint mem_type_src, uint mem_type_dest, uint cpy_type, cl::sycl::queue sycl_q) {

        init_timers();

        for (uint ir = 0; ir < run_count; ++ir) {
                run(1, 1, 1, sycl_q);
            }
            // print result timer(min, max, moy)
            t_alloc_src .print(" alloc src");
            t_fill      .print("      fill");
            t_alloc_dest.print("alloc dest");
            t_copy      .print("      copy");
            t_free_src  .print("  free src");
            t_free_dest .print(" free dest");
            log("\n");
    }

    void main() {
        // 1) Allocation de la mémoire (src) sycl / glibc
        // 2) Remplissage de la mémoire avec des nombres aléatoires (ou toujours le même nombre)
        // 3) Allocation de la mémoire (dest) sycl/glibc
        // 4) Copie de src vers dest
        // 5) Libération de la mémoire src
        // 6) Libération de la mémoire dest

        
        try {
            
            cl::sycl::queue sycl_q(d_selector, exception_handler);
            sycl_q.wait_and_throw();

            logs("glibc -> glibc (copie glibc)");
            multiple_runs(1, 1, 1, sycl_q);

            logs("sycl -> sycl : (copie glibc)");
            multiple_runs(2, 2, 1, sycl_q);

            logs("sycl -> sycl : (copie sycl)");
            multiple_runs(2, 2, 2, sycl_q);

            logs("glibc -> sycl : (copie glibc)");
            multiple_runs(1, 2, 1, sycl_q);

            logs("glibc -> sycl : (copie sycl)");
            multiple_runs(1, 2, 2, sycl_q);

            logs("sycl -> glibc : (copie glibc)");
            multiple_runs(2, 1, 1, sycl_q);

            logs("sycl -> glibc : (copie sycl)");
            multiple_runs(2, 1, 2, sycl_q);

        } catch (cl::sycl::exception const &e) {
            std::cout << "An exception has been caught while processing SyCL code.\n";
            std::terminate();
        }
    }

};

// SyCL asynchronous exception handler
// Create an exception handler for asynchronous SYCL exceptions
static auto r_exception_handler = [](cl::sycl::exception_list e_list) {
    for (std::exception_ptr const &e : e_list) {
        try {
            std::rethrow_exception(e);
        } catch (std::exception const &e) {
            std::cout << "Failure" << std::endl;
            std::terminate();
        }
    }
};

class bench_mem_alloc_free {
    // Bench mem USM shared, device, host and libstd memory.
    // Steps to track :
    // 1) allocation time (input buffer 100 times larger that the output buffer)
    // 2) (explicit) copy time from host mem
    // 3) kernel time / partial sum on GPU or CPU
    // 4) (explicit) copy time to host mem
    // 5) free memory

    // Keeping track of every run since the very first one.

    // A brand new code to avoid legacy bugs, if any, to make sure the
    // previous results were correct.+

    // Objectif : déterminer les temps caractéristiques en fonction du type de mémoire utilisée
    // i.e. refaire rapidemnet et simplement ce qui a déjà été fait avant,
    // pour m'assurer qu'il n'y ait pas de bugs chelous que je n'ai pas vus
    // et qui fausseraient les résultats.
    // Juste sortir les valeurs numériques, pour avoir un ordre de grandeur
    // et valider (ou pas) mes résultats.

    // Etape 0 :
    // - allocation d'un buffer host stdlib
    // - remplissage du buffer
    // - allocation du buffer de sortie.

    // Etape 1 : allocation de la mémoire SYCL / stdlib
    // mémoires à tester SYCL host/shared/device
    // et aussi CPU-only pour comparer (buffer stdlib host).

    // Etape 2 : copie (explicite) de la mémoire host vers la mémoire de l'étape 1.

    // Etape 3 : sommes partielles device / CPU

    // Etape 4 : copie (explicite) vers la mémoire stdlib host

    // Etape 5 : libération de la mémoire de l'étape 1

    // Etape 6 : libération des ressources du programme (dont étape 0)

    const size_t INPUT_INT_COUNT = 1024L * 1024L * 512L; // 4 MiB * 512 = 2 GiB
    const size_t INPUT_OUTPUT_FACTOR = 1024L;
    const size_t OUTPUT_INT_COUNT = INPUT_INT_COUNT / INPUT_OUTPUT_FACTOR; // 2 MiB donc pas mal de kernels tout de même

    enum mem_type {STDL, SYCL_HOST, SYCL_SHARED, SYCL_DEVICE};

    using data_type = unsigned int;

    data_type* HOST_INPUT;
    data_type* HOST_OUTPUT;

    data_type* COMPUTE_INPUT;
    data_type* COMPUTE_OUTPUT;

    data_type expected_sum;

    mem_type MEM_TYPE;


    



    // Etape 0 :
    // - allocation d'un buffer host stdlib
    // - remplissage du buffer
    // - allocation du buffer de sortie.
    void step0() {
        HOST_INPUT = new data_type[INPUT_INT_COUNT];
        HOST_OUTPUT = new data_type[OUTPUT_INT_COUNT];
        srand( (unsigned int) 42 );

        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist6(1,10); // distribution in range [1, 6]

        for (size_t i = 0; i < INPUT_INT_COUNT; ++i) {
            HOST_INPUT[i] = dist6(rng);
        }
        for (size_t i = 0; i < OUTPUT_INT_COUNT; ++i) {
            HOST_OUTPUT[i] = 0; // just in case
        }
        expected_sum = 0;
        for (size_t i = 0; i < INPUT_INT_COUNT; ++i) {
            expected_sum += HOST_INPUT[i];
        }
    }

    // Etape 1 : allocation de la mémoire SYCL / stdlib
    // mémoires à tester SYCL host/shared/device
    // et aussi CPU-only pour comparer (buffer stdlib host).
    void step1(cl::sycl::queue& sycl_q) {
        switch (MEM_TYPE) {
            case STDL:
                COMPUTE_INPUT  = new data_type[INPUT_INT_COUNT];
                COMPUTE_OUTPUT = new data_type[OUTPUT_INT_COUNT];
                break;

            case SYCL_HOST:
                COMPUTE_INPUT  = cl::sycl::malloc_host<data_type>(INPUT_INT_COUNT, sycl_q);
                COMPUTE_OUTPUT = cl::sycl::malloc_host<data_type>(OUTPUT_INT_COUNT, sycl_q);
                break;

            case SYCL_SHARED:
                COMPUTE_INPUT  = cl::sycl::malloc_shared<data_type>(INPUT_INT_COUNT, sycl_q);
                COMPUTE_OUTPUT = cl::sycl::malloc_shared<data_type>(OUTPUT_INT_COUNT, sycl_q);
                break;

            case SYCL_DEVICE:
                COMPUTE_INPUT  = cl::sycl::malloc_device<data_type>(INPUT_INT_COUNT, sycl_q);
                COMPUTE_OUTPUT = cl::sycl::malloc_device<data_type>(OUTPUT_INT_COUNT, sycl_q);
                break;
            // pas de default, tous les cas doivent être pris en compte ici.
        }
    }

    // Etape 2 : copie (explicite) de la mémoire host vers la mémoire de l'étape 1.
    void step2(cl::sycl::queue& sycl_q) {
        if ( (MEM_TYPE == SYCL_HOST) || (MEM_TYPE == SYCL_DEVICE) || (MEM_TYPE == SYCL_SHARED) ) {
            sycl_q.memcpy(COMPUTE_INPUT, HOST_INPUT, INPUT_INT_COUNT * sizeof(data_type)).wait();
        }
        if ( MEM_TYPE == STDL ) {
            memcpy(COMPUTE_INPUT, HOST_INPUT, INPUT_INT_COUNT * sizeof(data_type));
        }
    }

    // Etape 3 : sommes partielles device / CPU
    void step3(cl::sycl::queue& sycl_q) {
        if ( (MEM_TYPE == SYCL_HOST) || (MEM_TYPE == SYCL_DEVICE) || (MEM_TYPE == SYCL_SHARED) ) {
            
            sycl_q.parallel_for<class some_kernel>(cl::sycl::range<1>(OUTPUT_INT_COUNT), [=](cl::sycl::id<1> chunk_index) {
                auto cindex = chunk_index.get(0);
                data_type partial_sum = 0;

                // Chaque kernel doit faire la somme de INPUT_OUTPUT_FACTOR éléments
                // Les éléments sont distants de OUTPUT_INT_COUNT indexes
                // pour l'exécution en lockstep des threads sur GPU.
                for (size_t it = 0; it < INPUT_OUTPUT_FACTOR; ++it) {
                    size_t ind = cindex + it * OUTPUT_INT_COUNT;
                    partial_sum += COMPUTE_INPUT[ind];
                }

                COMPUTE_OUTPUT[cindex] = partial_sum;
            }).wait();
        }
        if ( MEM_TYPE == STDL ) {
            data_type sum = 0;
            // Pour chaque case du vecteur de sortie
            for (size_t i = 0; i < OUTPUT_INT_COUNT; ++i) {
                size_t cindex = i * INPUT_OUTPUT_FACTOR;
                data_type partial_sum = 0;
                // Somme de INPUT_OUTPUT_FACTOR éléments à la suite les uns des autres
                for (size_t ii = 0; ii < INPUT_OUTPUT_FACTOR; ++ii) {
                    partial_sum += COMPUTE_INPUT[cindex + ii];
                }
                COMPUTE_OUTPUT[i] = partial_sum;
            }
        }
    }

    // Etape 4 : copie (explicite) vers la mémoire stdlib host
    void step4(cl::sycl::queue& sycl_q) {
        if ( (MEM_TYPE == SYCL_HOST) || (MEM_TYPE == SYCL_DEVICE) || (MEM_TYPE == SYCL_SHARED) ) {
            sycl_q.memcpy(HOST_OUTPUT, COMPUTE_OUTPUT, INPUT_INT_COUNT * sizeof(data_type)).wait();
        }
        if ( MEM_TYPE == STDL ) {
            memcpy(HOST_OUTPUT, COMPUTE_OUTPUT, INPUT_INT_COUNT * sizeof(data_type));
        }
    }



    // Etape 5 : libération de la mémoire de l'étape 1
    void step5(cl::sycl::queue& sycl_q) {
        if ( (MEM_TYPE == SYCL_HOST) || (MEM_TYPE == SYCL_DEVICE) || (MEM_TYPE == SYCL_SHARED) ) {
            cl::sycl::free(COMPUTE_INPUT,  sycl_q);
            cl::sycl::free(COMPUTE_OUTPUT, sycl_q);
            sycl_q.wait_and_throw();
        }
        if ( MEM_TYPE == STDL ) {
            delete[] COMPUTE_INPUT;
            delete[] COMPUTE_OUTPUT;
        }
    }



    void step6() {
        delete[] HOST_INPUT;
        delete[] HOST_OUTPUT;
        
    }

    void main_sequence() {
        step0();

        try {
            // The default device selector will select the most performant device.
            //cl::sycl::default_selector d_selector;
            cl::sycl::default_selector d_selector;
            cl::sycl::queue sycl_q(d_selector, exception_handler);
            sycl_q.wait_and_throw();

            MEM_TYPE = mem_type::STDL;
            step1(sycl_q);



        } catch (cl::sycl::exception const &e) {
            std::cout << "SYCL HELLOWORLD ERROR : An exception has been caught while processing SyCL code.\n";
        }

        step6();
    }


};