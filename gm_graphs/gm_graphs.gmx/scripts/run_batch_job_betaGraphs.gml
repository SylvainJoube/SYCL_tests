/// run_batch_job_betaGraphs(step, computer_name);

/*
For unfinished beta data only.
Does not save to file, only pints graphically.

*/

var step = argument0; // must start at 0
var computer_name = argument1; // e.g. "msiNvidia_ST" or "thinkpad"

// daclared in create var total_iterations = tests_per_run * run_number;

if ( step >= total_iterations) return -1;

var current_run = step + 1;

// ON MSI var common_path = "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\";
// ON ordi fixe blanc
common_path = "H:\SYNCTHING\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\";

//var common_path = "C:\Users\sylvain\Desktop\plot_thinkpad_512MiB\";
//var computer_name = "thinkpad";
//var size_str = "1GiB";
var size_str = "512MiB";
//var common_file_name = "_" + computer_name + "_" + size_str + "_O2";


//var bench_version = "v06";
var bench_test_nb = "011";
var debug_run_prefix = "";
var debug_verid = "g";

//var citer = 1; // iteration count

//for (var current_run = 1; current_run <= run_number; ++current_run) {
    
var fname_prefix_output_short = "b" + bench_test_nb + "_";// + bench_version + "_";
//var fname_prefix_output = "b" + bench_test_nb + "_" + bench_version + "_";
//var fname_prefix_input  = bench_version + "_";

var fname_suffix_common = "_" + computer_name + "_" + size_str + "_O2_RUN" + string(current_run);
var fname_suffix_common_traccc = "_" + computer_name + "_O2_RUN" + string(current_run);

var fname_suffix_output = fname_suffix_common + "_q1.5" + ".png";
var fname_suffix_input  = fname_suffix_common + ".t";

var fname_suffix_output_traccc = fname_suffix_common_traccc + "_q1.5" + ".png";
var fname_suffix_input_traccc  = fname_suffix_common_traccc + ".t";

//var file_name_const_part = common_file_name + "_RUN" + string(current_run);// + ".t";
//var local_common_path = common_path + bench_version;
//var file_name_const_part_ouptut_png = file_name_const_part + "_" + bench_test_nb + ".png";

var current_test = 5;



switch (current_test) {

case 1:
    // == sumReadSpeed ==
    // Compare read speeds with USM host, device and shared
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output_short + "v06B_sumReadSpeed" + fname_suffix_output,
    /*use_script*/    draw_some_graph_14throughput,
    /*display_name*/  computer_name + " - L fixé, repeat variable - " + size_str + " - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    "v06B_sumReadSpeed" + fname_suffix_input,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = false;
    load_draw_save_graph(graph);
    ++g_citer;
    break;


case 2:
    // == sumReadSpeed ==
    // Compare read speeds with USM host, device and shared
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output_short + "v06B_sumReadSpeedBandwidth" + fname_suffix_output,
    /*use_script*/    draw_some_graph_14throughputBandwidth,
    /*display_name*/  computer_name + " - L fixé, repeat variable - " + size_str + " - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    "v06B_sumReadSpeed" + fname_suffix_input,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = false;
    load_draw_save_graph(graph);
    ++g_citer;
    break;

case 3:
    // == cacheSize ==
    // Compare read speeds with USM host, device and shared
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output_short + "v06C_cacheSize" + fname_suffix_output,
    /*use_script*/    draw_some_graph_15cacheSize,
    /*display_name*/  computer_name + " - L variable, repeat fixé - " + size_str + " - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    "v06C_cacheSize" + fname_suffix_input,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = false;
    load_draw_save_graph(graph);
    ++g_citer;
    break;

case 4:
    // == cacheSizeBandwidth ==
    // Compare read speeds with USM host, device and shared
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output_short + "v06D_cacheSizeBandwidth" + fname_suffix_output,
    /*use_script*/    draw_some_graph_15cacheSizeBandwidth,
    /*display_name*/  computer_name + " - L variable, repeat fixé - " + size_str + " - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    "v06D_cacheSize" + fname_suffix_input,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = false;
    load_draw_save_graph(graph);
    ++g_citer;
    break;

/*iter.t_alloc_fill = ds_list_find_value(values, 8);
            iter.t_copy_kernel = ds_list_find_value(values, 9);
            iter.t_read = ds_list_find_value(values, 10);
            iter.t_free_mem = ds_list_find_value(values, 11);*/
// TRACCC land
case 5:
    //show_message("run_batch_job_betaGraphs - TEST 5 - current_test = " + string(current_test));
    // == cacheSizeBandwidth ==
    // Compare read speeds with USM host, device and shared
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output_short + "vTRACCC-05_tracccMemLocStrat" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_16,
    /*display_name*/  computer_name + " - ACTS: type mémoire & stratégie - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    "v05_TEMP_tracccMemLocStrat" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = false;
    load_draw_save_graph(graph);
    ++g_citer;
    break;
    
    
    
default :
    show_message("ERROR @ run_batch_job_betaGraphs : current_test(" + string(current_test) + ") not handled.");
    break;
}

return 0;
