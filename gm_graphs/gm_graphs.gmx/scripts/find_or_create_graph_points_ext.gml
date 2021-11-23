/// find_or_create_graph_points_ext(graph_list, name, string_on_points) : instance of graph_points
/// with newly_created set to true if it was not found and therfore created
/// string_on_points is the string (or real) that should be shown on every point to
/// differenciate them.

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
gp.hide_label = false; // true to display the curve but not the label
gp.points = ds_list_create(); // list of graph_single_point instances
//gp.xlabels = ds_list_create();
//gp.ylabels = ds_list_create();
gp.name = pname;
gp.dataset_index = dataset_index;
return gp;
