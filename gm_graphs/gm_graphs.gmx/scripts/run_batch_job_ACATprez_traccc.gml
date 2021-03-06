/// run_batch_job_ACATprez_traccc(step, computer_name);

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
//common_path = "H:\SYNCTHING\data_sync\academique\M2\StageM2\SYCL_tests\mem_bench\output_bench\";
common_path = "H:\SYNCTHING\data_sync\academique\These\SYCL_tests\mem_bench\output_bench\";

// common_path -- output_path = common_path + "presentation_ACTS\";// + "traccc\";

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

bench_version = "v08";
bench_test_nb = "200";


switch (current_test) {
// TRACCC land





// ===== Comparaison de chaque type USM : graphe ptr vs flatten =====
case 1: // A_acts06_ptrVsFlat_mem20_sandor_AT_ld10_RUN1_q1
    
    computer_name = "sandor_AT"; // thinkpad_AT
    traccc_repeat_load_count_base = 1;
    g_graph_yoffset = 90;
    g_graph_height = 190;//190;
    g_graph_width = 400;//640;
    //g_yorig = g_graph_height + g_graph_yoffset;
    //var new_surf_height = g_graph_height + g_graph_yoffset + 20; // 70
    //g_surface_height = new_surf_height;
    //g_surface_width = g_graph_width + g_xorig + 100;
    g_xorig = 88;
    refresh_dimensions();
    
    
    g_ymax_impose = -1;
    
    g_multiple_xaxis = true;
    g_xgroup_has_own_scale = false;
    g_traccc_draw_graph_ptr = true;
    g_traccc_draw_flatten = true;
    // g_traccc_ptrVsFlat_memLocation = mem_location;
    //g_traccc_ignore_allocation_time = false;
    traccc_repeat_load_count = traccc_repeat_load_count_base;
    
    traccc_refresh_output_name();
    g_ymax_impose = 900;
        
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output_short + "_ACAT_ptrVsFlat_mem" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_17_ACATprez, 
    /*display_name*/  "SparseCCL - " + "pointer graph vs flat arrays" //mem_location_to_str(mem_location) + " memory"
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
    g_ymax_impose = -1;
    

    
    /*for (var im = 0; im < 3; ++im)
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
        
    }*/
    break;

case 2: // A_acts06_ptrVsFlat_mem20_sandor_AT_ld10_RUN1_q1
    
    computer_name = "sandor_AT"; // thinkpad_AT 
    traccc_repeat_load_count_base = 100;
    g_graph_yoffset = 90;
    g_graph_height = 190;//190;
    g_graph_width = 400;//640;
    g_acat_temp_host_is_disabled = true;
    //g_yorig = g_graph_height + g_graph_yoffset;
    //var new_surf_height = g_graph_height + g_graph_yoffset + 20; // 70
    //g_surface_height = new_surf_height;
    //g_surface_width = g_graph_width + g_xorig + 100;
    g_xorig = 88;
    refresh_dimensions();
    
    g_ymax_impose = -1;
    
    g_multiple_xaxis = true;
    g_xgroup_has_own_scale = false;
    g_traccc_draw_graph_ptr = true;
    g_traccc_draw_flatten = true;
    // g_traccc_ptrVsFlat_memLocation = mem_location;
    //g_traccc_ignore_allocation_time = false;
    traccc_repeat_load_count = traccc_repeat_load_count_base;
    
    traccc_refresh_output_name();
    g_ymax_impose = -1;
        
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output_short + "_ACAT_flatMem" + fname_suffix_output_traccc,
    /*use_script*/    draw_some_graph_traccc_17_ACATprez_flat, 
    /*display_name*/  "SparseCCL - flat arrays" //mem_location_to_str(mem_location) + " memory"
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
    g_ymax_impose = -1;
    
    break;
    
// SIMPLE (not traccc)
case 3: // b010_v05_alloc_msiNvidia_AT_512MiB_O2_RUN2fact1-5
    
    computer_name = "sandor_ST";
    size_str = "6GiB";//"512MiB";
    
    g_ymax_impose = -1;
    g_display_device = false;
    g_display_host = false;
    g_display_shared = false;
    g_display_accessors = false;
    current_run = 2;
    var suff = "";
    
    g_display_host = true;
    g_display_shared = true;
    suff = "07";
    
    g_graph_yoffset = 90;
    g_graph_height = 190;//190;
    g_graph_width = 840;//640;
    g_xorig = 118;
    refresh_dimensions();
    g_label_xoffset = 30;
    
    //current_run = 1;
    //computer_name = cccomputer_name;
    simple_refresh_output_name();    // == Alloc (glibc vs sycl) ==
    var graph = batch_add_graph(
    /*output_path*/   common_path,
    /*output_fname*/  fname_prefix_output + "alloc-" + suff + fname_suffix_output,
    /*use_script*/   draw_some_graph_8_compUSMAccDirect_ACAT,
    /*display_name*/ "USM device, shared, host, accessors + direct access to SYCL mem" //computer_name + " - alloc glibc vs sycl - " + size_str + " - run " + string(current_run)
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

    
    
    
// ===== Comparaison des USM en flatten =====
// -> ça serait bien de l'avoir aussi en without host
case 20: return 0;
    // device + shared + host
    computer_name = "sandor_AT";
    traccc_repeat_load_count_base = 10;
    g_graph_yoffset = 90;
    g_graph_height = 190;
    g_graph_width = 640;
    var new_surf_height = g_graph_height + g_graph_yoffset + 20; // 70
    g_surface_height = new_surf_height;
    g_surface_width = g_graph_width + g_xorig + 100;
    
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
        /*output_path*/   common_path,
        /*output_fname*/  fname_prefix_output_short + "generalFlatten_withAlloc-" + string(im) + fname_suffix_output_traccc,
        /*use_script*/    draw_some_graph_traccc_16_ACTSprez,
        /*display_name*/  "SparseCCL - GPU friendly arrays"
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
    

    
default :
    show_message("ERROR @ run_batch_job_ACTSprez_traccc : current_test(" + string(current_test) + ") not handled.");
    break;
}


return 0;
