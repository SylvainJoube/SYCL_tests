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

    


    using tdtype = unsigned int;
    /*
    Toutes les données stockées sont de type tdtype.
    Pour simplifier les traîtements.
    (nb modules, nb cellules, positions)
    */

    unsigned int* read_cells_lite(std::string fpath);

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

    struct device_input_module {
        unsigned int cell_count; // nombre de cellules du module
        unsigned int cell_start_index; // start index dans le grand tableau des cellules
    };
    struct device_output_module {
        // unsigned int cell_count; connu
        unsigned int cluster_count;
    };

    // Tout aplati
    struct device_input_data {
        input_cell* cells;
        device_input_module* modules;
    };

    struct device_output_data {
        output_cell* cells;
        device_output_module* modules;
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

    unsigned int make_union(output_cell * L, unsigned int e1, unsigned int e2){
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




    unsigned int total_module_count = 0;
    unsigned int total_cell_count = 0;
    unsigned int total_int_written = 0;

    unsigned int* all_data; // allocated with read_cells_lite
    unsigned int i_all_data = 0;

    unsigned int read_source() {
        return all_data[i_all_data++];
    }

    unsigned int* read_cells_lite(std::string fpath) {
        //traccc::host_cell_container cells_per_event;

        long fsize = GetFileSize(fpath);

        //log("read_cells 0");
        std::ifstream rf(fpath, std::ios::out | std::ios::binary);
        
        if(!rf) {
            return nullptr;
        }

        rf.read((char *)(&total_module_count), sizeof(unsigned int));
        rf.read((char *)(&total_cell_count), sizeof(unsigned int));
        rf.read((char *)(&total_int_written), sizeof(unsigned int));

        log("total_module_count = " + std::to_string(total_module_count));
        log("total_cell_count = " + std::to_string(total_cell_count));
        log("total_int_written = " + std::to_string(total_int_written));

        unsigned int nb_ints_chk = (fsize / sizeof(unsigned int)) - 2;

        if (nb_ints_chk != total_int_written) {
            log("ERROR ?   nb_ints_chk(" +std::to_string(nb_ints_chk)
            + ") != total_int_written(" + std::to_string(total_int_written) + ")");

        }

        // fdata = flat data
        unsigned int* fdata = new unsigned int[total_int_written];

        // read the whole remaining file at once
        rf.read((char *)(fdata), total_int_written * sizeof(unsigned int));

        //log("read_cells closing...");
        rf.close();

        all_data = nullptr;

        //log("read_cells closed !");
        if(!rf.good()) {
            delete[] fdata;
            return nullptr;
        }

        all_data = fdata;

        return fdata;
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

    void traccc_bench(sycl_mode mode) {

        std::string wdir_tmp = std::filesystem::current_path();
        std::string bin_path = wdir_tmp + "/events_bin/lite_all_events.bin";

        log("Read from " + bin_path + "...");
        read_cells_lite(bin_path);

        custom_device_selector d_selector;
        try {
            //chrono.reset(); //t_start = get_ms();
            cl::sycl::queue sycl_q(d_selector, exception_handler);
            sycl_q.wait_and_throw();

            // Tableau des modules en implicite
            implicit_input_module*  implicit_modules_in;
            implicit_output_module* implicit_modules_out;

            
            //unsigned int iall = 0;

            // lecture des modules + allocation, les uns après les autres

            log("Alloc & fill...");


            // Graphe de pointeurs
            // Lecture + fill
            if ( (mode == sycl_mode::host_USM)
            ||   (mode == sycl_mode::shared_USM)
            ||   (mode == sycl_mode::glibc) ) {

                if (mode == sycl_mode::host_USM) {
                    implicit_modules_in  = static_cast<implicit_input_module *>  (cl::sycl::malloc_host(total_module_count * sizeof(implicit_input_module),  sycl_q));
                    implicit_modules_out = static_cast<implicit_output_module *> (cl::sycl::malloc_host(total_module_count * sizeof(implicit_output_module), sycl_q));
                }
                if (mode == sycl_mode::glibc) {
                    implicit_modules_in  = new implicit_input_module[total_module_count];
                    implicit_modules_out = new implicit_output_module[total_module_count];
                }
                if (mode == sycl_mode::shared_USM) {
                    implicit_modules_in =  static_cast<implicit_input_module *>  (cl::sycl::malloc_shared(total_module_count * sizeof(implicit_input_module),  sycl_q));
                    implicit_modules_out = static_cast<implicit_output_module *> (cl::sycl::malloc_shared(total_module_count * sizeof(implicit_output_module), sycl_q));
                }

                // Allocation des modules, les uns après les autres
                for (uint im = 0; im < total_module_count; ++im) {
                    traccc::implicit_input_module  * module_in  = &implicit_modules_in[im];
                    traccc::implicit_output_module * module_out = &implicit_modules_out[im];
                    // lecture du nombre de cellules du module
                    unsigned int cell_count = read_source();
                    module_in->cell_count = cell_count;
                    // allocation des cellules
                    if (mode == sycl_mode::host_USM) {
                        module_in->cells = static_cast<input_cell *> (cl::sycl::malloc_host(cell_count * sizeof(input_cell), sycl_q));
                    }
                    if (mode == sycl_mode::glibc) {
                        module_in->cells  = new input_cell[cell_count];
                        module_out->cells = new output_cell[cell_count];
                        module_out->cluster_count = 0;
                    }
                    if (mode == sycl_mode::shared_USM) {
                        module_in->cells = static_cast<input_cell *> (cl::sycl::malloc_shared(cell_count * sizeof(input_cell), sycl_q));
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

            

            log("Parallel_for...");

            // Exécution du kernel
            /*if ( (mode == sycl_mode::host_USM)
            ||   (mode == sycl_mode::shared_USM) ) {

                // ==== parallel for ====
                class MyKernel_a;

                const unsigned int total_module_count_const = total_module_count;
                const unsigned int max_cell_count_per_module = 1000;

                //uint rep = module_count;
                sycl_q.parallel_for<MyKernel_a>(cl::sycl::range<1>(total_module_count_const), [=](cl::sycl::id<1> module_indexx) {

                    uint module_index = module_indexx[0] % total_module_count_const;
                    // ---- SparseCCL part ----

                    traccc::implicit_input_module  * module_in =  &implicit_modules_in[module_index];
                    traccc::implicit_output_module * module_out = &implicit_modules_out[module_index];

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
                    module_out[module_index].cluster_count = labels;
                });

                sycl_q.wait_and_throw();

            }*/


            // Exécution du kernel
            if ( mode == sycl_mode::glibc ) {

                // ==== parallel for ====
                class MyKernel_a;

                const unsigned int total_module_count_const = total_module_count;
                const unsigned int max_cell_count_per_module = 1000;

                //uint rep = module_count;
                for (uint module_index = 0; module_index < total_module_count_const; ++module_index) {
                    // ---- SparseCCL part ----
                    //log("module_index " + std::to_string(module_index));

                    traccc::implicit_input_module  * module_in =  &implicit_modules_in[module_index];
                    traccc::implicit_output_module * module_out = &implicit_modules_out[module_index];

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
                    // erreur de merde -> module_out[module_index].cluster_count = labels;
                };

            }


            // Lecture des données en sortie
            uint total_cluster_count = 0;
            for (int module_index = 0; module_index < total_module_count; ++module_index) {

                total_cluster_count += implicit_modules_out[module_index].cluster_count;
                uint debug_cluster_count = implicit_modules_out[module_index].cluster_count;

                /*if ( module_index % 2 == 0 || true ) {
                    if (module_index < 10) {
                        uint cell_count = implicit_modules_in[module_index].cell_count;
                        log("Module " + std::to_string(module_index) + " clusters(" + std::to_string(debug_cluster_count)
                            + ") cell_count(" + std::to_string(cell_count) + ")"
                        );

                        logs("cells : ");
                        for (uint ic = 0; ic < cell_count; ++ic) {
                            input_cell * cell = &implicit_modules_in[module_index].cells[ic];
                            logs( "(" + std::to_string(cell->channel0) + ", " + std::to_string(cell->channel1) + ") ");
                        }
                        log("");

                    }
                } else {
                    if (debug_cluster_count != 0) {
                        log("Devrait être 0 mais ne l'est pas ! Module " + std::to_string(module_index));
                    }
                }*/
            }

            log("Cluster count = " + std::to_string(total_cluster_count));



            log("Checks...");

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
            log("chk sum   = " + std::to_string(chk_sum));






            // Continuer ici.

            
        } catch (cl::sycl::exception const &e) {
            std::cout << "An exception has been caught while processing SyCL code.\n";
            std::terminate();
        }
    }
}



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