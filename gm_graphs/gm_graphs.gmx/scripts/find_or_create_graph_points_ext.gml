/// find_or_create_graph_points_ext(graph_list, name, dataset_index) : instance of graph_points
/// with newly_created set to true if it was not found and therfore created

var graph_list = argument0;
var pname = argument1;
var dataset_index = argument2;

var lsize = ds_list_size(graph_list);

for (var i = 0; i < lsize; ++i) {
    var gp = ds_list_find_value(graph_list, i);
    if (gp.name == pname) {
        gp.newly_created = false;
        return gp;
    }
}

var gp = instance_create(0, 0, graph_points);
ds_list_add(graph_list, gp);
gp.newly_created = true;
gp.points = ds_list_create(); // list of graph_single_point instances
//gp.xlabels = ds_list_create();
//gp.ylabels = ds_list_create();
gp.name = pname;
gp.dataset_index = dataset_index;
return gp;

