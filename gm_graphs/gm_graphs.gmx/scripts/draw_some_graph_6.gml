
/*
- évolution du temps pris d'une itération à l'autre
    x = n° itération (1, 2, ...)
    y = temps pris par [parallel for | allocation | copie | ... ]
*/

var echelle_log = true;

g_graph_title = "Temps pris en fonction de L et M, à nombre d'éléments constant ";

if (echelle_log) g_graph_title += "(échelle log2)";
else             g_graph_title += "(échelle linéaire)";

var gp;
var graph_list = ds_list_create();
var colors = ds_list_create();

/*var max_color = 1;
var color_step = 0.1;

for (var col = 0; col < max_color; col += color_step) {
    ds_list_add(colors, merge_color(c_blue, c_black, col));
}*/

ds_list_add(colors, c_black, c_aqua, c_blue, c_navy, c_lime, c_green, c_olive, c_yellow, c_orange, c_maroon, c_fuchsia, c_red, c_black);
var current_color_index = 0;

for (var ij = 0; ij < ds_list_size(ctrl.jobs_fixed_list); ++ij) {

    var j = ds_list_find_value(ctrl.jobs_fixed_list, ij);
    //show_message("ij index = " + string(ij));
    
    for (var ids = 0; ids < ds_list_size(j.datasets); ++ids) {
    
        //if (ids == 0) continue;
        
        var ds = ds_list_find_value(j.datasets, ids);
        //show_message("ij index = " + string(ij) + " ds index = " + string(ids));
        
        //if (j.PARALLEL_FOR_SIZE <= 1024) continue;
        
        //var as_x = ids;//j.PARALLEL_FOR_SIZE;
        
        //if (echelle_log) as_x = log2(as_x);
        
        var total_items_count = j.VECTOR_SIZE_PER_ITERATION * j.PARALLEL_FOR_SIZE;
        
        
        // I don't care about ds.iterations for now
        
        var lsize = ds_list_size(ds.iterations_only_parallel);
        if (lsize != 0) {
        //"dataset " + split_thousands(ids)
            var total_item_count = j.PARALLEL_FOR_SIZE * j.VECTOR_SIZE_PER_ITERATION;
            //gp = find_or_create_graph_points_ext(graph_list, "Nb. élems = " + string(total_item_count), ids); /// "nb. workitems = " + split_thousands(j.PARALLEL_FOR_SIZE)
            gp = find_or_create_graph_points_ext(graph_list, "Mem location = " + mem_location_to_str(j.MEMORY_LOCATION), j.MEMORY_LOCATION);
            if (gp.newly_created) {
                gp.color = ds_list_find_value(colors, current_color_index % ds_list_size(colors));
                ++current_color_index;
            }
            
            for (var i_iteration = 0; i_iteration < lsize; ++i_iteration) {
                //if (i_iteration <= 1) continue;
                var iter = ds_list_find_value(ds.iterations_only_parallel, i_iteration);
                var as_x = j.VECTOR_SIZE_PER_ITERATION;
                var as_y = iter.t_parallel_for; // temps pris par le parallel_for
                var pt = instance_create(0, 0, graph_single_point);
                if ( ! echelle_log ) pt.xx = as_x;
                else                 pt.xx = log2(as_x);
                pt.yy = as_y;
                pt.xlabel = "L(" + split_thousands(j.VECTOR_SIZE_PER_ITERATION) + ")" + chr(10) + "M(" + split_thousands(j.PARALLEL_FOR_SIZE) + ")"; //split_thousands(j.PARALLEL_FOR_SIZE);
                pt.ylabel = split_thousands(as_y);
                pt.color = gp.color; // <- debug only
                //ds_list_add(gp.points, as_x, as_y);
                //ds_list_add(gp.xlabels, split_thousands(j.PARALLEL_FOR_SIZE));
                //ds_list_add(gp.ylabels, string(as_y));
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

var strange_value_factor = 3;

var deleted_points_count = 0;

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
        
        var strange_threshold = 6 * (q3 - q1);
        
        
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
        
        /*ds_list_sort(ysort, true);
        
        var med;
        var q3_start_index;
        if (ptlen % 2 == 0) {
            var low = ds_list_find_value(ysort, floor(ptlen / 2) - 1);
            var high = ds_list_find_value(ysort, floor(ptlen / 2));
            med = (low + high) / 2;
            q3_start_index = ptlen / 2; // floor not needed
        } else {
            med = ds_list_find_value(ysort, floor(ptlen / 2));
            q3_start_index = floor(ptlen / 2) + 1;
        }
        
        var q1, q2, q3;
        var qlen = floor(ptlen / 2);
        
        q2 = med;
        
        // Q1
        if (qlen % 2 == 0) {
            var low = ds_list_find_value(ysort, floor(qlen / 2) - 1);
            var high = ds_list_find_value(ysort, floor(qlen / 2));
            q1 = (low + high) / 2;
        } else {
            q1 = ds_list_find_value(ysort, floor(qlen / 2));
        }
        
        // Q3
        
        if (qlen % 2 == 0) {
            var low = ds_list_find_value(ysort, floor(qlen / 2) - 1 + q3_start_index);
            var high = ds_list_find_value(ysort, floor(qlen / 2) + q3_start_index);
            q3 = (low + high) / 2;
        } else {
            q3 = ds_list_find_value(ysort, floor(qlen / 2) + q3_start_index);
        }*/
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

draw_graph_objs(graph_list, 20, 20, "Taille du vecteur", "Temps pris par parallel_for (us)", 0, -1, -1, -1);

ds_list_destroy(graph_list);
ds_list_destroy(colors);
with(graph_single_point) { instance_destroy(); }
with(graph_points) { instance_destroy(); }


