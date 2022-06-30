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

//show_message("current_run(" + string(current_run) + ") current_test(" + string(current_test) + ")");

++current_run;  // 1..4
++current_test; // 1..5

// H:\SYNCTHING\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench
common_path = "H:\SYNCTHING\data_sync\academique\These\SYCL_tests\mem_bench\output_bench\";


// v08_alloc_thinkpad_ST_512MiB_O2_RUN2

size_str = "6GiB";//"512MiB";
//size_str = "512MiB";
var cccomputer_name = "sandor_ST";
//var cccomputer_name = "thinkpad_ST";


bench_version = "v08";
bench_test_nb = "200"; // 007
debug_run_prefix = "";
debug_verid = "g";

g_graph_height = 360;
g_graph_width = 640;

//g_graph_height = 1000;
//g_graph_width = 1400;

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
    g_ymax_impose = 2000;
    for (var ii = 0; ii < 4; ++ii) {
        g_display_device = false;
        g_display_host = false;
        g_display_shared = false;
        g_display_accessors = false;
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
        if (ii == 3) {
            g_display_device = true;
            g_display_host = true;
            g_display_shared = true;
            g_display_accessors = true;
            suff = "04";
        }
        //current_run = 1;
        computer_name = cccomputer_name;
        simple_refresh_output_name();    // == Alloc (glibc vs sycl) ==
        var graph = batch_add_graph(
        /*output_path*/   common_path,
        /*output_fname*/  fname_prefix_output + "alloc-" + suff + fname_suffix_output,
        /*use_script*/   draw_some_graph_8_compUSM_david,
        /*display_name*/ "USM device, shared, host, accessors" //computer_name + " - alloc glibc vs sycl - " + size_str + " - run " + string(current_run)
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
    g_ymax_impose = 2000;
    for (var ii = 0; ii < 2; ++ii) {
        g_display_device = false;
        g_display_host = false;
        g_display_shared = false;
        g_display_accessors = false;
        var suff = "";
        
        if (ii == 0) {
            g_display_host = true;
            suff = "05";
        }
        if (ii == 1) {
            g_display_host = true;
            g_display_shared = true;
            suff = "06";
        }
        
        //current_run = 1;
        computer_name = cccomputer_name;
        simple_refresh_output_name();    // == Alloc (glibc vs sycl) ==
        var graph = batch_add_graph(
        /*output_path*/   common_path,
        /*output_fname*/  fname_prefix_output + "alloc-" + suff + fname_suffix_output,
        /*use_script*/   draw_some_graph_8_compUSMAccDirect_david,
        /*display_name*/ "USM device, shared, host, acc. + direct access to SYCL mem" //computer_name + " - alloc glibc vs sycl - " + size_str + " - run " + string(current_run)
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


default :
    show_message("ERROR @ run_batch_job_rapport_simple : current_test(" + string(current_test) + ") not handled.");
    break;
}

return 0;
