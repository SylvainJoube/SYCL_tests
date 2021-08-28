/// run_batch_msi_1(step);

var irun = argument0;

var run_number = 4;

if ( irun > run_number) return -1;

// Dessin des graphes et sauvegardes en png, pour plusieurs graphiques.
// Comparaison (à l'oeil) entre les différents runs

// var common_path = "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\";
var common_path = "C:\Users\sylvain\Desktop\plot_thinkpad_512MiB\";
var computer_name = "thinkpad";
var size_str = "512MiB";
//var common_file_name = "_" + computer_name + "_" + size_str + "_O2";


var bench_version = "v02";
var bench_test_nb = "007";
var debug_run_prefix = "";
var debug_verid = "f";

//var citer = 1; // iteration count

//for (var irun = 1; irun <= run_number; ++irun) {
    
    var fname_prefix_output = "b" + bench_test_nb + "_" + bench_version + "_";
    var fname_prefix_input  = bench_version + "_";
    
    var fname_suffix_common = "_" + computer_name + "_" + size_str + "_O2_RUN" + string(irun);
    
    var fname_suffix_output = fname_suffix_common + ".png";
    var fname_suffix_input  = fname_suffix_common + ".t";
    
    //var file_name_const_part = common_file_name + "_RUN" + string(irun);// + ".t";
    //var local_common_path = common_path + bench_version;
    //var file_name_const_part_ouptut_png = file_name_const_part + "_" + bench_test_nb + ".png";
    
    // == LM ==
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "LM" + fname_suffix_output,
    /*use_script*/    draw_some_graph_11,
    /*display_name*/  "thinkpad - LM - 512MiB - run " + string(irun)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    fname_prefix_input + "LM" + fname_suffix_input,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = false;
    load_draw_save_graph(graph);
    ++g_citer;
    
    
    // == DMA ==
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "dma" + fname_suffix_output,
    /*use_script*/    draw_some_graph_13dbg,
    /*display_name*/  "thinkpad - DMA - 512MiB - run " + string(irun)
    );
    batch_add_file(
    /*graph*/       graph,
    /*in_path*/     common_path,
    /*in_fname*/    fname_prefix_input + "dma" + fname_suffix_input,
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = true;
    load_draw_save_graph(graph);
    ++g_citer;
    
    
    // == SIMD ==
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "simd" + fname_suffix_output,
    /*use_script*/    draw_some_graph_9,
    /*display_name*/  "thinkpad - SIMD - 512MiB - run " + string(irun)
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
    
    
    // == Alloc (glibc vs sycl) ==
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "alloc" + fname_suffix_output,
    /*use_script*/   draw_some_graph_8,
    /*display_name*/ "thinkpad - alloc glibc vs sycl - 512MiB - run " + string(irun)
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
    
//}

return 0;
