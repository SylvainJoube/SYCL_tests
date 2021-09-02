
/*

version traccc_16 : comparaison des différents temps en fonction de la locatlisation de la mémoire.


WIP :
i.e. temps parall_for, allocation, copy... en fonction de si la mémoire est
allouée en host, device ou shared.
Paramètre supplémentaire : memcpy de SYCL vs de la glibc.

- évolution du temps pris d'une itération à l'autre
    x = n° itération (1, 2, ...)
    y = temps pris par [parallel for | allocation | copie | ... ]
*/

var echelle_log = false;

// deprecated g_graph_title = "Allocation glibc vs SYCL - SANDOR - 6 GiB - O2";

/*if (echelle_log) g_graph_title += "(échelle log2)";
else             g_graph_title += "(échelle linéaire)";*/

var gp;
var graph_list = ds_list_create();
var colors = ds_list_create();

var merge_cfactor = 0.3;

ds_list_add(colors, merge_colour(c_blue, c_black, 0)); // shared flat
ds_list_add(colors, merge_colour(c_green, c_black, 0)); // glibc flat
ds_list_add(colors, merge_colour(c_red, c_black, 0));  // host   flat

ds_list_add(colors, merge_colour(c_blue, c_black, merge_cfactor)); // shared graph pointer
ds_list_add(colors, merge_colour(c_green, c_black, merge_cfactor)); // glibc graph pointer
ds_list_add(colors, merge_colour(c_red, c_black, merge_cfactor)); // host    graph pointer


ds_list_add(colors, c_black, c_aqua, c_blue, c_navy, c_lime, c_green, c_olive, c_yellow, c_orange, c_maroon, c_fuchsia, c_red, c_black);
var current_color_index = 0;

g_iteration_count = 0;

for (var ij = 0; ij < ds_list_size(ctrl.jobs_fixed_list); ++ij) {

    var j = ds_list_find_value(ctrl.jobs_fixed_list, ij);

    // ingore when copy strategy is glibc and on device (no glibc on device)
    //if ( j.MEMCOPY_IS_SYCL == 0 && j.MEMORY_LOCATION == 1 ) continue;
    //if ( j.MEMORY_LOCATION == 2 ) continue; // located on host
    
    
    for (var ids = 0; ids < ds_list_size(j.datasets); ++ids) {
    
        var ds = ds_list_find_value(j.datasets, ids);

        var total_items_count = j.VECTOR_SIZE_PER_ITERATION * j.PARALLEL_FOR_SIZE;
        
        
        // I don't care about ds.iterations for now
        
        var used_iteration_list = ds.iterations;//_only_parallel;
        
        var lsize = ds_list_size(used_iteration_list);
        if (lsize > g_iteration_count) g_iteration_count = lsize;
        if (lsize != 0) {
            
            var gpshort_name = mem_location_to_str_prefix(j.MEMORY_LOCATION) + "" + mem_strategy_to_name_prefix(j.MEMORY_STRATEGY);
            var gpname = "" + mem_location_to_str(j.MEMORY_LOCATION) + ", " + mem_strategy_to_name(j.MEMORY_STRATEGY) + " (" + gpshort_name + ")";
            
            gp = find_or_create_graph_points_ext(graph_list, gpname, gpshort_name);
            if (gp.newly_created) {
                gp.color = ds_list_find_value(colors, current_color_index % ds_list_size(colors));
                ++current_color_index;
            }
            
            for (var i_iteration = 0; i_iteration < lsize; ++i_iteration) {
                //if (i_iteration <= 1) continue;
                var iter = ds_list_find_value(used_iteration_list, i_iteration);
                
                var gxoffset = 0;
                
                //var as_x = j.VECTOR_SIZE_PER_ITERATION;
                // allocation
                var as_x = 0 + gxoffset;
                var as_y = iter.t_alloc_fill;
                var pt = instance_create(0, 0, graph_single_point);
                pt.xx = as_x;
                pt.yy = as_y;
                pt.xlabel = "alloc & fill"; //split_thousands(j.PARALLEL_FOR_SIZE);
                pt.ylabel = split_thousands(as_y);
                pt.color = gp.color; // <- debug only
                ds_list_add(gp.points, pt);
                
                // t_copy_to_device
                var as_x = 10 + gxoffset;
                var as_y = iter.t_copy_kernel;
                var pt = instance_create(0, 0, graph_single_point);
                pt.xx = as_x;
                pt.yy = as_y;
                pt.xlabel = "copy & kernel";
                pt.ylabel = split_thousands(as_y);
                pt.color = gp.color; // <- debug only
                ds_list_add(gp.points, pt);
                
                // t_parallel_for
                var as_x = 20 + gxoffset;
                var as_y = iter.t_read;
                var pt = instance_create(0, 0, graph_single_point);
                pt.xx = as_x;
                pt.yy = as_y;
                pt.xlabel = "read";
                pt.ylabel = split_thousands(as_y);
                pt.color = gp.color; // <- debug only
                ds_list_add(gp.points, pt);
                
                // t_read_from_device
                var as_x = 30 + gxoffset;
                var as_y = iter.t_free_mem;
                var pt = instance_create(0, 0, graph_single_point);
                pt.xx = as_x;
                pt.yy = as_y;
                pt.xlabel = "free mem";
                pt.ylabel = split_thousands(as_y);
                pt.color = gp.color; // <- debug only
                ds_list_add(gp.points, pt);
                
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
