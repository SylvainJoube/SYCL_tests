
/*

v8 : comparaison des différents temps en fonction de la locatlisation de la mémoire.
i.e. temps parall_for, allocation, copy... en fonction de si la mémoire est
allouée en host, device ou shared.
Paramètre supplémentaire : memcpy de SYCL vs de la glibc.

- évolution du temps pris d'une itération à l'autre
    x = n° itération (1, 2, ...)
    y = temps pris par [parallel for | allocation | copie | ... ]
*/

var echelle_log = false;

g_graph_title = "Temps caractéristiques : allocation glibc vs SYCL";

/*if (echelle_log) g_graph_title += "(échelle log2)";
else             g_graph_title += "(échelle linéaire)";*/

var gp;
var graph_list = ds_list_create();
var colors = ds_list_create();

/*var max_color = 1;
var color_step = 0.1;

for (var col = 0; col < max_color; col += color_step) {
    ds_list_add(colors, merge_color(c_blue, c_black, col));
}*/

var merge_cfactor = 0.3;

ds_list_add(colors, merge_colour(c_blue, c_black, 0)); // shared
ds_list_add(colors, merge_colour(c_red, c_black, 0));  // host

ds_list_add(colors, merge_colour(c_blue, c_black, merge_cfactor)); // shared
ds_list_add(colors, merge_colour(c_green, c_black, 0)); // device
ds_list_add(colors, merge_colour(c_red, c_black, merge_cfactor)); // host (no device)


ds_list_add(colors, c_black, c_aqua, c_blue, c_navy, c_lime, c_green, c_olive, c_yellow, c_orange, c_maroon, c_fuchsia, c_red, c_black);
var current_color_index = 0;

g_iteration_count = 0;

for (var ij = 0; ij < ds_list_size(ctrl.jobs_fixed_list); ++ij) {

    var j = ds_list_find_value(ctrl.jobs_fixed_list, ij);
    //show_message("ij index = " + string(ij));
    
    // ingore when copy strategy is glibc and on device (no glibc on device)
    if ( j.MEMCOPY_IS_SYCL == 0 && j.MEMORY_LOCATION == 1 ) continue;
    //if ( j.MEMORY_LOCATION == 2 ) continue; // located on host
    
    
    for (var ids = 0; ids < ds_list_size(j.datasets); ++ids) {
    
        //if (ids == 0) continue;
        
        var ds = ds_list_find_value(j.datasets, ids);
        //show_message("ij index = " + string(ij) + " ds index = " + string(ids));
        
        //if (j.PARALLEL_FOR_SIZE <= 1024) continue;
        
        //var as_x = ids;//j.PARALLEL_FOR_SIZE;
        
        //if (echelle_log) as_x = log2(as_x);
        
        var total_items_count = j.VECTOR_SIZE_PER_ITERATION * j.PARALLEL_FOR_SIZE;
        
        
        // I don't care about ds.iterations for now
        
        var used_iteration_list = ds.iterations;//_only_parallel;
        
        var lsize = ds_list_size(used_iteration_list);
        if (lsize > g_iteration_count) g_iteration_count = lsize;
        if (lsize != 0) {
        //"dataset " + split_thousands(ids)
            var total_item_count = j.PARALLEL_FOR_SIZE * j.VECTOR_SIZE_PER_ITERATION;
            //gp = find_or_create_graph_points_ext(graph_list, "Nb. élems = " + string(total_item_count), ids); /// "nb. workitems = " + split_thousands(j.PARALLEL_FOR_SIZE)
            var memcopy_name = "non connue";
            var memcopy_short_name = "nc";
            if (j.MEMCOPY_IS_SYCL == 1 /*|| j.MEMORY_LOCATION == 1*/) { // copy strategy SYCL or mem on device (no glibc on device)
                memcopy_name = "copie SYCL";
                memcopy_short_name = "s";
            } else {
                memcopy_name = "copie glibc";
                memcopy_short_name = "g";
            }
            var gpname = "Mem location = " + mem_location_to_str(j.MEMORY_LOCATION) + " (" + memcopy_name + ")";
            var gpshort_name = mem_location_to_str_prefix(j.MEMORY_LOCATION) + "" + memcopy_short_name;
            // MEMCOPY_IS_SYCL
            gp = find_or_create_graph_points_ext(graph_list, gpname, gpshort_name);
            if (gp.newly_created) {
                gp.color = ds_list_find_value(colors, current_color_index % ds_list_size(colors));
                ++current_color_index;
            }
            
            for (var i_iteration = 0; i_iteration < lsize; ++i_iteration) {
                //if (i_iteration <= 1) continue;
                var iter = ds_list_find_value(used_iteration_list, i_iteration);
                
                var gxoffset = 0;
                gxoffset = j.MEMORY_LOCATION * 2 + j.MEMCOPY_IS_SYCL * 0.5;
                
                /*if (j.MEMCOPY_IS_SYCL == 1 || j.MEMORY_LOCATION == 1) {
                    gxoffset = j.MEMORY_LOCATION * 3;
                } else {
                    gxoffset = j.MEMORY_LOCATION * 3 + j.MEMCOPY_IS_SYCL * 0.7;
                }*/
                
                
                //var as_x = j.VECTOR_SIZE_PER_ITERATION;
                // allocation
                var as_x = 0 + gxoffset;
                var as_y = iter.t_allocation;
                var pt = instance_create(0, 0, graph_single_point);
                pt.xx = as_x;
                pt.yy = as_y;
                pt.xlabel = "gpu allocation"; //split_thousands(j.PARALLEL_FOR_SIZE);
                pt.ylabel = split_thousands(as_y);
                pt.color = gp.color; // <- debug only
                ds_list_add(gp.points, pt);
                
                // t_copy_to_device
                var as_x = 10 + gxoffset;
                var as_y = iter.t_copy_to_device;
                var pt = instance_create(0, 0, graph_single_point);
                pt.xx = as_x;
                pt.yy = as_y;
                pt.xlabel = "copy to device";
                pt.ylabel = split_thousands(as_y);
                pt.color = gp.color; // <- debug only
                ds_list_add(gp.points, pt);
                
                // t_parallel_for
                var as_x = 20 + gxoffset;
                var as_y = iter.t_parallel_for;
                var pt = instance_create(0, 0, graph_single_point);
                pt.xx = as_x;
                pt.yy = as_y;
                pt.xlabel = "parallel for";
                pt.ylabel = split_thousands(as_y);
                pt.color = gp.color; // <- debug only
                ds_list_add(gp.points, pt);
                
                // t_read_from_device
                var as_x = 30 + gxoffset;
                var as_y = iter.t_read_from_device;
                var pt = instance_create(0, 0, graph_single_point);
                pt.xx = as_x;
                pt.yy = as_y;
                pt.xlabel = "read from device";
                pt.ylabel = split_thousands(as_y);
                pt.color = gp.color; // <- debug only
                ds_list_add(gp.points, pt);
                
                // t_free_gpu
                var as_x = 40 + gxoffset;
                var as_y = iter.t_free_gpu; //t_free_gpu
                var pt = instance_create(0, 0, graph_single_point);
                pt.xx = as_x;
                pt.yy = as_y;
                pt.xlabel = "free gpu mem";
                pt.ylabel = split_thousands(as_y);
                pt.color = gp.color; // <- debug only
                ds_list_add(gp.points, pt);
                
                
                
                //show_message("ij index = " + string(ij) + " ds index = " + string(ids) + "  pt index = " + string(i_iteration) + "  pt size = " + string(ds_list_size(gp.points)));
            }
        }
    }
}

// For each graph_points instance, group points with the same x
for (var i = 0; i < ds_list_size(graph_list); ++i) {
    var gp = ds_list_find_value(graph_list, i);
    gp.xgroups = ds_list_create(); //instance_create(0, 0, graph_single_point_xgroup);
    var lptlen = ds_list_size(gp.points);
    
    for (var ipt = 0; ipt < lptlen; ++ipt) {
        var pt = ds_list_find_value(gp.points, ipt);
        var xglen = ds_list_size(gp.xgroups);
        var found_xgroup = false;
        
        for (var ixg = 0; ixg < xglen; ++ixg) {
            var xgroup = ds_list_find_value(gp.xgroups, ixg);
            if (xgroup.xx == pt.xx) {
                ds_list_add(xgroup.points, pt);
                found_xgroup = true;
                break;
            }
        }
        
        if ( ! found_xgroup ) {
            var xgroup = instance_create(0, 0, graph_single_point_xgroup);
            ds_list_add(gp.xgroups, xgroup);
            xgroup.xx = pt.xx;
            xgroup.xlabel = pt.xlabel;
            xgroup.points = ds_list_create();
            ds_list_add(xgroup.points, pt);
        }
    }
}

var strange_value_factor = 6; // normal : 1.5, inclusive : 6

var delete_strange_values = true;

var deleted_points_count = 0;

if (delete_strange_values) {
    // Delete strange values
    // TODO : finir la suppression des valeurs aberrantes
    for (var i = 0; i < ds_list_size(graph_list); ++i) {
        var gp = ds_list_find_value(graph_list, i);
        var xgroups_len = ds_list_size(gp.xgroups);
        
        for (var ig = 0; ig < xgroups_len; ++ig) {
            var xgroup = ds_list_find_value(gp.xgroups, ig);
            var ptlen = ds_list_size(xgroup.points);
            xgroup.deleted_strange_points = 0;
            
            if (ptlen <= 4) continue; // no median etc.
            
            // sort and delete strange values
            var ysort = ds_list_create();
            
            for (var ipt = 0; ipt < ptlen; ++ipt) {
                var pt = ds_list_find_value(xgroup.points, ipt);
                ds_list_add(ysort, pt.yy);
            }
            
            var quartils = compute_quartiles(ysort);
            var q1 = lfind(quartils, 0);
            var q2 = lfind(quartils, 1);
            var q3 = lfind(quartils, 2);
            
            var strange_threshold = strange_value_factor * (q3 - q1);
            
            
            var ipt = 0;
            for (var iuseless = 0; iuseless < ptlen; ++iuseless) {
                var pt = ds_list_find_value(xgroup.points, ipt);
                if ( abs(pt.yy - q2)  > strange_threshold ) {
                    // delete the point in gp list
                    for (var i2pt = 0; i2pt < ds_list_size(gp.points); ++i2pt) {
                        if (pt == ds_list_find_value(gp.points, i2pt)) {
                            ds_list_delete(gp.points, i2pt);
                            ++deleted_points_count;
                            //show_message("deleted item");
                            break; // only one instance in this list
                        }
                    }
                    with (pt) instance_destroy();
                    ds_list_delete(xgroup.points, ipt);
                    ++xgroup.deleted_strange_points;
                } else {
                    ++ipt;
                }
            }
        }
        
    }
}

var total_point_count = 0;
for (var i = 0; i < ds_list_size(graph_list); ++i) {
    var gp = ds_list_find_value(graph_list, i);
    total_point_count += ds_list_size(gp.points);
}

//show_message("total pts = " + string(total_point_count) + "  deleted = " + string(deleted_points_count) + "  : "
//             + string(deleted_points_count / total_point_count) + "%");

/*
Structure :
graph_list (list) -> graph_points (instance) : .points (list) -> graph_single_point (instance) : .xx
                                                                                                 .yy
                                                                                                 .xlabel
                                                                                                 .ylabel
                                               .xgroups (list) -> graph_single_point_xgroup (instance) : .points (list) -> graph_single_point (instance)
                                                                                                       : .xx
                                                                                                       : .xlabel

*/


/* Check if grouping is correct
for (var i = 0; i < ds_list_size(graph_list); ++i) {
    var gp = ds_list_find_value(graph_list, i);
    var xglen = ds_list_size(gp.xgroups);
    var vstr = "";
    for (var ixg = 0; ixg < xglen; ++ixg) {
        var xgroup = ds_list_find_value(gp.xgroups, ixg);
        vstr += chr(10) + "[i" + string(ixg) + "] - " + xgroup.xlabel + " - size " + string(ds_list_size(xgroup.points));
    }
    show_message("Points " + gp.name + " - size of xgroups " + string(ds_list_size(gp.xgroups)) + vstr);
}*/


//g_graph_title += chr(10) + "il y a " + string(ds_list_size(gp.points)) + " gp points à dessiner. et ds len = "+ string(ds_list_size(ds.iterations_only_parallel));

// Afficher les labels pour identifier les différents points
// Merge des labels entre eux s'ils correspondent (afficher le ds et les autrre sinfos)

//show_message("(should be 6) graph_list size = " + string(ds_list_size(graph_list)));

var sorted_glist = ds_list_create();
for (var i = 0; i <= 2; ++i) {
    ds_list_add(sorted_glist, ds_list_find_value(graph_list, 0 + i));
    ds_list_add(sorted_glist, ds_list_find_value(graph_list, 3 + i));
}

draw_graph_objs(graph_list, 20, 20, "Grandeur mesurée", "Temps pris en microsecondes", 0, -1, -1, -1);

ds_list_destroy(graph_list);
ds_list_destroy(sorted_glist);
ds_list_destroy(colors);
with(graph_single_point) { instance_destroy(); }
with(graph_points) { instance_destroy(); }


