/// draw_graph_objs(graph_list, x_space_left, y_space_left, xlabel, ylabel, ymin, ymax);

var graph_list  = argument0;
var list = graph_list;
var x_space_left = argument1;
var y_space_left = argument2;
var xlabel = argument3;
var ylabel = argument4;
var ymin_impose = argument5;
var ymax_impose = argument6;

var xmin = 0;
var ymin = 0;
var xmax = 0;
var ymax = 0;

var graph_height = 700;
var graph_width = 800;

var xorig = 100;
var yorig = graph_height + 100;

// Draw the origin lines
draw_set_color(c_black);
draw_set_alpha(1);
draw_line(xorig, yorig, xorig + graph_width, yorig);
draw_line(xorig, yorig, xorig, yorig - graph_height);

draw_set_halign(fa_center);
draw_set_font(ft_base);

// Draw labels
draw_text(xorig + floor(graph_width / 2), yorig + 30, xlabel);
draw_text_transformed(xorig - 66, yorig - floor(graph_height / 2), ylabel, 1, 1, 90);

// Draw graph title
var xcenter_surface = floor((graph_width + xorig * 2) / 2);
draw_text(xcenter_surface, 20, g_graph_title);

var xdraw_plabel = 500,
var ydraw_plabel = 120;

// Draw the points labels
for (var igp = 0; igp < ds_list_size(graph_list); ++igp) {
    var gp = ds_list_find_value(graph_list, igp);
    draw_set_halign(fa_left);
    draw_set_valign(fa_center);
    draw_set_font(ft_point_label);
    draw_set_color(gp.color);
    var str_height = string_height(gp.name);
    draw_circle(xdraw_plabel - 20, ydraw_plabel, 5, false);
    draw_set_color(c_black);
    draw_text(xdraw_plabel, ydraw_plabel, gp.name);
    ydraw_plabel += str_height + 10;
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

var scale_width  = xmax - xmin + 1;
var scale_height = ymax - ymin + 1;

var scale_width_factor  = (graph_width - x_space_left * 2) / scale_width;
var scale_height_factor = (graph_height - y_space_left * 2) / scale_height;

var drawn_labels_y = ds_list_create();
var drawn_labels_x = ds_list_create();
var drawn_labels_str = ds_list_create();
var min_distance_draw_label_y = 30;

var line_width_div2 = 5;
var line_width_div2_max = 10;
var line_height = 1;
var line_height_max = 2;
var line_width_div2_vertical = 1;


// Draw the points
for (var igp = 0; igp < ds_list_size(graph_list); ++igp) {
    var gp = ds_list_find_value(graph_list, igp);
    var list = gp.points;
    
    // Draw all points from an xgroup
    var ymin = -1;
    var ymax = -1;
    
    
    // For each xgroup
    for (var ixg = 0; ixg < ds_list_size(gp.xgroups); ++ixg) {
        var xgroup = ds_list_find_value(gp.xgroups, ixg);
        
        var xdraw = xorig + (xgroup.xx - xmin) * scale_width_factor + x_space_left;
        // xgroup.xx equals every pt.xx of this xgroup.
        
        // For each point of an xgroup, draw the line
        for (var i = 0; i < ds_list_size(xgroup.points); ++i) {
            var pt = ds_list_find_value(xgroup.points, i);
            //var xx = pt.xx; //ds_list_find_value(list, i);
            //var yy = pt.yy; //ds_list_find_value(list, i + 1);
            var ydraw = yorig - (pt.yy - ymin) * scale_height_factor - y_space_left;
            draw_set_color(gp.color);
            draw_set_alpha(1);
            draw_line_width(xdraw - line_width_div2, ydraw, xdraw + line_width_div2, ydraw, line_height);
            if (i == 0) {
                ymin = ydraw;
                ymax = ydraw;
            }
            if (ymin > ydraw) ymin = ydraw;
            if (ymax < ydraw) ymax = ydraw;
            //draw_circle(xdraw, ydraw, 5, false);
            
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
            
        }
        draw_line_width(xdraw - line_width_div2_max, ymin, xdraw + line_width_div2_max, ymin, line_height_max);
        draw_line_width(xdraw - line_width_div2_max, ymax, xdraw + line_width_div2_max, ymax, line_height_max);
        draw_line_width(xdraw, ymin, xdraw, ymax, line_width_div2_vertical);
        
        draw_set_font(ft_very_small_number);
        draw_set_alpha(1);
        draw_set_halign(fa_center);
        var sh = string_height(gp.name);
        draw_text(round(xdraw), round(ymin - sh + 1), gp.name);
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


