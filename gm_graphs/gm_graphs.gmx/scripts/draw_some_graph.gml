
/*
- évolution du temps pris d'une itération à l'autre
    x = n° itération (1, 2, ...)
    y = temps pris par [parallel for | allocation | copie | ... ]
*/

var j = ds_list_find_value(ctrl.jobs_fixed_list, 0);

var graph_list = ds_list_create();
var colors = ds_list_create();

ds_list_add(colors, c_blue, c_green, c_maroon, c_navy, c_lime, c_orange);

for (var ids = 0; ids < ds_list_size(j.datasets); ++ids) {
    
    var ds = ds_list_find_value(j.datasets, ids);
    
    
    var plist, xlabels, ylabels;
    plist = ds_list_create();
    xlabels = ds_list_create();
    ylabels = ds_list_create();
    
    for (var i_iteration = 0; i_iteration < ds_list_size(ds.iterations); ++i_iteration) {
        var iter = ds_list_find_value(ds.iterations, i_iteration);
        ds_list_add(plist, i_iteration, iter.t_parallel_for);
        ds_list_add(xlabels, string(i_iteration));
        ds_list_add(ylabels, string(iter.t_parallel_for));
    }
    
    var gp;
    gp = instance_create(0, 0, graph_points);
    ds_list_add(graph_list, gp);
    gp.color = merge_color(ds_list_find_value(colors, ids), c_red, 0.8);
    gp.points = plist;
    gp.xlabels = xlabels;
    gp.ylabels = ylabels;
    
    
    plist = ds_list_create();
    xlabels = ds_list_create();
    ylabels = ds_list_create();
    
    for (var i_iteration = 0; i_iteration < ds_list_size(ds.iterations_only_parallel); ++i_iteration) {
        var iter = ds_list_find_value(ds.iterations_only_parallel, i_iteration);
        ds_list_add(plist, i_iteration, iter.t_parallel_for);
        ds_list_add(xlabels, string(i_iteration));
        ds_list_add(ylabels, string(iter.t_parallel_for));
    }
    
    gp = instance_create(0, 0, graph_points);
    ds_list_add(graph_list, gp);
    gp.color = merge_color(ds_list_find_value(colors, ids), c_blue, 0.8);
    gp.points = plist;
    gp.xlabels = xlabels;
    gp.ylabels = ylabels;
    
    
    
}




draw_graph_objs(graph_list, 20, 20, "Itération n°", "Temps pris (ms)", 0, -1);
