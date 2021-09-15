/// run_batch_job_betaGraphs(step, computer_name);

/*
For unfinished beta data only.
Does not save to file, only pints graphically.

*/

var step = argument0; // must start at 0
computer_name = argument1; // e.g. "msiNvidia_ST" or "thinkpad"

// daclared in create var total_iterations = tests_per_run * run_number;

if ( step >= traccc_total_iterations) return -1;


// nombre de fois que les fichiers sont chargés (pour simuler + de données)
// traccc_repeat_load_count dans le init()

// premières exécutions avec repeat_count = 1 puis 10 pour les dernières
/*if ( step >= traccc_total_iterations ) {
    step -= traccc_total_iterations;
    traccc_repeat_load_count = 10;
} else {
    traccc_repeat_load_count = 1;
}*/


current_run = 1; //step + 1;

traccc_hide_host = false;

// ON MSI var common_path = "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\";
// ON ordi fixe blanc
common_path = "H:\SYNCTHING\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\";
output_path = common_path + "thinkpad\";

//var size_str = "512MiB";


// traccc_refresh_output_name(); à faire à chaque test

var current_test = step + 9; //5 + step;

//var bench_version = "v06";
/*var bench_test_nb = "A";
var debug_run_prefix = "";
var debug_verid = "g";

//var citer = 1; // iteration count

//for (var current_run = 1; current_run <= run_number; ++current_run) {

var fname_prefix_output_short = "" + bench_test_nb + "_";// + bench_version + "_";
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


//show_message("run_batch_job_traccc current_test = " + string(current_test)
//+ chr(10) + "step = " + string(step) + " traccc_total_iterations = " + string(traccc_total_iterations));

// in out file name suffix
var benchmark_version_traccc = "acts06_";
var bvt = benchmark_version_traccc;
fname_prefix_output_short += benchmark_version_traccc;*/

switch (current_test) {

// TRACCC land

// ===================================================
//H:\SYNCTHING\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\A_acts06_generalGraphPtr_withAlloc_msiNvidia_AT_ld1_RUN1_q1.5.png
// ===== Comparaison des USM en graphe de pointeur =====
case 9:

    g_multiple_xaxis = true;
    g_xgroup_has_own_scale = false;
    g_traccc_draw_graph_ptr = true;
    g_traccc_draw_flatten = false;
    g_traccc_ignore_allocation_time = false;
    traccc_repeat_load_count = traccc_repeat_load_count_base;
    traccc_refresh_output_name();
    
    var graph = batch_add_graph(
    /*output_path*/   output_path,
    /*output_fname*/  fname_prefix_output_short + "generalGraphPtr_withAlloc" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_16,
    /*display_name*/  computer_name + " - ACTS graphPtr - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    bvt + "generalGraphPtr" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    load_draw_save_graph(graph);
    ++g_citer;
    break;

// ===== Comparaison des USM en flatten =====
// -> ça serait bien de l'avoir aussi en without host
case 10:
    g_multiple_xaxis = true;
    g_xgroup_has_own_scale = false;
    g_traccc_draw_graph_ptr = false;
    g_traccc_draw_flatten = true;
    g_traccc_ignore_allocation_time = false;
    traccc_repeat_load_count = traccc_repeat_load_count_base;
    traccc_refresh_output_name();
    var graph = batch_add_graph(
    /*output_path*/   output_path,
    /*output_fname*/  fname_prefix_output_short + "generalFlatten_withAlloc" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_16,
    /*display_name*/  computer_name + " - ACTS flatten - run " + string(current_run)
    );
    batch_add_file( // gr.ptr.
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    bvt + "generalFlatten" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    load_draw_save_graph(graph);
    ++g_citer;
    break;

// ===== Comparaison des USM en graphe de pointeur =====
// -> Sans temps d'allocation


// ===== Comparaison des USM en flatten =====
// -> Sans temps d'allocation
// -> ça serait bien de l'avoir aussi en without host
case 11:
    g_multiple_xaxis = true;
    g_xgroup_has_own_scale = false;
    g_traccc_draw_graph_ptr = false;
    g_traccc_draw_flatten = true;
    g_traccc_ignore_allocation_time = true;
    traccc_hide_host = false;
    traccc_repeat_load_count = traccc_repeat_load_count_base;
    traccc_refresh_output_name();
    var graph = batch_add_graph(
    /*output_path*/   output_path,
    /*output_fname*/  fname_prefix_output_short + "generalFlatten_ignoreAlloc_withHost" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_16,
    /*display_name*/  computer_name + " - ACTS flatten - run " + string(current_run)
    );
    batch_add_file( // gr.ptr.
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    bvt + "generalFlatten" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    load_draw_save_graph(graph);
    ++g_citer;
    break;

// Idem mais sans l'host
case 12:
    g_multiple_xaxis = true;
    g_xgroup_has_own_scale = false;
    g_traccc_draw_graph_ptr = false;
    g_traccc_draw_flatten = true;
    g_traccc_ignore_allocation_time = true;
    traccc_hide_host = true;
    traccc_repeat_load_count = traccc_repeat_load_count_base;
    traccc_refresh_output_name();
    var graph = batch_add_graph(
    /*output_path*/   output_path,
    /*output_fname*/  fname_prefix_output_short + "generalFlatten_ignoreAlloc_ignoreHost" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_16,
    /*display_name*/  computer_name + " - ACTS flatten - run " + string(current_run)
    );
    batch_add_file( // gr.ptr.
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    bvt + "generalFlatten" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    load_draw_save_graph(graph);
    ++g_citer;
    break;



// ===== Tout supperposé, sans alloc =====
case 13:
    g_multiple_xaxis = true;
    g_xgroup_has_own_scale = false;
    g_traccc_draw_graph_ptr = true;
    g_traccc_draw_flatten = true;
    g_traccc_ignore_allocation_time = true;
    traccc_repeat_load_count = traccc_repeat_load_count_base;
    traccc_refresh_output_name();
    var graph = batch_add_graph(
    /*output_path*/   output_path,
    /*output_fname*/  fname_prefix_output_short + "generalAll_ignoreAlloc" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_16,
    /*display_name*/  computer_name + " - ACTS flatten - run " + string(current_run)
    );
    batch_add_file( // gr.ptr.
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    bvt + "generalFlatten" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    batch_add_file( // gr.ptr.
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    bvt + "generalGraphPtr" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    load_draw_save_graph(graph);
    ++g_citer;
    break;

// ===== Tout supperposé, avec alloc =====
case 14: // generalAll_withAlloc_hideHost inutile
    ++g_citer;
    break;

case 15:
    g_multiple_xaxis = true;
    g_xgroup_has_own_scale = false;
    g_traccc_draw_graph_ptr = true;
    g_traccc_draw_flatten = true;
    g_traccc_ignore_allocation_time = false;
    traccc_hide_host = false;
    traccc_repeat_load_count = traccc_repeat_load_count_base;
    traccc_refresh_output_name();
    var graph = batch_add_graph(
    /*output_path*/   output_path,
    /*output_fname*/  fname_prefix_output_short + "generalAll_withAlloc_withHost" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_16,
    /*display_name*/  computer_name + " - ACTS flatten - run " + string(current_run)
    );
    batch_add_file( // gr.ptr.
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    bvt + "generalFlatten" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    batch_add_file( // gr.ptr.
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    bvt + "generalGraphPtr" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
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
case 16:
    
    for (var im = 0; im < 3; ++im) {
        var mem_location;
        switch (im) {
        case 0: mem_location = 0; break; // shared
        case 1: mem_location = 2; break; // host
        case 2: mem_location = 20; break; // glibc
        default: break;
        }
        
        g_multiple_xaxis = true;
        g_xgroup_has_own_scale = false;
        g_traccc_draw_graph_ptr = true;
        g_traccc_draw_flatten = true;
        g_traccc_ptrVsFlat_memLocation = mem_location;
        g_traccc_ignore_allocation_time = false;
        traccc_repeat_load_count = traccc_repeat_load_count_base;
        traccc_refresh_output_name();
        
        var graph = batch_add_graph(
        /*output_path*/   output_path,
        /*output_fname*/  fname_prefix_output_short + "ptrVsFlat_mem" + string(mem_location) + fname_suffix_output_traccc,
        /*use_script*/    draw_some_graph_traccc_17, 
        /*display_name*/  computer_name + " - ACTS mémoire " + mem_location_to_str(mem_location) + " - run " + string(current_run)
        );
        batch_add_file( // gr.ptr.
        /*graph*/       graph,
        /*in_path*/     common_path,
        /*in_fname*/    bvt + "generalFlatten" + fname_suffix_input_traccc,
        /*curve_name*/  "aucun nom", // nom de la courbe associée
        /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
        );
        batch_add_file( // gr.ptr.
        /*graph*/       graph,
        /*in_path*/     common_path,
        /*in_fname*/    bvt + "generalGraphPtr_uniqueModules" + fname_suffix_input_traccc,
        /*curve_name*/  "aucun nom", // nom de la courbe associée
        /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
        );
        load_draw_save_graph(graph);
        ++g_citer;
    }
    break;

// ===== Comparaison de chaque type USM : inout vs unique tableau implicite =====
case 17:
    for (var im = 0; im < 3; ++im) {
        var mem_location;
        switch (im) {
        case 0: mem_location = 0; break; // shared
        case 1: mem_location = 2; break; // host
        case 2: mem_location = -1; break; // tout
        default: break;
        }
        
        g_multiple_xaxis = true;
        g_xgroup_has_own_scale = false;
        g_traccc_draw_graph_ptr = true;
        g_traccc_draw_flatten = true;
        g_traccc_ptrVsFlat_memLocation = mem_location; // -1 pour afficher tout
        g_traccc_ignore_allocation_time = false;
        traccc_repeat_load_count = traccc_repeat_load_count_base;
        traccc_refresh_output_name();
        var graph = batch_add_graph(
        /*output_path*/   output_path,
        /*output_fname*/  fname_prefix_output_short + "inOutVsUnique_mem" + string(mem_location) + fname_suffix_output_traccc,
        /*use_script*/    draw_some_graph_traccc_18, 
        /*display_name*/  computer_name + " - ACTS mémoire " + mem_location_to_str(mem_location) + " - run " + string(current_run)
        );
        batch_add_file( // gr.ptr.
        /*graph*/       graph,
        /*in_path*/     common_path,
        /*in_fname*/    bvt + "generalGraphPtr_inOutModules" + fname_suffix_input_traccc,
        /*curve_name*/  "aucun nom", // nom de la courbe associée
        /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
        );
        batch_add_file( // gr.ptr.
        /*graph*/       graph,
        /*in_path*/     common_path,
        /*in_fname*/    bvt + "generalGraphPtr_uniqueModules" + fname_suffix_input_traccc,
        /*curve_name*/  "aucun nom", // nom de la courbe associée
        /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
        );
        load_draw_save_graph(graph);
        ++g_citer;
    }
    break;

// 
case 18:
    g_multiple_xaxis = true;
    g_xgroup_has_own_scale = false;
    g_traccc_draw_graph_ptr = false;
    g_traccc_draw_flatten = true;
    g_traccc_ignore_allocation_time = false;
    traccc_hide_host = false;
    traccc_repeat_load_count = 10;
    traccc_refresh_output_name();
    var graph = batch_add_graph(
    /*output_path*/   output_path,
    /*output_fname*/  fname_prefix_output_short + "generalFlatten_sparse-1-2" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_16,
    /*display_name*/  computer_name + " - ACTS sparse 1-2" // - run " + string(current_run)
    );
    batch_add_file( // gr.ptr.
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    bvt + "generalFlatten_sparse-1-2" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    load_draw_save_graph(graph);
    ++g_citer;
    break;

case 19:
    g_multiple_xaxis = true;
    g_xgroup_has_own_scale = false;
    g_traccc_draw_graph_ptr = false;
    g_traccc_draw_flatten = true;
    g_traccc_ignore_allocation_time = false;
    traccc_hide_host = false;
    traccc_repeat_load_count = 10;
    traccc_refresh_output_name();
    var graph = batch_add_graph(
    /*output_path*/   output_path,
    /*output_fname*/  fname_prefix_output_short + "generalFlatten_sparse-500-1000" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_16,
    /*display_name*/  computer_name + " - ACTS sparse 500-1000" // - run " + string(current_run)
    );
    batch_add_file( // gr.ptr.
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    bvt + "generalFlatten_sparse-500-1000" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    load_draw_save_graph(graph);
    ++g_citer;
    break;
    
default :
    show_message("ERROR @ run_batch_job_betaGraphs : current_test(" + string(current_test) + ") not handled.");
    break;
}

return 0;
