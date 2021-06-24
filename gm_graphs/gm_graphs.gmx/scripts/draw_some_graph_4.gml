
/*
- évolution du temps pris d'une itération à l'autre
    x = n° itération (1, 2, ...)
    y = temps pris par [parallel for | allocation | copie | ... ]
*/

var echelle_log = true;

g_graph_title = "Temps pris pour chaque élement du vecteur, en fonction du nombre de workitems ";

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

ds_list_add(colors, c_aqua, c_blue, c_navy, c_lime, c_green, c_olive, c_yellow, c_orange, c_maroon, c_fuchsia, c_red, c_black);
var current_color_index = 0;

for (var ij = 0; ij < ds_list_size(ctrl.jobs_fixed_list); ++ij) {

    var j = ds_list_find_value(ctrl.jobs_fixed_list, ij);
    //show_message("ij index = " + string(ij));
    
    for (var ids = 0; ids < ds_list_size(j.datasets); ++ids) {
        
        var ds = ds_list_find_value(j.datasets, ids);
        //show_message("ij index = " + string(ij) + " ds index = " + string(ids));
        
        //if (j.PARALLEL_FOR_SIZE <= 1024) continue;
        
        var as_x = j.PARALLEL_FOR_SIZE;
        
        if (echelle_log) as_x = log2(as_x);
        
        var total_items_count = j.VECTOR_SIZE_PER_ITERATION * j.PARALLEL_FOR_SIZE;
        
        
        /*var plist, xlabels, ylabels;
        plist = ds_list_create();
        xlabels = ds_list_create();
        ylabels = ds_list_create();
        
        for (var i_iteration = 0; i_iteration < ds_list_size(ds.iterations); ++i_iteration) {
            var iter = ds_list_find_value(ds.iterations, i_iteration);
            
            var as_y = iter.t_parallel_for / total_items_count; // temps pris par un ajout à la somme totale
            ds_list_add(plist, as_x, as_y); // i_iteration
            ds_list_add(xlabels, split_thousands(j.PARALLEL_FOR_SIZE));
            ds_list_add(ylabels, split_thousands(iter.t_parallel_for));
        }
        
        var gp;
        if ( ds_list_size(plist) != 0 ) {
            gp = instance_create(0, 0, graph_points);
            ds_list_add(graph_list, gp);
            gp.color = merge_color(ds_list_find_value(colors, ids), c_red, 0.8);
            gp.points = plist;
            gp.xlabels = xlabels;
            gp.ylabels = ylabels;
            gp.name = "Ne devrait pas exister";
        } else {
            ds_list_destroy(plist);
            ds_list_destroy(xlabels);
            ds_list_destroy(ylabels);
        }*/
        
        // I don't care about ds.iterations for now
        
        var lsize = ds_list_size(ds.iterations_only_parallel);
        if (lsize != 0) {
            gp = find_or_create_graph_points(graph_list, "L = " + split_thousands(j.VECTOR_SIZE_PER_ITERATION)); /// "nb. workitems = " + split_thousands(j.PARALLEL_FOR_SIZE)
            if (gp.newly_created) {
                gp.color = ds_list_find_value(colors, current_color_index % ds_list_size(colors));
                ++current_color_index;
            }
            
            for (var i_iteration = 0; i_iteration < ds_list_size(ds.iterations_only_parallel); ++i_iteration) {
                //if (i_iteration <= 1) continue;
                var iter = ds_list_find_value(ds.iterations_only_parallel, i_iteration);
                var as_y = iter.t_parallel_for / (total_items_count / 1000000); // temps pris par un ajout à la somme totale
                var pt = instance_create(0, 0, graph_single_point);
                pt.xx = as_x;
                pt.yy = as_y;
                pt.xlabel = split_thousands(j.PARALLEL_FOR_SIZE);
                pt.ylabel = string(as_y);
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

// Delete strange values
// TODO : finir la suppression des valeurs aberrantes
for (var i = 0; i < ds_list_size(graph_list); ++i) {
    var gp = ds_list_find_value(graph_list, i);
    var xgroups_len = ds_list_size(gp.xgroups);
    
    for (var ig = 0; ig < xgroups_len; ++ig) {
        var xgroup = ds_list_find_value(gp.xgroups, ig);
        var ptlen = ds_list_size(xgroup.points);
        // sort and delete strange values
        var ysort = ds_list_create();
        
        for (var ipt = 0; ipt < ptlen; ++ipt) {
            var pt = ds_list_find_value(xgroup.points, ipt);
            ds_list_add(ysort, pt.yy);
        }
        ds_list_sort(ysort, true);
        
    }
    
}

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

draw_graph_objs(graph_list, 20, 20, "Nombre de workitems (i.e. taille parallel_for)", "Temps pris par élément (ns)", 0, -1);

ds_list_destroy(graph_list);
ds_list_destroy(colors);
with(graph_single_point) { instance_destroy(); }
with(graph_points) { instance_destroy(); }


