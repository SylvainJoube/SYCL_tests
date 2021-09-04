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
namespace traccc {

    //const unsigned int microseconds = 4000 * 1000;
    const unsigned int microseconds = 0;

    const uint TRACCC_LOG_LEVEL = 0; // Seulement afficher les infos du log 0

    //bool ignore_allocation_times;// = false;
    bool ignore_pointer_graph_benchmark;
    bool ignore_flatten_benchmark;

    // True pour utiliser la structure implicit_module
    // False pour utiliser les structures implicit_input_module et implicit_output_module.
    // (et les cellules qui vont avec)
    bool implicit_use_unique_module;


    using tdtype = unsigned int;
    /*
    Toutes les données stockées sont de type tdtype.
    Pour simplifier les traîtements.
    (nb modules, nb cellules, positions)
    */

    // A simple cell (input)
    struct input_cell {
        unsigned int channel0 = 0;
        unsigned int channel1 = 0;
        //float activation = 0.;
        //float time = 0.;
        // label
    };
    // A simple output cell (contains a label)
    struct output_cell {
        unsigned int label = 0;
    };


    // TODO : un seul tableau en implicite avec les cellules en entrée et en sortie
    // Voir si ça foire pas la cohérence du bordel, mais comme on est en full implicite
    // autant vrailent jouer le jeu du graphe de pointeurs.


    // Test de la lecture + écriture dans la même structure
    struct implicit_cell {
        unsigned int channel0 = 0;
        unsigned int channel1 = 0;
        unsigned int label = 0;
        //float activation = 0.;
        //float time = 0.;
        // label
    };
    struct implicit_module {
        unsigned int cell_count;
        unsigned int cluster_count;
        implicit_cell* cells;
    };


    // Contains all the cells for the module (input)
    struct implicit_input_module {
        unsigned int cell_count;
        input_cell* cells;
    };
    // Contains all the modules (input)
    /* inutile, j'utilise un tableau simple
    struct implicit_input_module_container {
        unsigned int module_count;
        implicit_input_module* modules;
    };*/

    // A simple output module (contains all the output cells)
    struct implicit_output_module {
        //unsigned int cell_count;
        unsigned int cluster_count;
        output_cell* cells;
    };
    // Module container for output data
    // inutile, j'utilise un tableau simple
    /*struct implicit_output_module_container {
        unsigned int module_count;
        implicit_output_module* modules;
    };*/

    // Shared and host will use the output_[...].
    // Device ne peut pas utiliser de graphe de pointeur.
    // Donc il faut tout aplatir.

    struct flat_input_module {
        unsigned int cell_count; // nombre de cellules du module
        unsigned int cell_start_index; // start index dans le grand tableau des cellules
    };
    struct flat_output_module {
        // unsigned int cell_count; connu
        unsigned int cluster_count;
    };

    // Tout aplati
    struct flat_input_data {
        input_cell* cells;
        flat_input_module* modules;
        // En device, il y a les tableaux malloc_host et une allocation explicite device
        input_cell* cells_device;
        flat_input_module* modules_device;
    };

    struct flat_output_data {
        output_cell* cells;
        flat_output_module* modules;
        // Device uniquement :
        output_cell* cells_device;
        flat_output_module* modules_device;
    };


    /// Implemementation of SparseCCL, following [DOI: 10.1109/DASIP48288.2019.9049184]
    ///
    /// Requires cells to be sorted in column major.

    /// Find root of the tree for entry @param e
    ///
    /// @param L an equivalance table
    ///
    /// @return the root of @param e 
    unsigned int find_root(const unsigned int* L, unsigned int e) {
        unsigned int r = e;
        while (L[r] != r) {
            r = L[r];
        }
        return r;
    } 

    unsigned int find_root(const output_cell * L, unsigned int e) {
        unsigned int r = e;
        while (L[r].label != r) {
            r = L[r].label;
        }
        return r;
    }

    /// Create a union of two entries @param e1 and @param e2
    ///
    /// @param L an equivalance table
    ///
    /// @return the rleast common ancestor of the entries 
    unsigned int make_union(unsigned int* L, unsigned int e1, unsigned int e2) {
        int e;
        if (e1 < e2){
            e = e1;
            L[e2] = e;
        } else {
            e = e2;
            L[e1] = e;
        }
        return e;
    }

    /*unsigned int make_union(output_cell * L, unsigned int e1, unsigned int e2){
        int e;
        if (e1 < e2){
            e = e1;
            L[e2].label = e;
        } else {
            e = e2;
            L[e1].label = e;
        }
        return e;
    }

    unsigned int make_union(implicit_cell * L, unsigned int e1, unsigned int e2){
        int e;
        if (e1 < e2){
            e = e1;
            L[e2].label = e;
        } else {
            e = e2;
            L[e1].label = e;
        }
        return e;
    }*/

    /// Helper method to find adjacent cells
    ///
    /// @param a the first cell
    /// @param b the second cell
    ///
    /// @return boolan to indicate 8-cell connectivity
    bool is_adjacent(input_cell a, input_cell b) {
        return (a.channel0 - b.channel0)*(a.channel0 - b.channel0) <= 1
            and (a.channel1 - b.channel1)*(a.channel1 - b.channel1) <= 1;
    }

    bool is_adjacent(implicit_cell a, implicit_cell b) {
        return (a.channel0 - b.channel0)*(a.channel0 - b.channel0) <= 1
            and (a.channel1 - b.channel1)*(a.channel1 - b.channel1) <= 1;
    }

    /// Helper method to find define distance,
    /// does not need abs, as channels are sorted in
    /// column major
    ///
    /// @param a the first cell
    /// @param b the second cell
    ///
    /// @return boolan to indicate !8-cell connectivity
    bool is_far_enough(input_cell a, input_cell b){
        return (a.channel1 - b.channel1) > 1;
    }

    bool is_far_enough(implicit_cell a, implicit_cell b){
        return (a.channel1 - b.channel1) > 1;
    }



    unsigned int total_module_count;
    unsigned int total_cell_count;
    unsigned int total_int_written;
    bool data_already_loaded_from_disk = false;

    unsigned int in_total_size;
    unsigned int out_total_size;

    // traccc_repeat_load_count dans constants.h

    // Valeurs que doivent avoir cluster_count et label_count
    // lorsque tout est exécuté (i.e. prendre en compte toutes les sparcités)
    unsigned int expected_cluster_count = 380554;
    unsigned int expected_label_sum = 23681637;

    unsigned int* all_data; // allocated with read_cells_lite
    unsigned int i_all_data;

    unsigned int read_source() {
        return all_data[i_all_data++];
    }

    void read_cells_lite(std::string fpath) {

        i_all_data = 0;

        if (data_already_loaded_from_disk) return;

        if (TRACCC_LOG_LEVEL >= 0) log("Read from " + fpath + "...");
        
        total_module_count = 0;
        total_cell_count = 0;
        total_int_written = 0;

        data_already_loaded_from_disk = true;

        //traccc::host_cell_container cells_per_event;

        long fsize = GetFileSize(fpath);

        //log("read_cells 0");
        std::ifstream rf(fpath, std::ios::out | std::ios::binary);
        
        if(!rf) {
            return;
        }

        rf.read((char *)(&total_module_count), sizeof(unsigned int));
        rf.read((char *)(&total_cell_count), sizeof(unsigned int));
        rf.read((char *)(&total_int_written), sizeof(unsigned int));

        log("total_module_count = " + std::to_string(total_module_count));
        log("total_cell_count = " + std::to_string(total_cell_count));
        log("total_int_written = " + std::to_string(total_int_written));

        unsigned int nb_ints_chk = (fsize / sizeof(unsigned int)) - 3;

        if (nb_ints_chk != total_int_written) {
            log("ERROR ?   nb_ints_chk(" +std::to_string(nb_ints_chk)
            + ") != total_int_written(" + std::to_string(total_int_written) + ")");

        }

        // fdata = flat data
        unsigned int* read_data = new unsigned int[total_int_written];

        // read the whole remaining file at once
        rf.read((char *)(read_data), total_int_written * sizeof(unsigned int));

        //log("read_cells closing...");
        rf.close();

        all_data = nullptr;

        //log("read_cells closed !");
        if(!rf.good()) {
            delete[] read_data;
            in_total_size = 0;
            out_total_size = 0;
            return;
            //return nullptr;
        }

        log("alloc traccc_repeat_load_count = " + std::to_string(traccc_repeat_load_count));
        all_data = new unsigned int[total_int_written * traccc_repeat_load_count];

        
        for (uint ir = 0; ir < traccc_repeat_load_count; ++ir) {
            log("copy ir = " + std::to_string(ir) + "...");
            memcpy(&all_data[ir * total_int_written], read_data, total_int_written * sizeof(unsigned int));
        }

        log("copy ok");

        total_module_count = total_module_count * traccc_repeat_load_count;
        total_cell_count = total_cell_count * traccc_repeat_load_count;

        expected_cluster_count = expected_cluster_count * traccc_repeat_load_count;
        expected_label_sum = expected_label_sum * traccc_repeat_load_count;

        in_total_size = total_module_count * sizeof(implicit_input_module) + total_cell_count * sizeof(input_cell);
        out_total_size = total_module_count * sizeof(implicit_output_module) + total_cell_count * sizeof(output_cell);

        log("IN SIZE  = " + std::to_string(in_total_size / 1024) + " KiB");
        log("OUT SIZE = " + std::to_string(out_total_size / 1024) + " KiB");

        return;
    }


    // A partir de ce tableau :
    // - host/shared : remplir le tableau alloué via SYCL avec ces données
    //   avec les vraies structures en mode graphe de pointeur (comme décrit ci-dessous)
    // - device : 

    
    // 1)   Allocation mémoire sycl (mem_fill si shared host et mem_dev si device)
    // Si host ou shared, plusieurs allocations (graphe de pointeurs)

    // 1.5) Pour device : allocation mémoire host (mem_fill)
    // 2)   Remplissage mémoire SYCL (depuis données brutes)
    // 2.5) Pour device : copie explicite vers la mémoire device
    // 3)   Parallel_for, exécution du kernel
    // 4.0) Pour device : copie explicite vers la mémoire host
    // 4)   Lecture des données de sortie (somme des labels des cases pour faire une lecture)
    //      et somme du nombre de clusters

    // Idées pour le débug :
    // print les items et voir s'ils sont dans le même ordre ?
    // i.e. vérifier que les cellules sont bien dans le même ordre,
    // pour chaque module (comme les cellule doivent être classées en
    // "column major, je crois")

    // A passer en paramètre à chaque fonction
    /*class bench_variables {
    public:
        // Tableau des modules en implicite
        
        cl::sycl::queue sycl_q;
    };*/

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
        uint t_alloc_fill, t_flatten_alloc, t_flatten_fill, t_copy_kernel, t_read, t_free_mem;
    };

    class bench_variables {
    public:
        implicit_input_module*  implicit_modules_in;
        implicit_output_module* implicit_modules_out;
        implicit_module* implicit_modules;

        sycl_mode mode;
        cl::sycl::queue sycl_q;
        //mem_strategy mstrat = pointer_graph;
        mem_strategy mstrat;// = flatten;

        

        flat_input_data  flat_input;
        flat_output_data flat_output;

        traccc_chrono_results chres;
        //uint t_alloc_fill, t_copy_kernel, t_read, t_free_mem;

    };


    void alloc_and_fill(bench_variables & b) {
        if (TRACCC_LOG_LEVEL >= 2) log("Alloc & fill...");
        stime_utils chrono, chrono_flatten;

        
        //sycl_mode mode = bench.mode;
        //traccc::implicit_input_module  * implicit_modules_in  = bench.implicit_modules_in;
        //traccc::implicit_output_module * implicit_modules_out = bench.implicit_modules_out;

        // malloc + fill
        // (copie) + parallel_for
        // (copie) + lecture

        // Décomposer le malloc + fill du device en malloc_host initial + fill
        // parce que le malloc_host initial n'est fait qu'une seule fois et est voué à resservir.

        chrono.reset();

        if (b.mstrat == pointer_graph) {
            
            b.chres.t_flatten_alloc = 0;
            b.chres.t_flatten_fill = 0;

            // Graphe de pointeurs
            // Lecture + fill
            if ( (b.mode == sycl_mode::host_USM) // aucun support pour device
            ||   (b.mode == sycl_mode::shared_USM)
            ||   (b.mode == sycl_mode::glibc) ) {

                if (implicit_use_unique_module) {
                    // Utilisation d'un unique module pour les in/out

                    if (b.mode == sycl_mode::host_USM) {
                        b.implicit_modules  = cl::sycl::malloc_host<implicit_module>(total_module_count,  b.sycl_q);
                        b.sycl_q.wait_and_throw();
                    }
                    if (b.mode == sycl_mode::glibc) {
                        b.implicit_modules  = new implicit_module[total_module_count];
                    }
                    if (b.mode == sycl_mode::shared_USM) {
                        b.implicit_modules  = cl::sycl::malloc_shared<implicit_module>(total_module_count,  b.sycl_q);;
                        b.sycl_q.wait_and_throw();
                    }

                    // Allocation des modules, les uns après les autres
                    for (uint im = 0; im < total_module_count; ++im) {
                        traccc::implicit_module  * module  = &b.implicit_modules[im];
                        // lecture du nombre de cellules du module
                        unsigned int cell_count = read_source();
                        module->cell_count = cell_count;
                        module->cluster_count = 0;

                        // allocation des cellules
                        if (b.mode == sycl_mode::host_USM) {
                            module->cells  = cl::sycl::malloc_host<implicit_cell>(cell_count,  b.sycl_q);
                            b.sycl_q.wait_and_throw();
                        }
                        if (b.mode == sycl_mode::glibc) {
                            module->cells  = new implicit_cell[cell_count];
                        }
                        if (b.mode == sycl_mode::shared_USM) {
                            module->cells  = cl::sycl::malloc_shared<implicit_cell>(cell_count,  b.sycl_q);
                            b.sycl_q.wait_and_throw();
                        }
                        
                        // Remplissage des cellules
                        for (uint ic = 0; ic < cell_count; ++ic) {
                            implicit_cell * cell = &module->cells[ic];
                            unsigned int c0 = read_source();
                            unsigned int c1 = read_source();
                            cell->channel0 = c0;
                            cell->channel1 = c1;
                            //if (im < 10) logs( "(" + std::to_string(c0) + ", " + std::to_string(c1) + ") ");
                        }
                        //if (im < 10) log("");
                    }
                } else {
                    // Utilisation des modules in/out

                    if (b.mode == sycl_mode::host_USM) {
                        b.implicit_modules_in  = static_cast<implicit_input_module *>  (cl::sycl::malloc_host(total_module_count * sizeof(implicit_input_module),  b.sycl_q));
                        b.implicit_modules_out = static_cast<implicit_output_module *> (cl::sycl::malloc_host(total_module_count * sizeof(implicit_output_module), b.sycl_q));
                        b.sycl_q.wait_and_throw();
                        //implicit_modules_in  = static_cast<implicit_input_module *>  (cl::sycl::malloc_host(total_module_count,  sycl_q));
                        //implicit_modules_out = static_cast<implicit_output_module *> (cl::sycl::malloc_host(total_module_count, sycl_q));
                    }
                    if (b.mode == sycl_mode::glibc) {
                        b.implicit_modules_in  = new implicit_input_module[total_module_count];
                        b.implicit_modules_out = new implicit_output_module[total_module_count];
                    }
                    if (b.mode == sycl_mode::shared_USM) {
                        b.implicit_modules_in =  static_cast<implicit_input_module *>  (cl::sycl::malloc_shared(total_module_count * sizeof(implicit_input_module),  b.sycl_q));
                        b.implicit_modules_out = static_cast<implicit_output_module *> (cl::sycl::malloc_shared(total_module_count * sizeof(implicit_output_module), b.sycl_q));
                        b.sycl_q.wait_and_throw();
                    }

                    // Allocation des modules, les uns après les autres
                    for (uint im = 0; im < total_module_count; ++im) {
                        traccc::implicit_input_module  * module_in  = &b.implicit_modules_in[im];
                        traccc::implicit_output_module * module_out = &b.implicit_modules_out[im];
                        // lecture du nombre de cellules du module
                        unsigned int cell_count = read_source();
                        module_in->cell_count = cell_count;
                        // allocation des cellules
                        if (b.mode == sycl_mode::host_USM) {
                            module_in->cells  = static_cast<input_cell *>  (cl::sycl::malloc_host(cell_count * sizeof(input_cell),  b.sycl_q));
                            module_out->cells = static_cast<output_cell *> (cl::sycl::malloc_host(cell_count * sizeof(output_cell), b.sycl_q));
                            b.sycl_q.wait_and_throw();
                            module_out->cluster_count = 0;
                        }
                        if (b.mode == sycl_mode::glibc) {
                            module_in->cells  = new input_cell[cell_count];
                            module_out->cells = new output_cell[cell_count];
                            module_out->cluster_count = 0;
                        }
                        if (b.mode == sycl_mode::shared_USM) {
                            module_in->cells  = static_cast<input_cell *>  (cl::sycl::malloc_shared(cell_count * sizeof(input_cell),  b.sycl_q));
                            module_out->cells = static_cast<output_cell *> (cl::sycl::malloc_shared(cell_count * sizeof(output_cell), b.sycl_q));
                            b.sycl_q.wait_and_throw();
                            module_out->cluster_count = 0;
                        }
                        
                        /*if (im < 10) {
                            log("Module " + std::to_string(im)
                                + " cell_count(" + std::to_string(cell_count) + ")"
                            );

                            logs("cells : ");
                        }*/

                        // Remplissage des cellules
                        for (uint ic = 0; ic < cell_count; ++ic) {
                            unsigned int c0 = read_source();
                            unsigned int c1 = read_source();
                            module_in->cells[ic].channel0 = c0;
                            module_in->cells[ic].channel1 = c1;
                            //if (im < 10) logs( "(" + std::to_string(c0) + ", " + std::to_string(c1) + ") ");
                        }
                        //if (im < 10) log("");
                    }
                }
            }
        } else { // flatten
            chrono_flatten.reset();

            // Alloc
            if (b.mode == sycl_mode::glibc) {
                b.flat_input.cells  = new input_cell[total_cell_count];
                b.flat_output.cells = new output_cell[total_cell_count];
                b.flat_input.modules = new flat_input_module[total_module_count];
                b.flat_output.modules = new flat_output_module[total_module_count];
            }

            // Host ou device, le device fera ensuite une allocation explicite
            if ( (b.mode == sycl_mode::host_USM) || b.mode == sycl_mode::device_USM ) {
                b.flat_input.cells  = static_cast<input_cell *>  (cl::sycl::malloc_host(total_cell_count * sizeof(input_cell),  b.sycl_q));
                b.flat_output.cells = static_cast<output_cell *> (cl::sycl::malloc_host(total_cell_count * sizeof(output_cell), b.sycl_q));
                b.flat_input.modules  = static_cast<flat_input_module *>  (cl::sycl::malloc_host(total_module_count * sizeof(flat_input_module),  b.sycl_q));
                b.flat_output.modules = static_cast<flat_output_module *> (cl::sycl::malloc_host(total_module_count * sizeof(flat_output_module), b.sycl_q));
            }

            // Donc allocation host + allocation device
            if ( b.mode == sycl_mode::device_USM ) {
                b.flat_input.cells_device  = cl::sycl::malloc_device<input_cell>(total_cell_count,  b.sycl_q);
                b.flat_output.cells_device = cl::sycl::malloc_device<output_cell>(total_cell_count, b.sycl_q);
                b.flat_input.modules_device  = cl::sycl::malloc_host<flat_input_module>(total_module_count,  b.sycl_q);
                b.flat_output.modules_device = cl::sycl::malloc_host<flat_output_module>(total_module_count, b.sycl_q);
            }

            if (b.mode == sycl_mode::shared_USM) {
                b.flat_input.cells  = static_cast<input_cell *>  (cl::sycl::malloc_shared(total_cell_count * sizeof(input_cell),  b.sycl_q));
                b.flat_output.cells = static_cast<output_cell *> (cl::sycl::malloc_shared(total_cell_count * sizeof(output_cell), b.sycl_q));
                b.flat_input.modules  = static_cast<flat_input_module *>  (cl::sycl::malloc_shared(total_module_count * sizeof(flat_input_module),  b.sycl_q));
                b.flat_output.modules = static_cast<flat_output_module *> (cl::sycl::malloc_shared(total_module_count * sizeof(flat_output_module), b.sycl_q));
            }

            //if (ignore_allocation_times) chrono.reset();
            b.chres.t_flatten_alloc = chrono_flatten.reset();

            // Fill
            unsigned int global_cell_index = 0;
            // Allocation des modules, les uns après les autres
            for (uint im = 0; im < total_module_count; ++im) {
                // lecture du nombre de cellules du module
                unsigned int cell_count = read_source();
                flat_input_module * module_in = &b.flat_input.modules[im];
                module_in->cell_count = cell_count;
                module_in->cell_start_index = global_cell_index;

                // Remplissage des cellules
                for (uint ic = 0; ic < cell_count; ++ic) {
                    input_cell * cell = &b.flat_input.cells[global_cell_index++];
                    unsigned int c0 = read_source();
                    unsigned int c1 = read_source();
                    cell->channel0 = c0;
                    cell->channel1 = c1;
                    //if (im < 10) logs( "(" + std::to_string(c0) + ", " + std::to_string(c1) + ") ");
                }
                //if (im < 10) log("");
            }
            b.chres.t_flatten_fill = chrono_flatten.reset();
        }

        b.sycl_q.wait_and_throw();
        if (TRACCC_LOG_LEVEL >= 2) log("Alloc & fill ok. -----");

        b.chres.t_alloc_fill = chrono.reset();

        if (microseconds != 0) usleep(microseconds);
    }



    void parallel_compute(bench_variables & b) {
        if (TRACCC_LOG_LEVEL >= 2) log("Parallel_for...");
        stime_utils chrono;
        chrono.reset();

        //sycl_mode mode = mode;
        //traccc::implicit_input_module  * implicit_modules_in  = bench.implicit_modules_in;
        //traccc::implicit_output_module * implicit_modules_out = bench.implicit_modules_out;

        if (b.mstrat == pointer_graph) {

            // Exécution du kernel
            if ( (b.mode == sycl_mode::host_USM)
            ||   (b.mode == sycl_mode::shared_USM) ) {

                if (implicit_use_unique_module) {
                    // Utiliser un seul module

                    // ==== parallel for ====
                    class MyKernel_ab;

                    const unsigned int total_module_count_const = total_module_count;
                    const unsigned int max_cell_count_per_module = 1000;

                    traccc::implicit_module  * implicit_modules_kern  = b.implicit_modules;

                    //uint rep = module_count;
                    b.sycl_q.parallel_for<MyKernel_ab>(cl::sycl::range<1>(total_module_count_const), [=](cl::sycl::id<1> module_indexx) {

                        uint module_index = module_indexx[0] % total_module_count_const;
                        // ---- SparseCCL part ----

                        traccc::implicit_module  * module =  &implicit_modules_kern[module_index];

                        uint cell_count = module->cell_count;

                        // The very dirty part : statically allocate a buffer of the maximum pixel density per module...
                        uint L[max_cell_count_per_module];

                        for (uint ic = 0; ic < cell_count; ++ic) {
                            module->cells[ic].label = 0;
                            // init oublié ?
                            L[ic] = 0; /// max_cell_count_per_module
                        }

                        unsigned int start_j = 0;
                        for (unsigned int i=0; i < cell_count; ++i){
                            L[i] = i;
                            int ai = i;
                            if (i > 0){

                                const implicit_cell &ci = module->cells[i];

                                for (unsigned int j = start_j; j < i; ++j){
                                    const implicit_cell &cj = module->cells[j];
                                    if (is_adjacent(ci, cj)){
                                        ai = make_union(L, ai, find_root(L, j));
                                    } else if (is_far_enough(ci, cj)){
                                        ++start_j;
                                    }
                                }
                            }
                        }

                        // second scan: transitive closure
                        uint labels = 0;
                        for (unsigned int i = 0; i < cell_count; ++i){
                            unsigned int l = 0;
                            if (L[i] == i){
                                ++labels;
                                l = labels; 
                            } else {
                                l = L[L[i]];
                            }
                            L[i] = l;
                        }

                        // Update the output values
                        for (unsigned int i = 0; i < cell_count; ++i){
                            module->cells[i].label = L[i];
                        }
                        module->cluster_count = labels;
                    });

                    b.sycl_q.wait_and_throw();

                } else {
                    // utiliser les modules in.out

                    // ==== parallel for ====
                    class MyKernel_aa;

                    const unsigned int total_module_count_const = total_module_count;
                    const unsigned int max_cell_count_per_module = 1000;

                    //const traccc::implicit_input_module  * implicit_modules_in_kern  = implicit_modules_in;
                    //traccc::implicit_output_module * implicit_modules_out_kern = implicit_modules_out;

                    traccc::implicit_input_module  * implicit_modules_in_kern  = b.implicit_modules_in;
                    traccc::implicit_output_module * implicit_modules_out_kern = b.implicit_modules_out;

                    //uint rep = module_count;
                    b.sycl_q.parallel_for<MyKernel_aa>(cl::sycl::range<1>(total_module_count_const), [=](cl::sycl::id<1> module_indexx) {

                        uint module_index = module_indexx[0] % total_module_count_const;
                        // ---- SparseCCL part ----

                        traccc::implicit_input_module  * module_in =  &implicit_modules_in_kern[module_index];
                        traccc::implicit_output_module * module_out = &implicit_modules_out_kern[module_index];

                        uint cell_count = module_in->cell_count;

                        // The very dirty part : statically allocate a buffer of the maximum pixel density per module...
                        uint L[max_cell_count_per_module];

                        for (uint ic = 0; ic < cell_count; ++ic) {
                            module_out->cells[ic].label = 0;
                            // init oublié ?
                            L[ic] = 0; /// max_cell_count_per_module
                        }

                        unsigned int start_j = 0;
                        for (unsigned int i=0; i < cell_count; ++i){
                            L[i] = i;
                            int ai = i;
                            if (i > 0){

                                const input_cell &ci = module_in->cells[i];

                                for (unsigned int j = start_j; j < i; ++j){
                                    const input_cell &cj = module_in->cells[j];
                                    if (is_adjacent(ci, cj)){
                                        ai = make_union(L, ai, find_root(L, j));
                                    } else if (is_far_enough(ci, cj)){
                                        ++start_j;
                                    }
                                }
                            }
                        }

                        // second scan: transitive closure
                        uint labels = 0;
                        for (unsigned int i = 0; i < cell_count; ++i){
                            unsigned int l = 0;
                            if (L[i] == i){
                                ++labels;
                                l = labels; 
                            } else {
                                l = L[L[i]];
                            }
                            L[i] = l;
                        }

                        // Update the output values
                        for (unsigned int i = 0; i < cell_count; ++i){
                            module_out->cells[i].label = L[i];
                        }
                        module_out->cluster_count = labels;
                    });

                    b.sycl_q.wait_and_throw();
                }
            }


            // Exécution du kernel
            if ( b.mode == sycl_mode::glibc ) {

                if (implicit_use_unique_module) {
                    // ==== parallel for ====

                    const unsigned int total_module_count_const = total_module_count;
                    const unsigned int max_cell_count_per_module = 1000;

                    //uint rep = module_count;
                    for (uint module_index = 0; module_index < total_module_count_const; ++module_index) {
                        // ---- SparseCCL part ----
                        //log("module_index " + std::to_string(module_index));

                        traccc::implicit_module  * module =  &b.implicit_modules[module_index];

                        uint cell_count = module->cell_count;

                        // The very dirty part : statically allocate a buffer of the maximum pixel density per module...
                        uint L[max_cell_count_per_module];

                        for (uint ic = 0; ic < cell_count; ++ic) {
                            module->cells[ic].label = 0;
                            L[ic] = 0;
                        }
                        

                        unsigned int start_j = 0;
                        for (unsigned int i=0; i < cell_count; ++i){
                            L[i] = i;
                            int ai = i;
                            if (i > 0){

                                const implicit_cell &ci = module->cells[i];
                                for (unsigned int j = start_j; j < i; ++j){
                                    const implicit_cell &cj = module->cells[j];
                                    if (is_adjacent(ci, cj)){
                                        ai = make_union(L, ai, find_root(L, j));
                                    } else if (is_far_enough(ci, cj)){
                                        ++start_j;
                                    }
                                }
                            }
                        }
                        
                        // second scan: transitive closure
                        uint labels = 0;
                        for (unsigned int i = 0; i < cell_count; ++i){
                            unsigned int l = 0;
                            if (L[i] == i){
                                ++labels;
                                l = labels; 
                            } else {
                                l = L[L[i]];
                            }
                            L[i] = l;
                        }

                        // Update the output values
                        for (unsigned int i = 0; i < cell_count; ++i){
                            module->cells[i].label = L[i];
                        }
                        module->cluster_count = labels;
                        // erreur de marde -> module_out[module_index].cluster_count = labels;
                    }

                } else {
                    // ==== parallel for ====

                    const unsigned int total_module_count_const = total_module_count;
                    const unsigned int max_cell_count_per_module = 1000;

                    //uint rep = module_count;
                    for (uint module_index = 0; module_index < total_module_count_const; ++module_index) {
                        // ---- SparseCCL part ----
                        //log("module_index " + std::to_string(module_index));

                        traccc::implicit_input_module  * module_in =  &b.implicit_modules_in[module_index];
                        traccc::implicit_output_module * module_out = &b.implicit_modules_out[module_index];

                        uint cell_count = module_in->cell_count;

                        // The very dirty part : statically allocate a buffer of the maximum pixel density per module...
                        uint L[max_cell_count_per_module];

                        for (uint ic = 0; ic < cell_count; ++ic) {
                            module_out->cells[ic].label = 0;
                            L[ic] = 0;
                        }
                        

                        unsigned int start_j = 0;
                        for (unsigned int i=0; i < cell_count; ++i){
                            L[i] = i;
                            int ai = i;
                            if (i > 0){

                                const input_cell &ci = module_in->cells[i];
                                for (unsigned int j = start_j; j < i; ++j){
                                    const input_cell &cj = module_in->cells[j];
                                    if (is_adjacent(ci, cj)){
                                        ai = make_union(L, ai, find_root(L, j));
                                    } else if (is_far_enough(ci, cj)){
                                        ++start_j;
                                    }
                                }
                            }
                        }
                        
                        // second scan: transitive closure
                        uint labels = 0;
                        for (unsigned int i = 0; i < cell_count; ++i){
                            unsigned int l = 0;
                            if (L[i] == i){
                                ++labels;
                                l = labels; 
                            } else {
                                l = L[L[i]];
                            }
                            L[i] = l;
                        }

                        // Update the output values
                        for (unsigned int i = 0; i < cell_count; ++i){
                            module_out->cells[i].label = L[i];
                        }
                        module_out->cluster_count = labels;
                        // erreur de marde -> module_out[module_index].cluster_count = labels;
                    }
                }
            }
        } else { // flat structure

            // Exécution du kernel
            if ( (b.mode == sycl_mode::host_USM)
            ||   (b.mode == sycl_mode::shared_USM)
            ||   (b.mode == sycl_mode::device_USM) ) {
                // ==== parallel for ====
                class MyKernel_flat;

                const unsigned int total_module_count_const = total_module_count;
                const unsigned int max_cell_count_per_module = 1000;

                //const traccc::implicit_input_module  * implicit_modules_in_kern  = implicit_modules_in;
                //traccc::implicit_output_module * implicit_modules_out_kern = implicit_modules_out;
                
                // Input data
                traccc::flat_input_module * flat_modules_in_kern;
                traccc::input_cell * flat_cells_in_kern;

                // Output data
                traccc::flat_output_module * flat_modules_out_kern;
                traccc::output_cell * flat_cells_out_kern;

                // Device : transfert explicite
                if (b.mode == sycl_mode::device_USM) {
                    b.sycl_q.memcpy(b.flat_input.modules_device, b.flat_input.modules, total_module_count * sizeof(flat_input_module));
                    b.sycl_q.memcpy(b.flat_input.cells_device, b.flat_input.cells, total_cell_count * sizeof(input_cell));
                    b.sycl_q.wait_and_throw();

                    flat_modules_in_kern = b.flat_input.modules_device;
                    flat_cells_in_kern = b.flat_input.cells_device;
                    flat_modules_out_kern  = b.flat_output.modules_device;
                    flat_cells_out_kern  = b.flat_output.cells_device;

                } else {
                    // Mémoire host ou shared
                    flat_modules_in_kern = b.flat_input.modules;
                    flat_cells_in_kern = b.flat_input.cells;
                    flat_modules_out_kern  = b.flat_output.modules;
                    flat_cells_out_kern  = b.flat_output.cells;
                }

                

                //uint rep = module_count;
                b.sycl_q.parallel_for<MyKernel_flat>(cl::sycl::range<1>(total_module_count_const), [=](cl::sycl::id<1> module_indexx) {

                    uint module_index = module_indexx[0] % total_module_count_const;
                    // ---- SparseCCL part ----

                    //traccc::flat_input_module * module_in

                    uint first_cindex = flat_modules_in_kern[module_index].cell_start_index;
                    uint cell_count = flat_modules_in_kern[module_index].cell_count;
                    uint cell_index = first_cindex;
                    uint stop_cindex = first_cindex + cell_count;

                    // ...

                    // The very dirty part : statically allocate a buffer of the maximum pixel density per module...
                    uint L[max_cell_count_per_module];

                    for (uint ic = 0; ic < cell_count; ++ic) {
                        flat_cells_out_kern[first_cindex + ic].label = 0;
                        // init oublié ?
                        L[ic] = 0; /// max_cell_count_per_module
                    }

                    unsigned int start_j = 0;
                    for (unsigned int i=0; i < cell_count; ++i){
                        L[i] = i;
                        int ai = i;
                        if (i > 0){

                            const input_cell &ci = flat_cells_in_kern[first_cindex + i];

                            for (unsigned int j = start_j; j < i; ++j){
                                const input_cell &cj = flat_cells_in_kern[first_cindex + j];
                                if (is_adjacent(ci, cj)){
                                    ai = make_union(L, ai, find_root(L, j));
                                } else if (is_far_enough(ci, cj)){
                                    ++start_j;
                                }
                            }
                        }
                    }

                    // second scan: transitive closure
                    uint labels = 0;
                    for (unsigned int i = 0; i < cell_count; ++i){
                        unsigned int l = 0;
                        if (L[i] == i){
                            ++labels;
                            l = labels; 
                        } else {
                            l = L[L[i]];
                        }
                        L[i] = l;
                    }

                    // Update the output values
                    for (unsigned int i = 0; i < cell_count; ++i){
                        flat_cells_out_kern[first_cindex + i].label = L[i];
                    }
                    flat_modules_out_kern[module_index].cluster_count = labels;
                });

                b.sycl_q.wait_and_throw();

                // Device : transfert explicite
                if (b.mode == sycl_mode::device_USM) {
                    b.sycl_q.memcpy(b.flat_output.modules, b.flat_output.modules_device, total_module_count * sizeof(flat_output_module));
                    b.sycl_q.memcpy(b.flat_output.cells, b.flat_output.cells_device, total_cell_count * sizeof(output_cell));
                    b.sycl_q.wait_and_throw();
                }


            }
            // Exécution du kernel
            if ( b.mode == sycl_mode::glibc ) {
                // ==== parallel for ====
                class MyKernel_a;

                const unsigned int total_module_count_const = total_module_count;
                const unsigned int max_cell_count_per_module = 1000;

                //const traccc::implicit_input_module  * implicit_modules_in_kern  = implicit_modules_in;
                //traccc::implicit_output_module * implicit_modules_out_kern = implicit_modules_out;

                // Input data
                traccc::flat_input_module * flat_modules_in_kern  = b.flat_input.modules;
                traccc::input_cell * flat_cells_in_kern  = b.flat_input.cells;

                // Output data
                traccc::flat_output_module * flat_modules_out_kern  = b.flat_output.modules;
                traccc::output_cell * flat_cells_out_kern  = b.flat_output.cells;

                for (uint module_index = 0; module_index < total_module_count_const; ++module_index) {
                    
                    uint first_cindex = flat_modules_in_kern[module_index].cell_start_index;
                    uint cell_count = flat_modules_in_kern[module_index].cell_count;
                    uint cell_index = first_cindex;
                    uint stop_cindex = first_cindex + cell_count;

                    // The very dirty part : statically allocate a buffer of the maximum pixel density per module...
                    uint L[max_cell_count_per_module];

                    for (uint ic = 0; ic < cell_count; ++ic) {
                        flat_cells_out_kern[first_cindex + ic].label = 0;
                        // init oublié ?
                        L[ic] = 0; /// max_cell_count_per_module
                    }

                    unsigned int start_j = 0;
                    for (unsigned int i=0; i < cell_count; ++i){
                        L[i] = i;
                        int ai = i;
                        if (i > 0){

                            const input_cell &ci = flat_cells_in_kern[first_cindex + i];

                            for (unsigned int j = start_j; j < i; ++j){
                                const input_cell &cj = flat_cells_in_kern[first_cindex + j];
                                if (is_adjacent(ci, cj)){
                                    ai = make_union(L, ai, find_root(L, j));
                                } else if (is_far_enough(ci, cj)){
                                    ++start_j;
                                }
                            }
                        }
                    }

                    // second scan: transitive closure
                    uint labels = 0;
                    for (unsigned int i = 0; i < cell_count; ++i){
                        unsigned int l = 0;
                        if (L[i] == i){
                            ++labels;
                            l = labels; 
                        } else {
                            l = L[L[i]];
                        }
                        L[i] = l;
                    }

                    // Update the output values
                    for (unsigned int i = 0; i < cell_count; ++i){
                        flat_cells_out_kern[first_cindex + i].label = L[i];
                    }
                    flat_modules_out_kern[module_index].cluster_count = labels;
                }
            }
        }
        b.chres.t_copy_kernel = chrono.reset();

        if (TRACCC_LOG_LEVEL >= 2) log("Parallel for ok.");
        if (microseconds != 0) usleep(microseconds);
    }

    void read_memory(bench_variables & b) {
        if (TRACCC_LOG_LEVEL >= 2) log("Read memory...");

        stime_utils chrono;
        chrono.reset();

        // Lecture des données en sortie
        uint total_cluster_count = 0;
        uint labels_sum = 0;

        if (b.mstrat == pointer_graph) {

            if (implicit_use_unique_module) {
                // un seul module pour les in/out

                for (int module_index = 0; module_index < total_module_count; ++module_index) {
                    total_cluster_count += b.implicit_modules[module_index].cluster_count;

                    // Somme de tous les labels des cellules
                    uint cell_count = b.implicit_modules[module_index].cell_count;
                    implicit_module * module = &b.implicit_modules[module_index];
                    for (uint ic = 0; ic < cell_count; ++ic) {
                        labels_sum += module->cells[ic].label;
                    }
                }

            } else { // utilisation des modules in/out

                for (int module_index = 0; module_index < total_module_count; ++module_index) {
                    total_cluster_count += b.implicit_modules_out[module_index].cluster_count;

                    // Somme de tous les labels des cellules
                    uint cell_count = b.implicit_modules_in[module_index].cell_count;
                    implicit_output_module * module_out = &b.implicit_modules_out[module_index];
                    for (uint ic = 0; ic < cell_count; ++ic) {
                        labels_sum += module_out->cells[ic].label;
                    }
                }
            }
        } else {
            // Valable pour tout : glibc, device, host et shared.
            for (int module_index = 0; module_index < total_module_count; ++module_index) {
                total_cluster_count += b.flat_output.modules[module_index].cluster_count;
            }

            for (uint ic = 0; ic < total_cell_count; ++ic) {
                labels_sum += b.flat_output.cells[ic].label;
            }

        }

        if ( (total_cluster_count != expected_cluster_count) || (labels_sum != expected_label_sum) ) {
            logs("\n    ERROR [[[ clusters(" + std::to_string(total_cluster_count)
                + " != expected " + std::to_string(expected_cluster_count) + ")"
                + " labels(" + std::to_string(labels_sum) + " != expected " + std::to_string(expected_label_sum) + ") ]]]   ");
        } else {
            logs("\n    OK [[[ clusters(" + std::to_string(total_cluster_count) + ")  labels(" + std::to_string(labels_sum) + ") ]]]   ");
        }
        
        

        b.chres.t_read = chrono.reset();
        //log("Read - time value = " + std::to_string(b.chres.t_read));
        if (TRACCC_LOG_LEVEL >= 2) log("Read memory ok.");
    }

    void free_memory(bench_variables & b) {

        if (TRACCC_LOG_LEVEL >= 2) log("Free memory...");

        stime_utils chrono;
        chrono.reset();

        if (b.mstrat == mem_strategy::pointer_graph) {
            // Free memory
            if ( (b.mode == sycl_mode::host_USM)
            ||   (b.mode == sycl_mode::shared_USM)
            ||   (b.mode == sycl_mode::glibc) ) {

                if (implicit_use_unique_module) {
                    // un seul module pour les in/out

                    // Libération de la mémoire des listes de cellules de chaque module
                    for (uint im = 0; im < total_module_count; ++im) {
                        traccc::implicit_module  * module  = &b.implicit_modules[im];

                        if ( (b.mode == sycl_mode::host_USM) || (b.mode == sycl_mode::shared_USM) ) {
                            cl::sycl::free(module->cells, b.sycl_q);
                        }
                        if (b.mode == sycl_mode::glibc) {
                            delete[] module->cells;
                        }
                    }

                    // Libération de la liste des modules
                    if ( (b.mode == sycl_mode::host_USM) || (b.mode == sycl_mode::shared_USM) ) {
                        cl::sycl::free(b.implicit_modules, b.sycl_q);
                    }
                    if (b.mode == sycl_mode::glibc) {
                        delete[] b.implicit_modules;
                    }
                    
                } else { // utilisation des modules in/out
                    
                    // Libération de la mémoire des listes de cellules de chaque module
                    for (uint im = 0; im < total_module_count; ++im) {
                        traccc::implicit_input_module  * module_in  = &b.implicit_modules_in[im];
                        traccc::implicit_output_module * module_out = &b.implicit_modules_out[im];

                        if ( (b.mode == sycl_mode::host_USM) || (b.mode == sycl_mode::shared_USM) ) {
                            cl::sycl::free(module_in->cells, b.sycl_q);
                            cl::sycl::free(module_out->cells, b.sycl_q);
                        }

                        if (b.mode == sycl_mode::glibc) {
                            delete[] module_in->cells;
                            delete[] module_out->cells;
                        }
                    }

                    // Libération de la liste des modules
                    if ( (b.mode == sycl_mode::host_USM) || (b.mode == sycl_mode::shared_USM) ) {
                        cl::sycl::free(b.implicit_modules_in, b.sycl_q);
                        cl::sycl::free(b.implicit_modules_out, b.sycl_q);
                    }
                    if (b.mode == sycl_mode::glibc) {
                        delete[] b.implicit_modules_in;
                        delete[] b.implicit_modules_out;
                    }
                }
            }
        } else { // flatten

            if (b.mode == sycl_mode::glibc) {
                delete[] b.flat_input.cells;
                delete[] b.flat_output.cells;
                delete[] b.flat_input.modules;
                delete[] b.flat_output.modules;
            }
            if ((b.mode == sycl_mode::host_USM) || (b.mode == sycl_mode::shared_USM) || (b.mode == sycl_mode::device_USM)) {
                cl::sycl::free(b.flat_input.cells, b.sycl_q);
                cl::sycl::free(b.flat_output.cells, b.sycl_q);
                cl::sycl::free(b.flat_input.modules, b.sycl_q);
                cl::sycl::free(b.flat_output.modules, b.sycl_q);
            }
            // En plus pour le device, libération de la mémoire device
            if (b.mode == sycl_mode::device_USM) {
                cl::sycl::free(b.flat_input.cells_device, b.sycl_q);
                cl::sycl::free(b.flat_output.cells_device, b.sycl_q);
                cl::sycl::free(b.flat_input.modules_device, b.sycl_q);
                cl::sycl::free(b.flat_output.modules_device, b.sycl_q);
            }
        }
        
        b.chres.t_free_mem = chrono.reset();

        if (TRACCC_LOG_LEVEL >= 2) log("Free memory ok.");
        if (microseconds != 0) usleep(microseconds);
    }

    traccc_chrono_results traccc_bench(sycl_mode mode, mem_strategy memory_strategy) {

        std::string wdir_tmp = std::filesystem::current_path();
        std::string bin_path = wdir_tmp + "/events_bin/lite_all_events.bin";
        
        read_cells_lite(bin_path);
        // TODO : repeat_count pour les lire plusieurs fois
        // et ainsi simuler plus de données.

        //log("=== Mode " + mode_to_string(mode) + " ===");

        custom_device_selector d_selector;
        try {
            //chrono.reset(); //t_start = get_ms();
            cl::sycl::queue sycl_q(d_selector, exception_handler);
            sycl_q.wait_and_throw();

            bench_variables bench;
            bench.mode = mode;
            bench.mstrat = memory_strategy;
            bench.sycl_q = sycl_q;

            // lecture des modules + allocation, les uns après les autres

            alloc_and_fill(bench);
            
            parallel_compute(bench);

            read_memory(bench);

            free_memory(bench);

            return bench.chres; // résultats chronométrés

            //log("Checks...");


            /*
            unsigned int clan0_sum = 0;
            unsigned int clan1_sum = 0;
            unsigned int chk_sum = 0;

            for (uint im = 0; im < total_module_count; ++im) {
                traccc::implicit_input_module  * module_in  = &implicit_modules_in[im];
                traccc::implicit_output_module * module_out = &implicit_modules_out[im];
                chk_sum += module_in->cell_count * im;
                if (im < 100) {
                    log("Module " + std::to_string(im) + " clusters(" + std::to_string(module_out->cluster_count)
                        + ") nb_cells(" + std::to_string(module_in->cell_count) + ")"
                    );

                    //logs("chk cells : ");
                }
                for (uint ic = 0; ic < module_in->cell_count; ++ic) {
                    traccc::input_cell * cell = &module_in->cells[ic];
                    clan0_sum += cell->channel0;
                    clan1_sum += cell->channel1;
                    //if (im < 10) logs( "(" + std::to_string(cell->channel0) + ", " + std::to_string(cell->channel1) + ") ");
                }
                //if (im < 10) log("");
            }

            // Données sauvegardées OK, donc bug au niveau du SparseCCL
            // TODO : faire l'exécution sur traccc (l'autre git)
            // et voir si j'ai le même résultat erroné avec cette version de SparseCCL.
            log("chan0 sum = " + std::to_string(clan0_sum));
            log("chan1 sum = " + std::to_string(clan1_sum));
            log("chk sum   = " + std::to_string(chk_sum));*/


            

            // Continuer ici.
            
        } catch (cl::sycl::exception const &e) {
            std::cout << "An exception has been caught while processing SyCL code.\n";
            std::terminate();
        }
    }

    void write_chrono_results(traccc_chrono_results cres, std::ofstream& myfile) {
        
        struct traccc_chrono_results {
            uint t_alloc_fill, t_copy_kernel, t_read, t_free_mem;
        };
    }

    void traccc_main_sequence(std::ofstream& write_file, sycl_mode mode, mem_strategy mstrat) {

        // Tous les champs du timer sont initialisés à 0.
        // Aucun n'est réellement utile ici.
        gpu_timer gtimer;

        // Je laisse tous les champs pour que ça reste compatible avec ce qui existe déjà
        write_file 
        
        
        << DATASET_NUMBER << " "
        << in_total_size << " " // INPUT_DATA_SIZE
        << out_total_size << " " // OUTPUT_DATA_SIZE
        
        // vv inutile ici vv
        << PARALLEL_FOR_SIZE << " "
        << VECTOR_SIZE_PER_ITERATION << " "
        // ^^ inutile ici ^^

        << REPEAT_COUNT_REALLOC << " " // ------ utile, nombre de fois que le test doit être lancé (défini dans le main)

        // vv inutile ici vv
        << REPEAT_COUNT_ONLY_PARALLEL << " "
        << 0 << " " // gtimer.t_data_generation_and_ram_allocation
        << 0 << " " // gtimer.t_queue_creation
        // ^^ inutile ici ^^

        << mode_to_int(mode) << " " // ------ utile

        // vv inutile ici vv
        << MEMCOPY_IS_SYCL << " "
        << SIMD_FOR_LOOP << " "
        << USE_NAMED_KERNEL << " "
        << USE_HOST_SYCL_BUFFER_DMA << " "
        // How many times the sum should be repeated
        << REPEAT_COUNT_SUM << " "
        // ^^ inutile ici ^^

        << mem_strategy_to_int(mstrat) << " " // 1 0 - 1 20 - 1 2 ; 2 0 - 2 20 - 2 2 ; 
        // mem strategy  mem location
        // output : 2 0 ; 2 2 ; 1 20
        // fait en GM -> << (ignore_allocation_times ? 1 : 0) << " "
        << implicit_use_unique_module << " "

        // plus tard : intervalles de valeurs pour la sparcité
        << "\n";

        // Allocation and free on device, for each iteration
        for (int rpt = 0; rpt < REPEAT_COUNT_REALLOC; ++rpt) {
            log("Iteration " + std::to_string(rpt+1) + " on " + std::to_string(REPEAT_COUNT_REALLOC), 2);

            traccc_chrono_results cres;

            cres = traccc_bench(mode, mstrat);


            write_file
            // RIen n'est utile jusqu'aux nouveaux champs
            << gtimer.t_allocation << " "
            << gtimer.t_sycl_host_alloc << " " // v6
            << gtimer.t_sycl_host_copy << " " // v6
            << gtimer.t_copy_to_device << " "
            << gtimer.t_sycl_host_free << " " // v6
            // TODO : do the same with alloc/cpy only once
            << gtimer.t_parallel_for << " " 
            << gtimer.t_read_from_device << " "
            << gtimer.t_free_gpu << " "

            // Nouveaux champs
            << cres.t_alloc_fill << " "
            << cres.t_copy_kernel << " "
            << cres.t_read << " " // TODO : faire la somme des labels trouvés, pour avoir une lecture complète
            << cres.t_free_mem << " "
            << cres.t_flatten_alloc << " "
            << cres.t_flatten_fill << " "

            << "\n";

            ++current_iteration_count;
            print_total_progress();

            uint fdiv = 1000; // ms
            logs(
                "\n       allocFill(" + std::to_string(cres.t_alloc_fill / fdiv) + ") "
                + "copyKernel(" + std::to_string(cres.t_copy_kernel / fdiv) + ") "
                + "read(" + std::to_string(cres.t_read / fdiv) + ") "
                + "free(" + std::to_string(cres.t_free_mem / fdiv) + ") "
                + "flatAlloc(" + std::to_string(cres.t_flatten_alloc / fdiv) + ") "
                + "fillAlloc(" + std::to_string(cres.t_flatten_fill / fdiv) + ") ");

            //log("");
        }

    }


    void bench_mem_location_and_strategy(std::ofstream& myfile) {

        //log("============    - L = VECTOR_SIZE_PER_ITERATION = " + std::to_string(VECTOR_SIZE_PER_ITERATION));
        //log("============    - M = PARALLEL_FOR_SIZE = " + std::to_string(PARALLEL_FOR_SIZE));
        
        total_main_seq_runs = 2 * 4;

        mem_strategy memory_strategy;
        
        traccc_chrono_results cres;

        for (int imode = 0; imode <= 3; ++imode) 
        //for (int ignore_at = 0; ignore_at <= 1; ++ignore_at)
        for (int imcp = 0; imcp <= 1; ++imcp)
        {

            switch (imcp) {
            case 0: memory_strategy = mem_strategy::flatten; break;
            case 1: memory_strategy = mem_strategy::pointer_graph; break;
            default : break;
            }

            if ( (memory_strategy == pointer_graph) && ignore_pointer_graph_benchmark ) {
                continue;
            }
            
            if ( (memory_strategy == flatten) && ignore_flatten_benchmark ) {
                continue;
            }

            switch (imode) {
            case 0: CURRENT_MODE = sycl_mode::shared_USM; break;
            case 1: CURRENT_MODE = sycl_mode::glibc; break;
            case 2: CURRENT_MODE = sycl_mode::host_USM; break;
            case 3: CURRENT_MODE = sycl_mode::device_USM; break;
            default : break;
            }

            if ( (CURRENT_MODE == device_USM) && (memory_strategy == pointer_graph) ) {
                continue; // pas de graphe de pointeurs en explicite
            }

            //ignore_allocation_times = (ignore_at == 1);
            
            log("\n");
            log("==== Mode(" + mode_to_string(CURRENT_MODE) + ")  memory_strategy(" + mem_strategy_to_str(memory_strategy) + ") ====");
            traccc_main_sequence(myfile, CURRENT_MODE, memory_strategy);
            log("");
        }
    
    }

    int main_of_traccc(std::function<void(std::ofstream &)> bench_function) {
        std::ofstream myfile;
        std::string wdir_tmp = std::filesystem::current_path();
        std::string wdir = wdir_tmp + "/output_bench/";
        std::string output_file_path = wdir + std::string(OUTPUT_FILE_NAME);

        if ( file_exists_test0(output_file_path) ) {
            log("\n\n\n\n\nFILE ALREADY EXISTS, SKIPPING TEST");
            log("NAME = " + OUTPUT_FILE_NAME + "\n");
            log("FULL PATH = " + output_file_path + "\n\n\n\n\n");
            return 4;
        }

        myfile.open(output_file_path);
        log("");

        log("current_path     = " + wdir);
        log("output_file_name = " + output_file_path);

        if (myfile.is_open()) {
            log("OK, fichier bien ouvert.");
        } else {
            log("ERREUR : échec de l'ouverture du fichier en écriture.");
            return 10;
        }
        log("");

        myfile << DATA_VERSION_TRACCC << "\n";

        std::cout << "============================" << std::endl;
        std::cout << "   SYCL TRACCC benchmark.   " << std::endl;
        std::cout << "============================" << std::endl;
        
        std::cout << OUTPUT_FILE_NAME << std::endl;
        log("");
        log("-------------- " + ver_indicator + " --------------");
        log("");

        if ( ! ignore_pointer_graph_benchmark ) log("-----> Do graph pointer.");
        if ( ! ignore_flatten_benchmark ) log("-----> Do flatten.");
        log("-----> traccc_repeat_load_count(" + std::to_string(traccc_repeat_load_count) + ")");
        //if ( ignore_allocation_times ) log("-----> Ignore allocation times.");
        //else                           log("-----> Count allocation times.");

        list_devices(exception_handler);

        init_progress();

        bench_function(myfile);
        
        myfile.close();
        log("OK, done.");

        /*if ( KEEP_SAME_DATASETS ) {
            // Delete local datasets
            delete_datasets(global_persistent_datasets);
            global_persistent_datasets = nullptr;
        }*/

        return 0;
    }


    void run_single_test_generic_traccc(std::string computer_name,
                             uint test_id, uint run_count) {
        std::string file_name_prefix = "_" + computer_name + "_ld" + std::to_string(traccc_repeat_load_count); // 02
        std::string file_name_const_part = file_name_prefix + "_RUN" + std::to_string(run_count) + ".t";

        switch (test_id) {
        //reset_bench_variables();

        // bench_mem_location_and_strategy

        // Tout est dans le nom de fichiers

        case 1:
            OUTPUT_FILE_NAME = BENCHMARK_VERSION_TRACCC + "_generalFlatten" + file_name_const_part; // TRACCC_OUT_FNAME
            ignore_pointer_graph_benchmark = true;
            ignore_flatten_benchmark = false;
            // inutile ici implicit_use_unique_module = false;
            main_of_traccc(bench_mem_location_and_strategy);
            break;

        case 2:
            OUTPUT_FILE_NAME = BENCHMARK_VERSION_TRACCC + "_generalGraphPtr_uniqueModules" + file_name_const_part; // TRACCC_OUT_FNAME
            ignore_pointer_graph_benchmark = false;
            ignore_flatten_benchmark = true;
            implicit_use_unique_module = true;
            main_of_traccc(bench_mem_location_and_strategy);
            break;

        case 3:
            OUTPUT_FILE_NAME = BENCHMARK_VERSION_TRACCC + "_generalGraphPtr_inOutModules" + file_name_const_part; // TRACCC_OUT_FNAME
            ignore_pointer_graph_benchmark = false;
            ignore_flatten_benchmark = true;
            implicit_use_unique_module = false;
            main_of_traccc(bench_mem_location_and_strategy);
            break;

        case 4:
            OUTPUT_FILE_NAME = BENCHMARK_VERSION_TRACCC + "_generalGraphPtr" + file_name_const_part; // TRACCC_OUT_FNAME
            ignore_pointer_graph_benchmark = false;
            ignore_flatten_benchmark = true;
            // inutile ici implicit_use_unique_module = false;
            main_of_traccc(bench_mem_location_and_strategy);
            break;

        /*case 1:
            OUTPUT_FILE_NAME = BENCHMARK_VERSION_TRACCC + "_generalFlatten_ignoreAlloc" + file_name_const_part; // TRACCC_OUT_FNAME
            ignore_pointer_graph_benchmark = true;
            ignore_flatten_benchmark = false;
            //ignore_allocation_times = true;
            main_of_traccc(bench_mem_location_and_strategy);
            break;
        
        case 2:
            OUTPUT_FILE_NAME = BENCHMARK_VERSION_TRACCC + "_generalFlatten_withAlloc" + file_name_const_part; // TRACCC_OUT_FNAME
            ignore_pointer_graph_benchmark = true;
            ignore_flatten_benchmark = false;
            ignore_allocation_times = false;
            main_of_traccc(bench_mem_location_and_strategy);
            break;

        case 3:
            OUTPUT_FILE_NAME = BENCHMARK_VERSION_TRACCC + "__generalGraphPtr_withAlloc" + file_name_const_part;
            ignore_pointer_graph_benchmark = false;
            ignore_flatten_benchmark = true;
            ignore_allocation_times = false;
            main_of_traccc(bench_mem_location_and_strategy);
            break;

        case 4:
            OUTPUT_FILE_NAME = BENCHMARK_VERSION_TRACCC + "__generalGraphPtr_ignoreAlloc" + file_name_const_part;
            ignore_pointer_graph_benchmark = false;
            ignore_flatten_benchmark = true;
            ignore_allocation_times = true;
            main_of_traccc(bench_mem_location_and_strategy);
            break;*/
        
        default: break;
        }
    }

    // Lancement de tous les tests traccc et écriture dans des fichiers

    // TODO : nombre de fois que les données doivent être répétées
    // (pour simuler plus de données)
    void run_all_traccc_benchs(std::string computer_name, int runs_count = 4) {

        // Tests to compare against, to check graphs validity
        //int test_runs_count = runs_count;
        for (uint irun = 1; irun <= runs_count; ++irun) {
            for (uint itest = 1; itest <= 4; ++itest) {
                run_single_test_generic_traccc(computer_name, itest, irun);
            }
        }
    }



}

/*



*/


// Aussi dans sycl_multi_event_implicit_v2.cpp de traccc github publique

    // Je ne vais plus pas utiliser la structure de traccc :
    // Le but est de mesurer le temps pris par l'allocation, le remplissage,
    // le calcul kernel et la lecture. (CCL + CCA ou je lis depuis le device toutes les cellules ?)
    // -> A mon avis juste SparseCCL pour lire plus de données du device et du coup pouvoir mieux
    // mesurer les performances mémoire.
    // Donc je veux que mes données soient super rapides et simples à lire en entrée.
    // Donc structure de base (donc déjà conversion de la structure utilisée par traccc
    // vers ma structure à moi) :

    // Le plus simple possible est une structure aplatie comme pour le cas explicite que j'ai
    // codé dans traccc. Je vais faire un seul grand tableau pour toutes les données,
    // exactement comme la manière dont je vais stocker ces données sur disque.
    // Le tout est de réduire à l'essentiel les données pour pouvoir se concentrer
    // sur SparseCCL seulement (donc pas le seeding ni le CCA).
    // Contenu du fichier (et de la mémoire RAM) :
    // (nombre de modules)
    // pour chaque module : (nombre de cases) (liste des )

    // Tableau regroupant tous les (pointeurs vers les) modules
    // Un module est composé d'un id (peu importe) et d'un tableau de cellules.
    // Le module pointe vers le tableau de cellules.
    // En host et shared 

    // 1)   Allocation mémoire sycl
    // 1.5) Pour device : allocation mémoire host 
    // 2)   Remplissage mémoire SYCL (depuis données brutes)
    // 2.5) Pour device : copie explicite vers la mémoire device
    // 3)   Parallel_for, exécution du kernel
    // 4.0) Pour device : copie explicite vers la mémoire host
    // 4)   Lecture des données de sortie (somme des labels des cases pour faire une lecture)
    //      et somme du nombre de clusters

    // Résultat attendu :
    // Device : Les allocations initiales du device coutent très cher
    // mais les accès une fois les données sur device sont rapides.
    // Shared : Le remplissage sera très lent, et le temps du kernel
    // plus lent qu'en device.
    // Host : allocation mémoire lente, et kernel lent, mais sinon rapide car
    // ne nécessitant pas de copie. Juste ralenti par le parallel_for.

    // La structure de base de laquelle je pars doit être une grande








/*
        unsigned int module_count;
        rf.read((char *)(&module_count), sizeof(unsigned int));

        fdata[0] = module_count;

        //log("read_cells module_count " + std::to_string(module_count) + " start...");

        //log("read_cells 2");

        //hc_container.headers.reserve(module_count);
        //hc_container.items.reserve(module_count);

        for (std::size_t im = 0; im < module_count; ++im) {
            //log("read_cells im " + std::to_string(im) + " start...");

            unsigned int cell_count;
            rf.read((char *)(&cell_count), sizeof(unsigned int));

            for (std::size_t ic = 0; ic < cell_count; ++ic) {
                unsigned int chan0, chan1;
                rf.read((char *)(&chan0), sizeof(unsigned int));
                rf.read((char *)(&chan1), sizeof(unsigned int));
            }

            traccc::cell_module module;
            rf.read((char *)(&module), sizeof(traccc::cell_module));
            hc_container.headers.push_back(module);

            //hc_container.headers[im] = module;
            //rf.read((char *)(&hc_container.headers[im]), sizeof(traccc::cell_module));
            
            //log("read_cells im " + std::to_string(im) + "cell module loaded");

            //log("read_cells im " + std::to_string(im) + " cells nb = " + std::to_string(cell_count));

            vecmem::vector<traccc::cell> cells;

            cells.reserve(cell_count);
            //log("read_cells im " + std::to_string(im) + " reserve ok");
            for (std::size_t ic = 0; ic < cell_count; ++ic) {
                traccc::cell cell;
                rf.read((char *)(&cell), sizeof(traccc::cell));
                cells.push_back(cell);
            }
            hc_container.items.push_back(cells);
            //log("read_cells im " + std::to_string(im) + " all cells read.");
        }
        */