/// run_batch_job_rapport_traccc(step, computer_name);

/*
For unfinished beta data only.
Does not save to file, only pints graphically.
*/

var step = argument0; // must start at 0
//computer_name = argument1; // e.g. "msiNvidia_ST" or "thinkpad"

// daclared in create var total_iterations = tests_per_run * run_number;

if ( step >= traccc_total_iterations) return -1;

current_run = 1; //step + 1;

traccc_hide_host = false;

// ON MSI var common_path = "C:\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\";
// ON ordi fixe blanc
common_path = "H:\SYNCTHING\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\";
output_path = common_path + "presentation\";// + "traccc\";

// traccc_refresh_output_name(); à faire à chaque test

var current_test = step + 1; //5 + step;

g_surface_height = g_graph_height + g_graph_yoffset + 70;

/*
//current_computer_name = "thinkpad_AT";//"msiNvidia_ST";
//current_computer_name = "msiNvidia_AT";
//current_computer_name = "thinkpad_AT";
//current_computer_name = "sandor_AT";
*/



g_line_pts_link_width = 2;


switch (current_test) {
// TRACCC land





// ===== Comparaison de chaque type USM : graphe ptr vs flatten =====
case 1: // A_acts06_ptrVsFlat_mem20_sandor_AT_ld10_RUN1_q1
    
    computer_name = "sandor_AT";
    traccc_repeat_load_count_base = 10;
    g_graph_yoffset = 90;
    g_graph_height = 190;
    g_graph_width = 640;
    g_yorig = g_graph_height + g_graph_yoffset;
    var new_surf_height = g_graph_height + g_graph_yoffset + 20; // 70
    g_surface_height = new_surf_height;
    
    for (var im = 0; im < 3; ++im)
    for (var stp = 0; stp < 2; ++stp) {
        var mem_location;
        switch (im) {
        case 0: mem_location = 0; g_ymax_impose = 9700000;  break; // shared
        case 1: mem_location = 2; g_ymax_impose = 9700000; break; // host
        case 2: mem_location = 20; g_ymax_impose = 210000; break; // glibc
        default: break;
        }
        g_ptrVsFlat_firstStep = (stp == 0);
        
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
        /*output_fname*/  fname_prefix_output_short + "ptrVsFlat_mem" + string(mem_location) + "-" + string(stp) + fname_suffix_output_traccc,
        /*use_script*/    draw_some_graph_traccc_17_prez, 
        /*display_name*/  "SparseCCL - Sandor - mémoire " + mem_location_to_str(mem_location)
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

// ===== Comparaison des USM en flatten =====
// -> ça serait bien de l'avoir aussi en without host
case 2:
    // device + shared + host
    computer_name = "sandor_AT";
    traccc_repeat_load_count_base = 10;
    g_graph_yoffset = 90;
    g_graph_height = 190;
    g_graph_width = 640;
    var new_surf_height = g_graph_height + g_graph_yoffset + 20; // 70
    g_surface_height = new_surf_height;
    
    for (var im = 0; im < 5; ++im) {
        g_display_shared = false;
        g_display_host = false;
        var mem_location;
        g_traccc_ignore_allocation_time = false;
        g_draw_shared_before_shared = false;
        switch (im) {
        case 0: mem_location = 0; g_ymax_impose = 100000;  break; // device
        case 1: mem_location = 1; g_display_shared = true; g_ymax_impose = 100000; break; // shared
        case 2: mem_location = 2; g_display_shared = true; g_display_host = true; g_ymax_impose = -1; break; // host
        case 3:
            g_display_device = false;
            g_display_shared = true;
            g_display_host = false;
            g_ymax_impose = 93000;
            g_draw_shared_before_shared = true;
            g_traccc_ignore_allocation_time = true;
            break;
        case 4:
            g_display_shared = true;
            g_display_device = true;
            g_display_host = false;
            g_ymax_impose = 93000;
            g_draw_shared_before_shared = true;
            g_traccc_ignore_allocation_time = true;
            break;
        default: break;
        }
        g_multiple_xaxis = true;
        g_xgroup_has_own_scale = false;
        g_traccc_draw_graph_ptr = false;
        g_traccc_draw_flatten = true;
        traccc_repeat_load_count = traccc_repeat_load_count_base;
        
        traccc_refresh_output_name();
        var graph = batch_add_graph(
        /*output_path*/   output_path,
        /*output_fname*/  fname_prefix_output_short + "generalFlatten_withAlloc-" + string(im) + fname_suffix_output_traccc,
        /*use_script*/    draw_some_graph_traccc_16_prez,
        /*display_name*/  "SparseCCL - Sandor - structures applaties"
        );
        batch_add_file(
        /*graph*/       graph,
        /*in_path*/     common_path,
        /*in_fname*/    bvt + "generalFlatten" + fname_suffix_input_traccc,
        /*curve_name*/  "aucun nom", // nom de la courbe associée
        /*computer_id*/ 1 // 1 Thinkpad, 2 MSI Intel, 3 MSI Nvidia, 4 Sandor
        );
        load_draw_save_graph(graph);
        ++g_citer;
    }
    break;
    
// Idem mais sans l'host
// pas utilisé
case 3:
    computer_name = "sandor_AT";
    traccc_repeat_load_count_base = 10;
    g_graph_yoffset = 90;
    g_graph_height = 200;
    g_graph_width = 640;
    g_surface_height = new_surf_height;
    
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
    /*display_name*/  "ACTS : Sandor - applati, sans hôte"
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

    
default :
    show_message("ERROR @ run_batch_job_rapport_traccc : current_test(" + string(current_test) + ") not handled.");
    break;
}


return 0;
