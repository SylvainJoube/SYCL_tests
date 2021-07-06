
/*
- évolution du temps pris d'une itération à l'autre
    x = n° itération (1, 2, ...)
    y = temps pris par [parallel for | allocation | copie | ... ]
*/

g_graph_title = "Temps pris par parallel_for en fonction du nombre de workitems (échelle linéaire)"
                + chr(10) + "L = 1 (VECTOR_SIZE_PER_WORKITEM = 1)"
                + chr(10) + "- seul le temps pris par le parallal_for est mesuré -"
                + chr(10) + "rouge = à chaque fois [allocation + envoi des données + free]"
                + chr(10) + "bleu = une seule fois [allocation + envoi des données + free]"
                ; // VECTOR_SIZE_PER_WORKITEM = VECTOR_SIZE_PER_ITERATION

var graph_list = ds_list_create();
var colors = ds_list_create();
ds_list_add(colors, c_blue, c_green, c_maroon, c_navy, c_lime, c_orange);

for (var ij = 0; ij < ds_list_size(ctrl.jobs_fixed_list); ++ij) {

    var j = ds_list_find_value(ctrl.jobs_fixed_list, ij);
    
    
    for (var ids = 0; ids < ds_list_size(j.datasets); ++ids) {
        
        var ds = ds_list_find_value(j.datasets, ids);
        
        var as_x = (j.PARALLEL_FOR_SIZE);
        
        var plist, xlabels, ylabels;
        plist = ds_list_create();
        xlabels = ds_list_create();
        ylabels = ds_list_create();
        
        for (var i_iteration = 0; i_iteration < ds_list_size(ds.iterations); ++i_iteration) {
            var iter = ds_list_find_value(ds.iterations, i_iteration);
            ds_list_add(plist, as_x, iter.t_parallel_for); // i_iteration
            ds_list_add(xlabels, string(j.PARALLEL_FOR_SIZE));
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
            ds_list_add(plist, as_x, iter.t_parallel_for);
            ds_list_add(xlabels, string(j.PARALLEL_FOR_SIZE));
            ds_list_add(ylabels, string(iter.t_parallel_for));
        }
        
        gp = instance_create(0, 0, graph_points);
        ds_list_add(graph_list, gp);
        gp.color = merge_color(ds_list_find_value(colors, ids), c_blue, 0.8);
        gp.points = plist;
        gp.xlabels = xlabels;
        gp.ylabels = ylabels;
        
        
        
    }
}

draw_graph_objs(graph_list, 20, 20, "Nombre de workitems (i.e. taille parallel_for)", "Temps pris (ms)", 0, -1, -1, -1);


