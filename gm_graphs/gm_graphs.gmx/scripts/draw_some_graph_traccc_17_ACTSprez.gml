
/*

version traccc_17 : comparaison des différents temps en fonction de la locatlisation de la mémoire.

Graphe de pointeurs vs flatten : glibc puis SYCL

*/

var echelle_log = false;

// deprecated g_graph_title = "Allocation glibc vs SYCL - SANDOR - 6 GiB - O2";

/*if (echelle_log) g_graph_title += "(échelle log2)";
else             g_graph_title += "(échelle linéaire)";*/

var gp;
var graph_list = ds_list_create();
var colors = ds_list_create();

var merge_cfactor = 0.3;

var draw_graph_ptr = g_traccc_draw_graph_ptr;
var draw_flatten = g_traccc_draw_flatten;

//g_traccc_ptrVsFlat_memLocation // j.MEMORY_LOCATION

var merge_fact = 0.3;

// Shared
if (g_traccc_ptrVsFlat_memLocation == 0) {
    ds_list_add(colors, merge_colour(c_blue, c_black, merge_fact)); // shared
    ds_list_add(colors, merge_colour(c_blue, c_black, 0)); // shared
}
// Host
if (g_traccc_ptrVsFlat_memLocation == 2) {
    ds_list_add(colors, merge_colour(c_red, c_black, merge_fact));  // host
    ds_list_add(colors, merge_colour(c_red, c_black, 0));  // host
}
// CPU
if (g_traccc_ptrVsFlat_memLocation == 20) {
    ds_list_add(colors, merge_colour(c_maroon, c_black, 0.9)); // GPU
    ds_list_add(colors, merge_colour(c_maroon, c_black, 0)); // GPU
}

//ds_list_add(colors, merge_colour(c_yellow, c_black, 0)); // graphe de pointeurs
//ds_list_add(colors, merge_colour(c_yellow, c_black, 0));  // flatten

// aplati : 0 shared 1 cpu 2 host 3 device ;   graphe ptr : 4 shared 5 cpu 6 host
var do_job_index = ds_list_create();
//ds_list_add(do_job_index, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
ds_list_add(do_job_index, 0, 1, 2, 3, 4, 5, 6);

// shared
if (g_traccc_ptrVsFlat_memLocation == 0) {
    if (g_ptrVsFlat_firstStep) ds_list_replace(do_job_index, 0, -1); // shared
    ds_list_replace(do_job_index, 1, -1); // cpu
    ds_list_replace(do_job_index, 2, -1); // host
    ds_list_replace(do_job_index, 3, -1); // device
    //ds_list_replace(do_job_index, 4, -1); // shared ptr
    ds_list_replace(do_job_index, 5, -1); // cpu ptr
    ds_list_replace(do_job_index, 6, -1); // host ptr
}

// host
if (g_traccc_ptrVsFlat_memLocation == 2) {
    ds_list_replace(do_job_index, 0, -1); // shared
    ds_list_replace(do_job_index, 1, -1); // cpu
    if (g_ptrVsFlat_firstStep) ds_list_replace(do_job_index, 2, -1); // host
    ds_list_replace(do_job_index, 3, -1); // device
    ds_list_replace(do_job_index, 4, -1); // shared ptr
    ds_list_replace(do_job_index, 5, -1); // cpu ptr
    //ds_list_replace(do_job_index, 6, -1); // host ptr
}

// cpu
if (g_traccc_ptrVsFlat_memLocation == 20) {
    ds_list_replace(do_job_index, 0, -1); // shared
    if (g_ptrVsFlat_firstStep) ds_list_replace(do_job_index, 1, -1); // cpu
    ds_list_replace(do_job_index, 2, -1); // host
    ds_list_replace(do_job_index, 3, -1); // device
    ds_list_replace(do_job_index, 4, -1); // shared ptr
    //ds_list_replace(do_job_index, 5, -1); // cpu ptr
    ds_list_replace(do_job_index, 6, -1); // host ptr
}


var do_job_index2 = ds_list_create();
ds_list_add(do_job_index2, ds_list_find_value(do_job_index, 4));
ds_list_add(do_job_index2, ds_list_find_value(do_job_index, 5));
ds_list_add(do_job_index2, ds_list_find_value(do_job_index, 6));
ds_list_add(do_job_index2, ds_list_find_value(do_job_index, 0));
ds_list_add(do_job_index2, ds_list_find_value(do_job_index, 1));
ds_list_add(do_job_index2, ds_list_find_value(do_job_index, 2));
ds_list_add(do_job_index2, ds_list_find_value(do_job_index, 3));

var tt = do_job_index;
do_job_index = do_job_index2;
ds_list_destroy(tt);

//if ( ! g_display_shared ) ds_list_replace(do_job_index, 4, -1);
//if ( ! g_display_host )   ds_list_replace(do_job_index, 5, -1);



ds_list_add(colors, c_black, c_aqua, c_blue, c_navy, c_lime, c_green, c_olive, c_yellow, c_orange, c_maroon, c_fuchsia, c_red, c_black);
var current_color_index = 0;

g_iteration_count = 0;

for (var loop_ij = 0; loop_ij < ds_list_size(ctrl.jobs_fixed_list); ++loop_ij) {
    var ij = loop_ij;
    
    //show_message("ij index = " + string(ij) + " size = " + string(ds_list_size(ctrl.jobs_fixed_list)));
    
    var new_jindex = ds_list_find_value(do_job_index, loop_ij);
    
    if (new_jindex == -1) continue;

    var j = ds_list_find_value(ctrl.jobs_fixed_list, new_jindex);

    // Seulement afficher la mémoire localisée à g_traccc_ptrVsFlat_memLocation.
    //if (j.MEMORY_LOCATION != g_traccc_ptrVsFlat_memLocation)  continue;
    
    for (var ids = 0; ids < ds_list_size(j.datasets); ++ids) {
    
        var ds = ds_list_find_value(j.datasets, ids);

        var total_items_count = j.VECTOR_SIZE_PER_ITERATION * j.PARALLEL_FOR_SIZE;
        
        // I don't care about ds.iterations for now
        
        var used_iteration_list = ds.iterations;//_only_parallel;
        
        var lsize = ds_list_size(used_iteration_list);
        if (lsize > g_iteration_count) g_iteration_count = lsize;
        if (lsize != 0) {
            
            var gpshort_name = "NC";
            var gpname = "inconnu";
            if (j.MEMORY_STRATEGY == 1) { gpshort_name = ""; gpname = mem_location_to_str(j.MEMORY_LOCATION) + ", pointer graph"; }
            if (j.MEMORY_STRATEGY == 2) { gpshort_name = "f"; gpname = mem_location_to_str(j.MEMORY_LOCATION) + ", flat (f)"; }
            //var gpshort_name = mem_location_to_str_prefix(j.MEMORY_LOCATION) + "" + mem_strategy_to_name_prefix(j.MEMORY_STRATEGY);
            //                   //+ ignore_alloc_time_to_name_prefix(j.IGNORE_ALLOC_TIME);
            // mem_location_to_str(j.MEMORY_LOCATION) +
            //var gpname = "" + "" + mem_strategy_to_name(j.MEMORY_STRATEGY)
                         //+ ", " + ignore_alloc_time_to_name(j.IGNORE_ALLOC_TIME)
            //             +  " (" + gpshort_name + ")";
            
            gp = find_or_create_graph_points_ext(graph_list, gpname, gpshort_name);
            if (gp.newly_created) {
                gp.color = ds_list_find_value(colors, current_color_index % ds_list_size(colors));
                ++current_color_index;
            }
            
            //if (g_traccc_ignore_allocation_time && 
            
            for (var i_iteration = 0; i_iteration < lsize; ++i_iteration) {
                //if (i_iteration <= 1) continue;
                var iter = ds_list_find_value(used_iteration_list, i_iteration);
                
                var gxoffset = 0;
                
                //var as_x = j.VECTOR_SIZE_PER_ITERATION;
                // allocation
                
                // Si prendre juste temps fill (et pas alloc) et que je suis dans le cas
                // d'un graphe de pointeurs, ne pas afficher ce point.
                if ( g_traccc_ignore_allocation_time && (j.MEMORY_STRATEGY == 1) ) {
                    // do nothing
                } else {
                    var as_x = 0 + gxoffset;
                    var as_y = 0; 
                    var pt = instance_create(0, 0, graph_single_point);
                    
                    if (g_traccc_ignore_allocation_time) {
                        pt.xlabel = "fill";
                        as_y = iter.t_flatten_fill; // ne prend en charge que flatten et pas graphe de ptr
                    } else {
                        if (j.MEMORY_LOCATION == 20) pt.xlabel = "CPU alloc & fill";
                        else                         pt.xlabel = "SYCL alloc & fill";
                        as_y = iter.t_alloc_fill;
                    }
                    
                    pt.xx = as_x;
                    pt.yy = as_y;
                    pt.ylabel = split_thousands(as_y);
                    pt.color = gp.color;
                    ds_list_add(gp.points, pt);
                }
                
                // Lorsque ignore alloc time, n'afficher que le temps sans alloc
                // car les autres temps sont identiques
                //if ( g_traccc_ignore_allocation_time ) { // j.IGNORE_ALLOC_TIME
                
                var as_x = 10 + gxoffset;
                var as_y = iter.t_copy_kernel;
                var pt = instance_create(0, 0, graph_single_point);
                pt.xx = as_x;
                pt.yy = as_y;
                if (j.MEMORY_LOCATION == 20) pt.xlabel = "CPU compute";
                else                         pt.xlabel = "GPU kernel";
                pt.ylabel = split_thousands(as_y);
                pt.color = gp.color; // <- debug only
                ds_list_add(gp.points, pt);
                
                var as_x = 20 + gxoffset;
                var as_y = iter.t_read;
                var pt = instance_create(0, 0, graph_single_point);
                pt.xx = as_x;
                pt.yy = as_y;
                if (j.MEMORY_LOCATION == 20) pt.xlabel = "read from CPU mem";
                else                         pt.xlabel = "read from SYCL mem";
                pt.ylabel = split_thousands(as_y);
                pt.color = gp.color; // <- debug only
                ds_list_add(gp.points, pt);
                //}
                
                // Ignorer le temps de libération mémoire
                if ( ! g_traccc_ignore_allocation_time ) { // j.IGNORE_ALLOC_TIME{
                    var as_x = 30 + gxoffset;
                    var as_y = iter.t_free_mem;
                    var pt = instance_create(0, 0, graph_single_point);
                    pt.xx = as_x;
                    pt.yy = as_y;
                    if (j.MEMORY_LOCATION == 20) pt.xlabel = "free CPU mem";
                    else                         pt.xlabel = "free SYCL mem";
                    pt.ylabel = split_thousands(as_y);
                    pt.color = gp.color; // <- debug only
                    ds_list_add(gp.points, pt);
                }
                //show_message("ij index = " + string(ij) + " ds index = " + string(ids) + "  pt index = " + string(i_iteration) + "  pt size = " + string(ds_list_size(gp.points)));
            }
        }
    }
}

draw_some_graph_shared_code(graph_list);

draw_graph_objs(graph_list, 20, 20, "", "Elapsed time µs", 0, g_ymax_impose, -1, -1);

ds_list_destroy(graph_list);
ds_list_destroy(colors);
with(graph_single_point) { instance_destroy(); }
with(graph_points) { instance_destroy(); }
