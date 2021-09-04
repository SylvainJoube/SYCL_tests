/// run_batch_job_betaGraphs(step, computer_name);

/*
For unfinished beta data only.
Does not save to file, only pints graphically.

*/

var step = argument0; // must start at 0
var computer_name = argument1; // e.g. "msiNvidia_ST" or "thinkpad"

// daclared in create var total_iterations = tests_per_run * run_number;

if ( step >= traccc_total_iterations * 2) return -1;


// nombre de fois que les fichiers sont chargés (pour simuler + de données)
// traccc_repeat_load_count dans le init()

// premières exécutions avec repeat_count = 1 puis 10 pour les dernières
if ( step >= traccc_total_iterations ) {
    step -= traccc_total_iterations;
    traccc_repeat_load_count = 10;
} else {
    traccc_repeat_load_count = 1;
}


var current_run = 1; //step + 1;

// ON MSI var common_path = "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\";
// ON ordi fixe blanc
common_path = "H:\SYNCTHING\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\traccc\";

//var size_str = "512MiB";


//var bench_version = "v06";
var bench_test_nb = "A";
var debug_run_prefix = "";
var debug_verid = "g";

//var citer = 1; // iteration count

//for (var current_run = 1; current_run <= run_number; ++current_run) {
    
var fname_prefix_output_short = "b" + bench_test_nb + "_";// + bench_version + "_";
//var fname_prefix_output = "b" + bench_test_nb + "_" + bench_version + "_";
//var fname_prefix_input  = bench_version + "_";

//var fname_suffix_common = "_" + computer_name + "_" + size_str + "_O2_RUN" + string(current_run);
var fname_suffix_common_traccc = "_" + computer_name + "_ld" + string(traccc_repeat_load_count) + "_RUN" + string(current_run);

//var fname_suffix_output = fname_suffix_common + "_q1.5" + ".png";
//var fname_suffix_input  = fname_suffix_common + ".t";

var fname_suffix_output_traccc = fname_suffix_common_traccc + "_q1.5" + ".png";
var fname_suffix_input_traccc  = fname_suffix_common_traccc + ".t";

if (traccc_hide_host) {
    fname_suffix_output_traccc = fname_suffix_common_traccc + "_q1.5_hideHost.png";
} else {
    fname_suffix_output_traccc = fname_suffix_common_traccc + "_q1.5.png";
}

//var file_name_const_part = common_file_name + "_RUN" + string(current_run);// + ".t";
//var local_common_path = common_path + bench_version;
//var file_name_const_part_ouptut_png = file_name_const_part + "_" + bench_test_nb + ".png";

var current_test = step + 9; //5 + step;



switch (current_test) {

// TRACCC land

// ===================================================

// ===== Comparaison des USM en graphe de pointeur =====
case 9:
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output_short + "acts05_generalGraphPtr_withAlloc" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_16,
    /*display_name*/  computer_name + " - ACTS graphPtr - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    "acts05_generalGraphPtr" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = true;
    g_xgroup_has_own_scale = false;
    g_traccc_draw_graph_ptr = true;
    g_traccc_draw_flatten = false;
    g_traccc_ignore_allocation_time = false;
    load_draw_save_graph(graph);
    ++g_citer;
    break;

// ===== Comparaison des USM en flatten =====
case 10:
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output_short + "acts05_generalFlatten_withAlloc" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_16,
    /*display_name*/  computer_name + " - ACTS flatten - run " + string(current_run)
    );
    batch_add_file( // gr.ptr.
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    "acts05_generalFlatten" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = true;
    g_xgroup_has_own_scale = false;
    g_traccc_draw_graph_ptr = false;
    g_traccc_draw_flatten = true;
    g_traccc_ignore_allocation_time = false;
    load_draw_save_graph(graph);
    ++g_citer;
    break;

// ===== Comparaison des USM en graphe de pointeur =====
// -> Sans temps d'allocation
case 11:
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output_short + "acts05_generalGraphPtr_ignoreAlloc" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_16,
    /*display_name*/  computer_name + " - ACTS graphPtr - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    "acts05_generalGraphPtr" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = true;
    g_xgroup_has_own_scale = false;
    g_traccc_draw_graph_ptr = true;
    g_traccc_draw_flatten = false;
    g_traccc_ignore_allocation_time = true;
    load_draw_save_graph(graph);
    ++g_citer;
    break;

// ===== Comparaison des USM en flatten =====
// -> Sans temps d'allocation
case 12:
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output_short + "acts05_generalFlatten_ignoreAlloc" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_16,
    /*display_name*/  computer_name + " - ACTS flatten - run " + string(current_run)
    );
    batch_add_file( // gr.ptr.
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    "acts05_generalFlatten" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = true;
    g_xgroup_has_own_scale = false;
    g_traccc_draw_graph_ptr = false;
    g_traccc_draw_flatten = true;
    g_traccc_ignore_allocation_time = true;
    load_draw_save_graph(graph);
    ++g_citer;
    break;



// ===== Tout supperposé, sans alloc =====
case 13:
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output_short + "acts05_generalAll_ignoreAlloc" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_16,
    /*display_name*/  computer_name + " - ACTS flatten - run " + string(current_run)
    );
    batch_add_file( // gr.ptr.
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    "acts05_generalFlatten" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    batch_add_file( // gr.ptr.
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    "acts05_generalGraphPtr" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = true;
    g_xgroup_has_own_scale = false;
    g_traccc_draw_graph_ptr = true;
    g_traccc_draw_flatten = true;
    g_traccc_ignore_allocation_time = true;
    load_draw_save_graph(graph);
    ++g_citer;
    break;

// ===== Tout supperposé, avec alloc =====
case 14:
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output_short + "acts05_generalAll_withAlloc" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_16,
    /*display_name*/  computer_name + " - ACTS flatten - run " + string(current_run)
    );
    batch_add_file( // gr.ptr.
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    "acts05_generalFlatten" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    batch_add_file( // gr.ptr.
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    "acts05_generalGraphPtr" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = true;
    g_xgroup_has_own_scale = false;
    g_traccc_draw_graph_ptr = true;
    g_traccc_draw_flatten = true;
    g_traccc_ignore_allocation_time = false;
    load_draw_save_graph(graph);
    ++g_citer;
    break;
    
// Graphe de pointeurs vs flatten : glibc puis SYCL
/*
case 0 : return "shared";
case 1 : return "device";
case 2 : return "host";
case 3 : return "buffers";
case 20 : return "glibc";
*/
// ===== Comparaison de chaque type USM : graphe ptr vs flatten =====
case 15:
    for (var im = 0; im < 3; ++im) {
        var mem_location;
        switch (im) {
        case 0: mem_location = 0; break; // shared
        case 1: mem_location = 2; break; // host
        case 2: mem_location = 20; break; // glibc
        default: break;
        }
        
        var graph = batch_add_graph(
        /*output_path*/   common_path,
        /*output_fname*/  fname_prefix_output_short + "acts05_ptrVsFlat_mem" + string(mem_location) + fname_suffix_output_traccc,
        /*use_script*/    draw_some_graph_traccc_17, 
        /*display_name*/  computer_name + " - ACTS mémoire " + mem_location_to_str(mem_location) + " - run " + string(current_run)
        );
        batch_add_file( // gr.ptr.
        /*graph*/       graph,
        /*in_path*/     common_path,
        /*in_fname*/    "acts05_generalFlatten" + fname_suffix_input_traccc,
        /*curve_name*/  "aucun nom", // nom de la courbe associée
        /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
        );
        batch_add_file( // gr.ptr.
        /*graph*/       graph,
        /*in_path*/     common_path,
        /*in_fname*/    "acts05_generalGraphPtr_uniqueModules" + fname_suffix_input_traccc,
        /*curve_name*/  "aucun nom", // nom de la courbe associée
        /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
        );
        g_multiple_xaxis = true;
        g_xgroup_has_own_scale = false;
        g_traccc_draw_graph_ptr = true;
        g_traccc_draw_flatten = true;
        g_traccc_ptrVsFlat_memLocation = mem_location;
        g_traccc_ignore_allocation_time = false;
        load_draw_save_graph(graph);
        ++g_citer;
    }
    break;

// ===== Comparaison de chaque type USM : inout vs unique tableau implicite =====
case 16:
    for (var im = 0; im < 3; ++im) {
        var mem_location;
        switch (im) {
        case 0: mem_location = 0; break; // shared
        case 1: mem_location = 2; break; // host
        case 2: mem_location = -1; break; // tout
        default: break;
        }
        
        var graph = batch_add_graph(
        /*output_path*/   common_path,
        /*output_fname*/  fname_prefix_output_short + "acts05_inOutVsUnique_mem" + string(mem_location) + fname_suffix_output_traccc,
        /*use_script*/    draw_some_graph_traccc_18, 
        /*display_name*/  computer_name + " - ACTS mémoire " + mem_location_to_str(mem_location) + " - run " + string(current_run)
        );
        batch_add_file( // gr.ptr.
        /*graph*/       graph,
        /*in_path*/     common_path,
        /*in_fname*/    "acts05_generalGraphPtr_inOutModules" + fname_suffix_input_traccc,
        /*curve_name*/  "aucun nom", // nom de la courbe associée
        /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
        );
        batch_add_file( // gr.ptr.
        /*graph*/       graph,
        /*in_path*/     common_path,
        /*in_fname*/    "acts05_generalGraphPtr_uniqueModules" + fname_suffix_input_traccc,
        /*curve_name*/  "aucun nom", // nom de la courbe associée
        /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
        );
        g_multiple_xaxis = true;
        g_xgroup_has_own_scale = false;
        g_traccc_draw_graph_ptr = true;
        g_traccc_draw_flatten = true;
        g_traccc_ptrVsFlat_memLocation = mem_location; // -1 pour afficher tout
        g_traccc_ignore_allocation_time = false;
        load_draw_save_graph(graph);
        ++g_citer;
    }
    break;
    
    
    
default :
    show_message("ERROR @ run_batch_job_betaGraphs : current_test(" + string(current_test) + ") not handled.");
    break;
}

return 0;
