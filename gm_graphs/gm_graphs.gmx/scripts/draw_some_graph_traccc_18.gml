
/*

version traccc_18 :

IMPLICIT_USE_UNIQUE_MODULE

Module unique ou plusieurs modules

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

ds_list_add(colors, merge_colour(c_red, c_black, 0)); // host inout
ds_list_add(colors, merge_colour(c_blue, c_black, 0)); // shared inout
ds_list_add(colors, merge_colour(c_red, c_black, merge_cfactor)); // host unique
ds_list_add(colors, merge_colour(c_blue, c_black, merge_cfactor)); // shared unique


//ds_list_add(colors, merge_colour(c_blue, c_black, 0)); // graphe de pointeurs
//ds_list_add(colors, merge_colour(c_red, c_black, 0));  // flatten

/*
if (draw_graph_ptr && draw_flatten) {
    ds_list_add(colors, merge_colour(c_blue, c_black, 0)); // shared flat
    ds_list_add(colors, merge_colour(c_green, c_black, 0)); // glibc flat
    ds_list_add(colors, merge_colour(c_red, c_black, 0));  // host   flat
    ds_list_add(colors, merge_colour(c_maroon, c_black, 0));  // device   flat
    ds_list_add(colors, merge_colour(c_blue, c_black, merge_cfactor)); // shared graph pointer
    ds_list_add(colors, merge_colour(c_green, c_black, merge_cfactor)); // glibc graph pointer
    ds_list_add(colors, merge_colour(c_red, c_black, merge_cfactor)); // host    graph pointer
} else {
    ds_list_add(colors, merge_colour(c_blue, c_black, 0)); // shared
    ds_list_add(colors, merge_colour(c_green, c_black, 0)); // glibc
    ds_list_add(colors, merge_colour(c_red, c_black, 0));  // host
    ds_list_add(colors, merge_colour(c_maroon, c_black, 0));  // device (if flatten, else not used)
}*/


ds_list_add(colors, c_black, c_aqua, c_blue, c_navy, c_lime, c_green, c_olive, c_yellow, c_orange, c_maroon, c_fuchsia, c_red, c_black);
var current_color_index = 0;

g_iteration_count = 0;

for (var loop_ij = 0; loop_ij < ds_list_size(ctrl.jobs_fixed_list); ++loop_ij) {
    var ij = loop_ij;

    var j = ds_list_find_value(ctrl.jobs_fixed_list, ij);

    // Seulement afficher la mémoire host ou shared
    if ( (j.MEMORY_LOCATION != 0) && (j.MEMORY_LOCATION != 2) )  continue;
    
    if ( (g_traccc_ptrVsFlat_memLocation != -1)
      && (j.MEMORY_LOCATION != g_traccc_ptrVsFlat_memLocation) )  continue;
    
    for (var ids = 0; ids < ds_list_size(j.datasets); ++ids) {
    
        var ds = ds_list_find_value(j.datasets, ids);

        var total_items_count = j.VECTOR_SIZE_PER_ITERATION * j.PARALLEL_FOR_SIZE;
        
        // I don't care about ds.iterations for now
        
        var used_iteration_list = ds.iterations;//_only_parallel;
        
        var lsize = ds_list_size(used_iteration_list);
        if (lsize > g_iteration_count) g_iteration_count = lsize;
        if (lsize != 0) {
            
            var gpshort_name = mem_location_to_str_prefix(j.MEMORY_LOCATION) + "" + mem_strategy_to_name_prefix(j.MEMORY_STRATEGY)
                               + unique_module_to_str_prefix(j.IMPLICIT_USE_UNIQUE_MODULE);
                               //+ ignore_alloc_time_to_name_prefix(j.IGNORE_ALLOC_TIME);
            var gpname = "" + mem_location_to_str(j.MEMORY_LOCATION) + ", " + mem_strategy_to_name(j.MEMORY_STRATEGY)
                         + ", " + unique_module_to_str(j.IMPLICIT_USE_UNIQUE_MODULE)
                         //+ ", " + ignore_alloc_time_to_name(j.IGNORE_ALLOC_TIME)
                         + " (" + gpshort_name + ")";
            
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
                        pt.xlabel = "alloc & fill";
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
                pt.xlabel = "copy & kernel";
                pt.ylabel = split_thousands(as_y);
                pt.color = gp.color; // <- debug only
                ds_list_add(gp.points, pt);
                
                var as_x = 20 + gxoffset;
                var as_y = iter.t_read;
                var pt = instance_create(0, 0, graph_single_point);
                pt.xx = as_x;
                pt.yy = as_y;
                pt.xlabel = "read";
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
                    pt.xlabel = "free mem";
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

draw_graph_objs(graph_list, 20, 20, "Grandeur mesurée", "Temps pris en microsecondes", 0, -1, -1, -1);

ds_list_destroy(graph_list);
ds_list_destroy(colors);
with(graph_single_point) { instance_destroy(); }
with(graph_points) { instance_destroy(); }
