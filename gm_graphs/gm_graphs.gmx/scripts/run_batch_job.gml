/// run_batch_job(step, computer_name);

var step = argument0; // must start at 0
var computer_name = argument1; // e.g. "msiNvidia_ST" or "thinkpad"

// Draw one single graph at a time, to avoir overloading the draw event
// of game maker (that, for some reason, does not like to draw toomany things at once...)
//var tests_per_run = 4; defines globally in Create event
//var run_number = 4;

var total_iterations = tests_per_run * run_number;

if ( step >= total_iterations) return -1;


var current_run = floor(step / tests_per_run); // 0..3
var current_test = step - current_run * tests_per_run; // 0..3

++current_run;  // 1..4
++current_test; // 1..5

//error_add("Draw test(" + string(current_test) + ") - run(" + string(current_run) + ") for " + computer_name);
//return 0;

// Dessin des graphes et sauvegardes en png, pour plusieurs graphiques.
// Comparaison (à l'oeil) entre les différents runs

// ON MSI var common_path = "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\";
// ON ordi fixe blanc
common_path = "H:\SYNCTHING\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\";


//var common_path = "C:\Users\sylvain\Desktop\plot_thinkpad_512MiB\";
//var computer_name = "thinkpad";
//var size_str = "1GiB";
var size_str = "512MiB";
//var common_file_name = "_" + computer_name + "_" + size_str + "_O2";


var bench_version = "v05";
var bench_test_nb = "010"; // 007
var debug_run_prefix = "";
var debug_verid = "g";

//var citer = 1; // iteration count

//for (var current_run = 1; current_run <= run_number; ++current_run) {
    
var fname_prefix_output = "b" + bench_test_nb + "_" + bench_version + "_";
var fname_prefix_input  = bench_version + "_";

var fname_suffix_common = "_" + computer_name + "_" + size_str + "_O2_RUN" + string(current_run);

var fname_suffix_output = fname_suffix_common + "fact1-5" + ".png";
var fname_suffix_input  = fname_suffix_common + ".t";

//var file_name_const_part = common_file_name + "_RUN" + string(current_run);// + ".t";
//var local_common_path = common_path + bench_version;
//var file_name_const_part_ouptut_png = file_name_const_part + "_" + bench_test_nb + ".png";

switch (current_test) {

case 1:
    // == LM ==
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "LMoptim" + fname_suffix_output,
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
    load_draw_save_graph(graph);
    ++g_citer;
    break;

case 2:
    // == DMA ==
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "dma" + fname_suffix_output,
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
    
case 3:
    // == SIMD ==
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "simd" + fname_suffix_output,
    /*use_script*/    draw_some_graph_9,
    /*display_name*/  computer_name + " - SIMD - " + size_str + " - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    fname_prefix_input + "simd" + fname_suffix_input,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = true;
    load_draw_save_graph(graph);
    ++g_citer;
    break;
    
case 4:
    // == Alloc (glibc vs sycl) ==
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "alloc" + fname_suffix_output,
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

case 5:
    // == LM classic ==
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "LMclassic" + fname_suffix_output,
    /*use_script*/    draw_some_graph_11,
    /*display_name*/  computer_name + " - LM classic - " + size_str + " - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    fname_prefix_input + "LMclassic" + fname_suffix_input,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = false;
    load_draw_save_graph(graph);
    ++g_citer;
    break;
    
// + supperposition de L et M en classique vs optimisation mémoire
case 6: // ...
    // == LM classic vs optimization ==
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "LMclassicVSoptim" + fname_suffix_output,
    /*use_script*/    draw_some_graph_11,
    /*display_name*/  computer_name + " - LM classic - " + size_str + " - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    fname_prefix_input + "LMclassic" + fname_suffix_input,
    /*curve_name*/  "boucle for classique", // nom de la courbe associée
    /*computer_id*/ 3 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    fname_prefix_input + "LMoptim" + fname_suffix_input,
    /*curve_name*/  "boucle for optimisée", // nom de la courbe associée
    /*computer_id*/ 3 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = false;
    load_draw_save_graph(graph);
    ++g_citer;
    break;

// Courbe du débit en fonction des valeurs de L et M - cas non optimisé
case 7:
    // == LM classic ==
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "LMclassicBandwidth" + fname_suffix_output,
    /*use_script*/    draw_some_graph_11bandwidth,
    /*display_name*/  computer_name + " - LM classique, débit - " + size_str + " - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    fname_prefix_input + "LMclassic" + fname_suffix_input,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 3 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = false;
    load_draw_save_graph(graph);
    ++g_citer;
    break;

// Courbe du débit en fonction des valeurs de L et M - cas optimisé
case 8:
    // == LM optimized ==
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "LMoptimBandwidth" + fname_suffix_output,
    /*use_script*/    draw_some_graph_11bandwidth,
    /*display_name*/  computer_name + " - LM optim, débit - " + size_str + " - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    fname_prefix_input + "LMoptim" + fname_suffix_input,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 3 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = false;
    load_draw_save_graph(graph);
    ++g_citer;
    break;


case 9:
    // == sumReadSpeed ==
    // Compare read speeds with USM host, device and shared
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "sumReadSpeed" + fname_suffix_output,
    /*use_script*/    draw_some_graph_14throughput,
    /*display_name*/  computer_name + " - L fixé, repeat variable - " + size_str + " - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    fname_prefix_input + "sumReadSpeed" + fname_suffix_input,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = false;
    load_draw_save_graph(graph);
    ++g_citer;
    break;

case 10:
    // == sumReadSpeed ==
    // Compare read speeds with USM host, device and shared
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "sumReadSpeedBandwidth" + fname_suffix_output,
    /*use_script*/    draw_some_graph_14throughputBandwidth,
    /*display_name*/  computer_name + " - L fixé, repeat variable - " + size_str + " - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    fname_prefix_input + "sumReadSpeed" + fname_suffix_input,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = false;
    load_draw_save_graph(graph);
    ++g_citer;
    break;

case 11:
    // == cacheSize ==
    // Compare read speeds with USM host, device and shared
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "cacheSize" + fname_suffix_output,
    /*use_script*/    draw_some_graph_15cacheSize,
    /*display_name*/  computer_name + " - L variable, repeat fixé - " + size_str + " - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    fname_prefix_input + "cacheSize" + fname_suffix_input,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = false;
    load_draw_save_graph(graph);
    ++g_citer;
    break;

case 12:
    // == cacheSizeBandwidth ==
    // Compare read speeds with USM host, device and shared
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "cacheSizeBandwidth" + fname_suffix_output,
    /*use_script*/    draw_some_graph_15cacheSizeBandwidth,
    /*display_name*/  computer_name + " - L variable, repeat fixé - " + size_str + " - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    fname_prefix_input + "cacheSize" + fname_suffix_input,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = false;
    load_draw_save_graph(graph);
    ++g_citer;
    break;

case 13:
    // == DMA full infos ==
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "dmaFull" + fname_suffix_output,
    /*use_script*/    draw_some_graph_13dbg,
    /*display_name*/  computer_name + " - DMA (full) - " + size_str + " - run " + string(current_run)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    fname_prefix_input + "dma" + fname_suffix_input,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = true;
    DISPLAY_TEMP_COPY_BUFF_TIMES = true; // aussi pour avoir le temps copie host -> host
    load_draw_save_graph(graph);
    ++g_citer;
    break;
    
default :
    show_message("ERROR @ run_batch_all : current_test(" + string(current_test) + ") not in [1..4]");
    break;
}
    
//}

return 0;
