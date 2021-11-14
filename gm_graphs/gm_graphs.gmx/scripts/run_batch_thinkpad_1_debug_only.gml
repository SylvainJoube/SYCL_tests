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
var common_file_name = "_" + computer_name + "_" + size_str + "_O2";


var bench_version = "v02";
var bench_test_nb = "006";
var debug_run_prefix = "";
var debug_verid = "f";

//var citer = 1; // iteration count

//for (var irun = 1; irun <= run_number; ++irun) {

    var file_name_const_part = common_file_name + "_RUN" + string(irun);// + ".t";
    var local_common_path = common_path + bench_version;
    var file_name_const_part_ouptut_png = file_name_const_part + "_" + bench_test_nb + ".png";
    var local_common_path_debug_output;
    
    /*debug*/ debug_run_prefix = debug_verid + fill_front(string(g_citer), 3, "0") + "_";
    /*debug*/ local_common_path_debug_output = common_path + debug_run_prefix + "DEBUG" + bench_version;
    
    // == LM ==
    var graph = batch_add_graph_v1(
    /*output_path*/  local_common_path_debug_output + "_LM" + file_name_const_part_ouptut_png,
    /*use_script*/   draw_some_graph_11,
    /*display_name*/ "thinkpad - LM - 512MiB - run " + string(irun)
    );
    batch_add_file_v1(
    /*graph*/       graph,
    /*path*/        local_common_path + "_LM" + file_name_const_part + ".t",
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = false;
    load_draw_save_graph(graph);
    ++g_citer;
    
    /*debug*/ debug_run_prefix = debug_verid + fill_front(string(g_citer), 3, "0") + "_";
    /*debug*/ local_common_path_debug_output = common_path + debug_run_prefix + "DEBUG" + bench_version;
    
    // == DMA ==
    var graph = batch_add_graph_v1(
    /*output_path*/  local_common_path_debug_output + "_dma" + file_name_const_part_ouptut_png,
    /*use_script*/   draw_some_graph_13dbg,
    /*display_name*/ "thinkpad - DMA - 512MiB - run " + string(irun)
    );
    batch_add_file_v1(
    /*graph*/       graph,
    /*path*/        local_common_path + "_dma" + file_name_const_part + ".t",
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = true;
    load_draw_save_graph(graph);
    ++g_citer;
    
    /*debug*/ debug_run_prefix = debug_verid + fill_front(string(g_citer), 3, "0") + "_";
    /*debug*/ local_common_path_debug_output = common_path + debug_run_prefix + "DEBUG" + bench_version;
    
    // == SIMD ==
    var graph = batch_add_graph_v1(
    /*output_path*/  local_common_path_debug_output + "_simd" + file_name_const_part_ouptut_png,
    /*use_script*/   draw_some_graph_9,
    /*display_name*/ "thinkpad - SIMD - 512MiB - run " + string(irun)
    );
    batch_add_file_v1(
    /*graph*/       graph,
    /*path*/        local_common_path + "_simd" + file_name_const_part + ".t",
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = true;
    load_draw_save_graph(graph);
    ++g_citer;
    
    /*debug*/ debug_run_prefix = debug_verid + fill_front(string(g_citer), 3, "0") + "_";
    /*debug*/ local_common_path_debug_output = common_path + debug_run_prefix + "DEBUG" + bench_version;
    
    // == Alloc (glibc vs sycl) ==
    var graph = batch_add_graph_v1(
    /*output_path*/  local_common_path_debug_output + "_alloc" + file_name_const_part_ouptut_png,
    /*use_script*/   draw_some_graph_8,
    /*display_name*/ "thinkpad - alloc glibc vs sycl - 512MiB - run " + string(irun)
    );
    batch_add_file_v1(
    /*graph*/       graph,
    /*path*/        local_common_path + "_alloc" + file_name_const_part + ".t",
    /*curve_name*/  "aucun nom", // nom de la courbe associée
    /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
    );
    g_multiple_xaxis = true;
    load_draw_save_graph(graph);
    ++g_citer;
    
//}

return 0;
