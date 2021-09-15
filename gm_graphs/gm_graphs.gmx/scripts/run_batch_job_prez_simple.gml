/// run_batch_job(step, computer_name);

var step = argument0; // must start at 0
//computer_name = argument1; // e.g. "msiNvidia_ST" or "thinkpad"

// Draw one single graph at a time, to avoir overloading the draw event
// of game maker (that, for some reason, does not like to draw toomany things at once...)
//var tests_per_run = 4; defines globally in Create event
//var run_number = 4;

var total_iterations = tests_per_run * run_number;

if ( step >= total_iterations) return -1;


current_run = floor(step / tests_per_run); // 0..3
current_test = step - current_run * tests_per_run; // 0..3

++current_run;  // 1..4
++current_test; // 1..5

common_path = "H:\SYNCTHING\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\";


size_str = "512MiB";

bench_version = "v05";
bench_test_nb = "200"; // 007
debug_run_prefix = "";
debug_verid = "g";

//g_graph_height = 140;
g_graph_height = 360;
g_graph_width = 640;

g_yorig = g_graph_height + g_graph_yoffset;
g_surface_width = g_graph_width + g_xorig + 100;
g_surface_height = g_graph_height + g_graph_yoffset + 20;

//computer_name = "thinkpad_AT";//"msiNvidia_ST";
//computer_name = "msiNvidia_AT";
//computer_name = "thinkpad_AT";
//computer_name = "sandor_AT";

g_line_pts_link_width = 2;

switch (current_test) {

//case 1: // b010_v05_LMclassic_msiNvidia_AT_512MiB_O2    
//case 2: // b010_v05_LMclassicBandwidth_msiNvidia_AT_512MiB_O2_RUN1fact1-5
//case 3: // b010_v05_LMclassicBandwidth_sandor_AT_512MiB_O2_RUN2fact1-5
//case 4: // b010_v05_LMoptimBandwidth_msiNvidia_AT_512MiB_O2_RUN1fact1-5


case 1: // b010_v05_alloc_msiNvidia_AT_512MiB_O2_RUN2fact1-5
    g_ymax_impose = 200000;
    for (var ii = 0; ii < 3; ++ii) {
        g_display_device = false;
        g_display_host = false;
        g_display_shared = false;
        var suff = "";
        
        if (ii == 0) {
            g_display_device = true;
            suff = "01";
        }
        if (ii == 1) {
            g_display_device = true;
            g_display_shared = true;
            suff = "02";
        }
        if (ii == 2) {
            g_display_device = true;
            g_display_host = true;
            g_display_shared = true;
            suff = "03";
        }
        current_run = 2;
        computer_name = "msiNvidia_AT";
        simple_refresh_output_name();    // == Alloc (glibc vs sycl) ==
        var graph = batch_add_graph(
        /*output_path*/   common_path,
        /*output_fname*/  fname_prefix_output + "alloc-" + suff + fname_suffix_output,
        /*use_script*/   draw_some_graph_8_compUSM,
        /*display_name*/ "MSI - USM device, shared, host" //computer_name + " - alloc glibc vs sycl - " + size_str + " - run " + string(current_run)
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
    }
    break;

case 2: // b010_v05_alloc_msiNvidia_AT_512MiB_O2_RUN2fact1-5
    g_ymax_impose = 200000;
    for (var ii = 0; ii < 2; ++ii) {
        g_display_device = false;
        g_display_host = false;
        g_display_shared = false;
        var suff = "";
        
        if (ii == 0) {
            g_display_host = true;
            suff = "04";
        }
        if (ii == 1) {
            g_display_host = true;
            g_display_shared = true;
            suff = "05";
        }
        
        current_run = 2;
        computer_name = "msiNvidia_AT";
        simple_refresh_output_name();    // == Alloc (glibc vs sycl) ==
        var graph = batch_add_graph(
        /*output_path*/   common_path,
        /*output_fname*/  fname_prefix_output + "alloc-" + suff + fname_suffix_output,
        /*use_script*/   draw_some_graph_8_compUSMAccDirect,
        /*display_name*/ "MSI - USM device, shared, host + accès direct" //computer_name + " - alloc glibc vs sycl - " + size_str + " - run " + string(current_run)
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
    }
    break;

    
    

case 6: // b010_v05_alloc_sandor_AT_512MiB_O2_RUN2fact1-5
    current_run = 2;
    computer_name = "sandor_AT";
    simple_refresh_output_name();    // == Alloc (glibc vs sycl) ==
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "alloc" + fname_suffix_output,
    /*use_script*/   draw_some_graph_8,
    /*display_name*/ "Sandor - copie SYCL vs glibc" //computer_name + " - alloc glibc vs sycl - " + size_str + " - run " + string(current_run)
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

    
    
case 7: // b010_v05_dma_msiNvidia_AT_512MiB_O2_RUN2fact1-5
    current_run = 2;
    computer_name = "msiNvidia_AT";
    simple_refresh_output_name();    // == DMA ==
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "dma" + fname_suffix_output,
    /*use_script*/    draw_some_graph_13dbg,
    /*display_name*/  "MSI - buffer SYCL vs glibc"//computer_name + " - DMA - " + size_str + " - run " + string(current_run)
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
    
    

case 8: // b010_v05_sumReadSpeedBandwidth_sandor_AT_512MiB_O2_RUN2fact1-5
    current_run = 2;
    computer_name = "sandor_AT";
    simple_refresh_output_name();    // == sumReadSpeed ==
    // Compare read speeds with USM host, device and shared
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "sumReadSpeedBandwidth" + fname_suffix_output,
    /*use_script*/    draw_some_graph_14throughputBandwidth,
    /*display_name*/  "Sandor - bandes passantes observées" //computer_name + " - L fixé, repeat variable - " + size_str + " - run " + string(current_run)
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

case 9:
    current_run = 2;
    computer_name = "sandor_AT";
    simple_refresh_output_name();    // == sumReadSpeed ==
    // == cacheSizeBandwidth ==
    // Compare read speeds with USM host, device and shared
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "cacheSizeBandwidth" + fname_suffix_output,
    /*use_script*/    draw_some_graph_15cacheSizeBandwidth,
    /*display_name*/  "Sandor - effets de caches"
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


case 10:
    current_run = 1;
    computer_name = "msiNvidia_ST";
    bench_version = "v06C";
    simple_refresh_output_name();    // == sumReadSpeed ==
    bench_version = "v05";
    // == cacheSizeBandwidth ==
    // Compare read speeds with USM host, device and shared
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "cacheSizeBandwidth" + fname_suffix_output,
    /*use_script*/    draw_some_graph_15cacheSizeBandwidth,
    /*display_name*/  "MSI - effets de caches"
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
    
default :
    show_message("ERROR @ run_batch_job_rapport_simple : current_test(" + string(current_test) + ") not handled.");
    break;
}

return 0;
