/// draw_some_graph_shared_code(graph_list);


var graph_list = argument0;

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
            if (xgroup.xx == pt.xx) { // faire avec xlabel plutôt que xx ?
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

var strange_value_factor = 1.5;//1.5; // normal : 1.5, inclusive : 6

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

return total_point_count;
