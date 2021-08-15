/// draw_graph_objs(graph_list, x_space_left, y_space_left, xlabel, ylabel, ymin, ymax);

var graph_list  = argument0;
var list = graph_list;
var x_space_left = argument1;
var y_space_left = argument2;
var xlabel = argument3;
var ylabel = argument4;
var ymin_impose = argument5;
var ymax_impose = argument6;
var xmin_impose = argument7;
var xmax_impose = argument8;

var xmin = 0;
var ymin = 0;
var xmax = 0;
var ymax = 0;

var graph_yoffset = 160;
var graph_label_ystart = 70;

var graph_height = 700;
var graph_width = 800;

// surface height = 

var xorig = 100;
var yorig = graph_height + graph_yoffset;

// Draw the origin lines
draw_set_color(c_black);
draw_set_alpha(1);
draw_line(xorig, yorig, xorig + graph_width, yorig);
draw_line(xorig, yorig, xorig, yorig - graph_height);

draw_set_alpha(0.07);
var x_orig_and_offset = xorig + x_space_left;
var y_orig_and_offset = yorig - y_space_left;
draw_line(x_orig_and_offset, y_orig_and_offset, x_orig_and_offset + graph_width - x_space_left, y_orig_and_offset);
draw_line(x_orig_and_offset, y_orig_and_offset, x_orig_and_offset, y_orig_and_offset - graph_height + y_space_left);

draw_set_alpha(1);
draw_set_halign(fa_center);
draw_set_font(ft_base);

// Draw labels
draw_text(xorig + floor(graph_width / 2), yorig + 60, xlabel);
draw_text_transformed(xorig - 90, yorig - floor(graph_height / 2), ylabel, 1, 1, 90);

// Draw graph title
var xcenter_surface = floor((graph_width + xorig * 2) / 2);
draw_text(xcenter_surface, 20, g_graph_display_name);

// Draw L and M, if their values are shared
draw_set_halign(fa_right);
if (g_display_LM) {
    /*draw_text(graph_width + xorig - x_space_left + 100, 20,
        "L = " + split_thousands(g_VECTOR_SIZE_PER_ITERATION_common) + chr(10)
      + "M = " + split_thousands(g_PARALLEL_FOR_SIZE_common) + chr(10)
      + "Itérations = " + split_thousands(g_iteration_count)
    );*/
    
    draw_text(graph_width + xorig - x_space_left + 100, 60,
        split_thousands(g_VECTOR_SIZE_PER_ITERATION_common) + " = L" + chr(10)
      + split_thousands(g_PARALLEL_FOR_SIZE_common) + " = M" + chr(10)
      + split_thousands(g_iteration_count) + " pts" + chr(10)
      + "in " + split_thousands(round(g_input_data_size / 1024)) + " kio" + chr(10)
      + "out " + split_thousands(round(g_output_data_size / 1024)) + " kio"
    );
} else {
    draw_text(graph_width + xorig - x_space_left + 100, 60,
      + split_thousands(g_iteration_count) + " pts" + chr(10)
      + "in " + split_thousands(round(g_input_data_size / 1024)) + " kio"
    );
}


var plabel_xmin = 160;
var plabel_width = graph_width - xorig - 20;
var plabel_ymin = graph_label_ystart;
var plabel_cx = plabel_xmin; // current x, y
var plabel_cy = plabel_ymin;
var plabel_xmax = plabel_xmin + plabel_width;

var DRAW_LABEL_ON_GRAPH = true;

// Draw the points labels
for (var igp = 0; igp < ds_list_size(graph_list); ++igp) {
    var gp = ds_list_find_value(graph_list, igp);
    var drawn_text = gp.name;
    draw_set_halign(fa_left);
    draw_set_valign(fa_center);
    draw_set_font(ft_point_label);
    draw_set_color(gp.color);
    var str_height = string_height(drawn_text);
    var str_width = string_width(drawn_text);
    var full_width = str_width + 60
    if (plabel_cx + full_width >= plabel_xmax) {
        // new line
        plabel_cx = plabel_xmin;
        var line_height = str_height + 2;
        plabel_cy += line_height;
    }
    draw_circle(plabel_cx - 20, plabel_cy, 5, false);
    //draw_set_color(c_black);
    if (DRAW_LABEL_ON_GRAPH) {
        draw_text(plabel_cx, plabel_cy, drawn_text); //"--" + drawn_text + "--");
    }
    //ydraw_plabel += str_height + 10;
    plabel_cx += full_width;
}
draw_set_valign(fa_top);

//var pt_count = 0;

// Compute max and min coordinates
for (var igp = 0; igp < ds_list_size(graph_list); ++igp) {
    var gp = ds_list_find_value(graph_list, igp);
    var list = gp.points;
    // Minimum and maximum coordinates computation
    for (var i = 0; i < ds_list_size(gp.points); ++i) {
        var pt = ds_list_find_value(gp.points, i);
        var xx = pt.xx; //ds_list_find_value(list, i);
        var yy = pt.yy; //ds_list_find_value(list, i + 1);
        if ( (igp == 0) && (i == 0) ) {
            xmin = xx;
            ymin = yy;
            xmax = xx;
            ymax = yy;
        }
        if (xmin > xx) xmin = xx;
        if (ymin > yy) ymin = yy;
        if (xmax < xx) xmax = xx;
        if (ymax < yy) ymax = yy;
        //++pt_count;
    }
}
//show_message("pt_count = " + string(pt_count));
//pt_count = 0;
/*xmin -= x_space_left;
xmax += x_space_left;
ymin -= y_space_left;
ymax += y_space_left;*/

if (ymin_impose != -1) ymin = ymin_impose;
if (ymax_impose != -1) ymax = ymax_impose;
if (xmin_impose != -1) xmin = xmin_impose;
if (xmax_impose != -1) xmax = xmax_impose;

//show_message("x : " + string(xmin) + " -> " + string(xmax)
//             + chr(10) + "y : " + string(ymin) + " -> " + string(ymax));

var scale_width  = xmax - xmin + 1;
var scale_height = ymax - ymin + 1;

var scale_width_factor  = (graph_width - x_space_left * 2) / scale_width;
var scale_height_factor = (graph_height - y_space_left * 2) / scale_height;

var drawn_labels_y = ds_list_create();
var drawn_labels_x = ds_list_create();
var drawn_labels_str = ds_list_create();

draw_set_font(ft_small_number_ylabel)
var min_distance_draw_label_y = string_height("0123456789") + 2; // 30

var line_width_div2 = 5;
var line_width_div2_max = 10;
var line_height = 1;
var line_height_max = 2;
var line_width_div2_vertical = 1;

/*
Dessin de lignes entre les médianes de points ayant le même label
i.e. un seul trait entre paquets de points à x différents
Donc une liste contenant à chaque index :
- une liste de points (x, y) à relier
Pour chaque label : 
*/
//var a1_label_name = ds_list_create();
//var a2_label_positions = ds_list_create();



// Draw the points
for (var igp = 0; igp < ds_list_size(graph_list); ++igp) {
    var gp = ds_list_find_value(graph_list, igp);
    var list = gp.points;
    
    // Draw all points from an xgroup
    
    var gp_xdraw_old = -1;
    var gp_ydraw_old = -1;
    
    
    // For each xgroup
    for (var ixg = 0; ixg < ds_list_size(gp.xgroups); ++ixg) {
        var xgroup = ds_list_find_value(gp.xgroups, ixg);
        
        // y min and max for thix xgroup
        var xgroup_ymin = -1;
        var xgroup_ymax = -1;
        
        // xdraw on surface
        // xorig : on screen x origin
        // (xgroup.xx - xmin) : xgroup x position relative to first drawn x
        // * scale_width_factor : puton scale, retavive to min and max x
        // x_space_left : static offset
        var xdraw = xorig + (xgroup.xx - xmin) * scale_width_factor + x_space_left;
        // xgroup.xx equals every pt.xx of this xgroup.
        
        var median_y_sum = 0;
        
        // For each point of an xgroup, draw the line
        for (var i = 0; i < ds_list_size(xgroup.points); ++i) {
            var pt = ds_list_find_value(xgroup.points, i);
            //var xx = pt.xx; //ds_list_find_value(list, i);
            //var yy = pt.yy; //ds_list_find_value(list, i + 1);
            
            // ydraw on surface
            // yorig : on screen y origin
            // (pt.yy - ymin) : point y position relative to first drawn y
            // * scale_height_factor : put on scale, retavive to min and max y
            // x_space_left : static offset
            // 
            var ydraw = yorig - (pt.yy - ymin) * scale_height_factor - y_space_left;
            draw_set_color(gp.color);
            draw_set_alpha(1);
            draw_line_width(xdraw - line_width_div2, ydraw, xdraw + line_width_div2, ydraw, line_height);
            if (i == 0) {
                xgroup_ymin = ydraw;
                xgroup_ymax = ydraw;
            }
            if (xgroup_ymin > ydraw) xgroup_ymin = ydraw;
            if (xgroup_ymax < ydraw) xgroup_ymax = ydraw;
            //draw_circle(xdraw, ydraw, 5, false);
            
            median_y_sum += ydraw;
            
            // Draw y label, if possible
            var can_draw = true;
            var current_label = pt.ylabel;
            // Draw label if no label is near it
            for (var il = 0; il < ds_list_size(drawn_labels_y); ++il) {
                
                var l2_y = ds_list_find_value(drawn_labels_y, il);
                //show_message(string(ydraw) + string(l2_y));
                if (abs(ydraw - l2_y) <= min_distance_draw_label_y) {
                    can_draw = false;
                    break;
                }
            }
            
            if (can_draw) {
                draw_set_font(ft_small_number_ylabel);
                //draw_set_color(c_black);
                draw_set_color(pt.color);
                draw_set_alpha(1);
                draw_set_halign(fa_right);
                draw_set_valign(fa_middle);
                
                draw_text(round(xorig - 4), round(ydraw), current_label);
                ds_list_add(drawn_labels_y, ydraw);
                draw_line(xorig, ydraw, xorig + 6, ydraw);
                
                draw_set_valign(fa_top);
            }
            
        }
        // Draw maximum and minimum y for this xgroup
        draw_line_width(xdraw - line_width_div2_max, xgroup_ymin, xdraw + line_width_div2_max, xgroup_ymin, line_height_max);
        draw_line_width(xdraw - line_width_div2_max, xgroup_ymax, xdraw + line_width_div2_max, xgroup_ymax, line_height_max);
        draw_line_width(xdraw, xgroup_ymin, xdraw, xgroup_ymax, line_width_div2_vertical);
        
        // emulate a median
        var median_ydraw = (xgroup_ymin + xgroup_ymax) / 2;
        if (ds_list_size(xgroup.points) != 0) {
            median_ydraw = round(median_y_sum / ds_list_size(xgroup.points));
        }
        
        
        if ( (gp_xdraw_old != -1) ) {
            draw_line(gp_xdraw_old, gp_ydraw_old, xdraw, median_ydraw);
        }
        
        gp_xdraw_old = xdraw;
        gp_ydraw_old = median_ydraw;
        
        
        draw_set_font(ft_very_small_number);
        draw_set_alpha(1);
        draw_set_halign(fa_center);
        var sh = string_height(gp.name);
        //draw_text(round(xdraw), round(xgroup_ymin - sh + 1), gp.name + " d(" + string(xgroup.deleted_strange_points) + ")");
        draw_text(round(xdraw), round(xgroup_ymin - sh + 2), string(gp.dataset_index));
        //++pt_count;
        //draw_text(xorig + 40, yorig - i * 12, string(i) + " (" + );
        
        
        
        //draw_set_font(ft_base);
        draw_set_font(ft_small_number);
        
        // x label
        var can_draw = true;
        var current_label = xgroup.xlabel;
        // Draw label if no label is near it
        for (var il = 0; il < ds_list_size(drawn_labels_x); ++il) {
            
            var check_label_str = ds_list_find_value(drawn_labels_str, il);
            var l2_x = ds_list_find_value(drawn_labels_x, il);
            var dist_min = (string_width(current_label) + string_width(check_label_str)) / 2 + 10;
        
            if (abs(xdraw - l2_x) <= dist_min) {
                can_draw = false;
                break;
            }
        }
        
        if (can_draw) {
            draw_set_font(ft_small_number);
            draw_set_color(c_black);
            draw_set_alpha(1);
            draw_set_halign(fa_center);
            draw_text(round(xdraw), round(yorig) , current_label);
            ds_list_add(drawn_labels_x, xdraw);
            ds_list_add(drawn_labels_str, current_label);
            draw_line(xdraw, yorig, xdraw, yorig - 6);
        }
        
        
        
    }
    
    
    
    
    
    
    
    
    
    
    
    /* Old code
    
    for (var i = 0; i < ds_list_size(gp.points); ++i) {
        var pt = ds_list_find_value(gp.points, i);
        var xx = pt.xx; //ds_list_find_value(list, i);
        var yy = pt.yy; //ds_list_find_value(list, i + 1);
        var xdraw = xorig + (xx - xmin) * scale_width_factor + x_space_left;
        var ydraw = yorig - (yy - ymin) * scale_height_factor - y_space_left;
        draw_set_color(gp.color);
        draw_set_alpha(0.7);
        draw_set_font(ft_base);
        draw_circle(xdraw, ydraw, 5, false);
        //++pt_count;
        //draw_text(xorig + 40, yorig - i * 12, string(i) + " (" + );
        
        draw_set_font(ft_small_number);
        
        // x label
        var can_draw = true;
        var current_label = ds_list_find_value(gp.xlabels, i/2);
        // Draw label if no label is near it
        for (var il = 0; il < ds_list_size(drawn_labels_x); ++il) {
            
            var check_label_str = ds_list_find_value(drawn_labels_str, il);
            var l2_x = ds_list_find_value(drawn_labels_x, il);
            var dist_min = (string_width(current_label) + string_width(check_label_str)) / 2 + 10;
        
            if (abs(xdraw - l2_x) <= dist_min) {
                can_draw = false;
                break;
            }
        }
        
        if (can_draw) {
            draw_set_font(ft_small_number);
            draw_set_color(c_black);
            draw_set_alpha(1);
            draw_set_halign(fa_center);
            draw_text(round(xdraw), round(yorig) , current_label);
            ds_list_add(drawn_labels_x, xdraw);
            ds_list_add(drawn_labels_str, current_label);
            draw_line(xdraw, yorig, xdraw, yorig - 6);
        }
        
        // y label
        var can_draw = true;
        var current_label = ds_list_find_value(gp.ylabels, i/2);
        // Draw label if no label is near it
        for (var il = 0; il < ds_list_size(drawn_labels_y); ++il) {
            
            //var check_label_str = ds_list_find_value(drawn_labels_str, il);
            //var l2_x = ds_list_find_value(drawn_labels_x, il);
            //var dist_min = (string_width(current_label) + string_width(check_label_str)) / 2 + 10;
            
            var l2_y = ds_list_find_value(drawn_labels_y, il);
            //show_message(string(ydraw) + string(l2_y));
            if (abs(ydraw - l2_y) <= min_distance_draw_label_y) {
                can_draw = false;
                break;
            }
        }
        
        if (can_draw) {
            draw_set_font(ft_small_number);
            draw_set_color(c_black);
            draw_set_alpha(1);
            draw_set_halign(fa_right);
            draw_set_valign(fa_middle);
            
            draw_text(round(xorig - 4), round(ydraw), current_label);
            ds_list_add(drawn_labels_y, ydraw);
            draw_line(xorig, ydraw, xorig + 6, ydraw);
            
            draw_set_valign(fa_top);
        }
        
    }*/
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

//show_message("drawn pt_count = " + string(pt_count));
ds_list_destroy(drawn_labels_x);
ds_list_destroy(drawn_labels_y);
ds_list_destroy(drawn_labels_str);
draw_set_alpha(1);



/*
var str = "Je suis un poney !    kikou";

var list = split_string(str, " ");

for (var i = 0; i < ds_list_size(list); ++i) {
    draw_text(10, 10 + i * 20, ds_list_find_value(list, i));
}*/


/*
Maintenant je vais essayer de virer les valeurs aberrantes, et il va falloir aussi que je comprenne ces valeurs aberrantes justement, parce que la première exécution est toujours plus longue que les autres (d'un facteur absolument colossal). Je pense faire la médiane des valeurs et supprimer les valeurs "trop grandes" i.e. dont la valeur est au moins x2 la valeur de la médiane.
Je pense que ce qu'on observe pour l'instant avec la courbe décroissante, c'est juste le temps de compilation du kernel / taille du vecteur, et du coup on a probablement la même courbe (i.e. même temps de compilation du kernel) mais plus ou moins aplatie. Ce qui va devenir intéressant je pense, c'est quand je vais avoir supprimé les valeurs associées aux coûts de compilation, et qu'on va pouvoir vraiment voir en régime établi comment ça se comporte !

Demain je ne vais pas pouvoir faire de graphiques parce qu'il faudrait que je configure mon logiciel 
*/

