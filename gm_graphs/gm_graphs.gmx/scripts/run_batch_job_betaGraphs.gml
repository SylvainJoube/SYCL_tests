/// run_batch_job_betaGraphs(step, computer_name);

/*
For unfinished beta data only.
Does not save to file, only pints graphically.

*/

var step = argument0; // must start at 0
var computer_name = argument1; // e.g. "msiNvidia_ST" or "thinkpad"

// daclared in create var total_iterations = tests_per_run * run_number;

if ( step >= total_iterations) return -1;

var current_run = step + 1; //step + 1;

// ON MSI var common_path = "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\";
// ON ordi fixe blanc
common_path = "H:\SYNCTHING\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\";
common_path_out = common_path;

//var size_str = "512MiB";
var size_str = "6GiB";
computer_name = "sandor_ST"
// nombre de fois que les fichiers sont chargés (pour simuler + de données)
// traccc_repeat_load_count dans le create de l'objet actuel


//var bench_version = "v06";
var bench_test_nb = "011";
var debug_run_prefix = "";
var debug_verid = "g";

//var citer = 1; // iteration count

//for (var current_run = 1; current_run <= run_number; ++current_run) {
    
var fname_prefix_output_short = "b" + bench_test_nb + "_";// + bench_version + "_";
//var fname_prefix_output = "b" + bench_test_nb + "_" + bench_version + "_";
//var fname_prefix_input  = bench_version + "_";

fname_prefix_input = "v05_";

var fname_suffix_common = "_" + computer_name + "_" + size_str + "_O2_RUN" + string(current_run);
var fname_suffix_common_traccc = "_" + computer_name + "_ld" + string(traccc_repeat_load_count) + "_RUN" + string(current_run);

var fname_suffix_output = fname_suffix_common + "_q1.5" + ".png";
var fname_suffix_input  = fname_suffix_common + ".t";

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

var current_test = 19;// + step; //5 + step;



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
    /*output_fname*/  fname_prefix_output_short + "TR05_memLocStrat6_flat" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_16,
    /*display_name*/  computer_name + " - ACTS: type mémoire & stratégie - données aplaties - run " + string(current_run)
    );
    batch_add_file( // gr.ptr.
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    "v05_TEMP_tracccMemLocStrat6" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = true;
    g_xgroup_has_own_scale = false;
    g_traccc_draw_graph_ptr = false;
    g_traccc_draw_flatten = true;
    load_draw_save_graph(graph);
    ++g_citer;
    break;

case 6:
    //show_message("run_batch_job_betaGraphs - TEST 5 - current_test = " + string(current_test));
    // == cacheSizeBandwidth ==
    // Compare read speeds with USM host, device and shared
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output_short + "TR05_memLocStrat6_graphPtr" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_16,
    /*display_name*/  computer_name + " - ACTS: type mémoire & stratégie - graphe de pointeurs - run " + string(current_run)
    );
    batch_add_file( // gr.ptr.
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    "v05_TEMP_tracccMemLocStrat6" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = true;
    g_xgroup_has_own_scale = false;
    g_traccc_draw_graph_ptr = true;
    g_traccc_draw_flatten = false;
    load_draw_save_graph(graph);
    ++g_citer;
    break;

case 7:
    //show_message("run_batch_job_betaGraphs - TEST 5 - current_test = " + string(current_test));
    // == cacheSizeBandwidth ==
    // Compare read speeds with USM host, device and shared
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output_short + "TR05_memLocStrat6_tout" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_16,
    /*display_name*/  computer_name + " - ACTS: type mémoire & stratégie - tout affiché - run " + string(current_run)
    );
    batch_add_file( // gr.ptr.
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    "v05_TEMP_tracccMemLocStrat6" + fname_suffix_input_traccc,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = true;
    g_xgroup_has_own_scale = false;
    g_traccc_draw_graph_ptr = true;
    g_traccc_draw_flatten = true;
    load_draw_save_graph(graph);
    ++g_citer;
    break;

case 8:
    //show_message("run_batch_job_betaGraphs - TEST 5 - current_test = " + string(current_test));
    // == cacheSizeBandwidth ==
    // Compare read speeds with USM host, device and shared
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output_short + "TR05_memLocStrat7_flattenAll" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_16,
    /*display_name*/  computer_name + " - ACTS: type mémoire & stratégie - flat - run " + string(current_run)
    );
    batch_add_file( // gr.ptr.
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    "v05_TEMP_tracccMemLocStrat7_sansGraphPtr" + fname_suffix_input_traccc,
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

// ===================================================
    
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
    
case 17:
    // == LM ==
    var graph = batch_add_graph(
    /*output_path*/   common_path_out,
    /*output_fname*/  fname_prefix_output_short + "LMoptim" + fname_suffix_output,
    /*use_script*/    draw_some_graph_11,
    /*display_name*/  computer_name + " - LM optim - " + size_str + " - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    fname_prefix_input + "LMoptim" + fname_suffix_input,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = false;
    g_xgroup_has_own_scale = false;
    //g_traccc_draw_graph_ptr = false;
    //g_traccc_draw_flatten = true;
    //g_traccc_ptrVsFlat_memLocation = mem_location; // -1 pour afficher tout
    //g_traccc_ignore_allocation_time = false;
    load_draw_save_graph(graph);
    ++g_citer;
    break;
    
case 18:
    // == DMA ==
    var graph = batch_add_graph(
    /*output_path*/   common_path_out,
    /*output_fname*/  fname_prefix_output_short + "dma" + fname_suffix_output,
    /*use_script*/    draw_some_graph_13dbg,
    /*display_name*/  computer_name + " - DMA - " + size_str + " - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    fname_prefix_input + "dma" + fname_suffix_input,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = true;
    DISPLAY_TEMP_COPY_BUFF_TIMES = false;
    load_draw_save_graph(graph);
    ++g_citer;
    break;

case 19:
    // == Alloc (glibc vs sycl) ==
    var graph = batch_add_graph(
    /*output_path*/   common_path_out,
    /*output_fname*/  fname_prefix_output_short + "alloc" + fname_suffix_output,
    /*use_script*/   draw_some_graph_8,
    /*display_name*/ computer_name + " - alloc glibc vs sycl - " + size_str + " - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    fname_prefix_input + "alloc" + fname_suffix_input,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = true;
    load_draw_save_graph(graph);
    ++g_citer;
    break;
    
default :
    show_message("ERROR @ run_batch_job_betaGraphs : current_test(" + string(current_test) + ") not handled.");
    break;
}

return 0;
