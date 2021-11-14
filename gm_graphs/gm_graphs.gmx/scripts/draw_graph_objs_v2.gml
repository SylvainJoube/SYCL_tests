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

var graph_yoffset = g_graph_yoffset;
var graph_label_ystart = g_graph_label_ystart;

var origin_arrow_size = g_origin_arrow_size;
var origin_arrow_size_secondary = 0;//ceil(g_origin_arrow_size);

var zero_lines_alpha = 0.2;
var secondary_arrows_alpha = 0.2;

var graph_height = g_graph_height;
var graph_width = g_graph_width;

// surface height = 

var xorig = g_xorig;
var yorig = g_yorig;
var xend = xorig + graph_width;

var yarrow_reduce = 8;

// Draw the origin lines
draw_set_color(c_black);
draw_set_alpha(1);
draw_arrow(xorig, yorig, xorig + graph_width, yorig, origin_arrow_size);
draw_arrow(xorig, yorig, xorig, yorig - graph_height + yarrow_reduce, origin_arrow_size);

draw_set_alpha(zero_lines_alpha);
var x_orig_and_offset = xorig + x_space_left;
var y_orig_and_offset = yorig - y_space_left;
draw_line(x_orig_and_offset, y_orig_and_offset, x_orig_and_offset + graph_width - x_space_left, y_orig_and_offset);
draw_line(x_orig_and_offset, y_orig_and_offset, x_orig_and_offset, y_orig_and_offset - graph_height + y_space_left + yarrow_reduce);

draw_set_alpha(1);
draw_set_halign(fa_center);
draw_set_font(ft_base);

// Draw labels
draw_text(xorig + floor(graph_width / 2), yorig + 40, xlabel);
draw_text_transformed(10, yorig - floor(graph_height / 2) - 10, ylabel, 1, 1, 90);

// Draw graph title
var xcenter_surface = floor((graph_width + xorig * 2) / 2);
draw_text(xcenter_surface, 20, g_graph_display_name);

// Draw L and M, if their values are shared
draw_set_halign(fa_right);
var dstr = "";

draw_set_font(ft_LM);
if (g_display_LM) {
    dstr =
      split_thousands(g_VECTOR_SIZE_PER_ITERATION_common) + " = L" + chr(10)
      + split_thousands(g_PARALLEL_FOR_SIZE_common) + " = M" + chr(10)
      //+ split_thousands(g_iteration_count) + " pts" + chr(10)
      + "in " + split_thousands(round(g_input_data_size / 1024)) + " kio" + chr(10)
      + "out " + split_thousands(round(g_output_data_size / 1024)) + " kio";
} else {
    dstr = "" //split_thousands(g_iteration_count) + " pts" + chr(10)
      + "in " + split_thousands(round(g_input_data_size / 1024)) + " kio" + chr(10)
      + "out " + split_thousands(round(g_output_data_size / 1024)) + " kio";
}

if (g_display_REPEAT_COUNT_SUM && (g_REPEAT_COUNT_SUM_common != -1)) {
    dstr += chr(10) + string(g_REPEAT_COUNT_SUM_common) + " accès";
}        


draw_text(graph_width + xorig - x_space_left + 100, 60, dstr);
draw_set_font(ft_base);


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

// Je veux avoir le ymin et ymax pour chaque x
// méthode super bourrin mais rapide : objet instancié ayant un x et n ymin et ymax
// sachant que je m'en fiche de libérer la mémoire après, c'est une opération ponctuelle
// et le logiciel se termine après sans avoir à refaire les opérations.

var xgroup_fscale_list = ds_list_create();

//xgroup_fscale

// g_xgroup_has_own_scale

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

/*
- inventaire de tous les points
- classement selon leur coordonnée xx
- création d'autant de listes xgroup que nécessaire
  les listes seront crées dans l'ordre des x croissants
- donc à x donné j'ai le ymin et ymax
*/

var scale_all_points = ds_list_create();
var scale_all_distincts_xx = ds_list_create();
var scale_global_xgroups = ds_list_create();
var scale_global_xgroup = ds_list_create();
// si besoin, remplacer pt.xx par pt.xlabel si offset artificiel
// mais à voir si ça casse pas l'ordre d'affichage (ce qui serait ballot)
// pour chaque courbe
for (var igp = 0; igp < ds_list_size(graph_list); ++igp) {
    var gp = ds_list_find_value(graph_list, igp);
    // pour chaque point de la courbe
    for (var ipt = 0; ipt < ds_list_size(gp.points); ++ipt) {
        var pt = ds_list_find_value(gp.points, ipt);
        ds_list_add(scale_all_points, pt);
        // Si la coordonnée xx n'existe pas, je l'ajoute
        if (ds_list_find_index(scale_all_distincts_xx, pt.xx) == -1) {
            ds_list_add(scale_all_distincts_xx, pt.xx);
        }
    }
}
// classement des valeurs de xx croissantes
ds_list_sort(scale_all_distincts_xx, true);

for (var ixx = 0; ixx < ds_list_size(scale_all_distincts_xx); ++ixx) {
    var xgg = instance_create(0, 0, global_xgroup);
    xgg.xx = ds_list_find_value(scale_all_distincts_xx, ixx);
    xgg.points = ds_list_create();
    //var same_xx = ds_list_find_value(scale_all_distincts_xx, ixx);
    //var l_same_xx = ds_list_create();
    ds_list_add(scale_global_xgroups, xgg);
    // création de la liste des points au même xx
    for (var ipt = 0; ipt < ds_list_size(scale_all_points); ++ipt) {
        var pt = ds_list_find_value(scale_all_points, ipt);
        if (pt == -1) continue;
        if (pt.xx != xgg.xx) continue;
        ds_list_add(xgg.points, pt);
        ds_list_replace(scale_all_points, ipt, -1);
    }
}
// global_xgroup

// calcul du ymin et ymax pour chaque xgroup global
// scale_global_xgroups et scale_all_distincts_xx
// classées par ordre xx croissant
for (var ixx = 0; ixx < ds_list_size(scale_all_distincts_xx); ++ixx) {
    var xgg = ds_list_find_value(scale_global_xgroups, ixx);
    var local_ymin = 0;
    var local_ymax = 0;
    for (var ipt = 0; ipt < ds_list_size(xgg.points); ++ipt) {
        var pt = ds_list_find_value(xgg.points, ipt);
        if (ipt == 0) {
            local_ymin = pt.yy;
            local_ymax = pt.yy;
        }
        if (pt.yy < local_ymin) local_ymin = pt.yy;
        if (pt.yy > local_ymax) local_ymax = pt.yy;
    }
    if (ymin_impose != -1) local_ymin = ymin_impose;
    if (ymax_impose != -1) local_ymax = ymax_impose;
    xgg.ymin = local_ymin;
    xgg.ymax = local_ymax;
    xgg.scale_height = xgg.ymax - xgg.ymin + 1;
    xgg.scale_height_factor = (graph_height - y_space_left * 2) / xgg.scale_height;
}





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



// split_thousands
// liste des médianes avec couleur, ylabel, ydraw, xorig
// liste des x des médianes (x, liste des médianes à cet x)
//   copie de la liste des médianes
//   remplissage de la liste des médianes (et remplacement à -1)
// classement par ordre y croissant à x fixé
// dessin des nombres associés



var median_list = ds_list_create();
var median_list_ydraw_sorted = ds_list_create();
var median_xgroup_list =  ds_list_create();
// graph_median

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
        
        // par défaut : échelle partagée globale
        var local_scale_height_factor = scale_height_factor;
        var local_ymin = ymin;
        
        // Si chaque xgroup global a sa propre échelle
        if (g_xgroup_has_own_scale) {
            var xgg;
            var found_xgg = false;
            // find the associated global xgroup
            for (var ixgg = 0; ixgg < ds_list_size(scale_global_xgroups); ++ixgg) {
                xgg = ds_list_find_value(scale_global_xgroups, ixgg);
                if (xgg.xx == xgroup.xx) {
                    found_xgg = true;
                    break;
                }
            }
            if (! found_xgg ) {
                show_message("ERROR draw_graph_objs_v2 : found_xgg == false");
            }
            local_scale_height_factor = xgg.scale_height_factor;
            local_ymin = xgg.ymin;
            
        }
        
        // y min and max for this xgroup
        var xgroup_ymin = -1;
        var xgroup_ymax = -1;
        
        // xdraw on surface
        // xorig : on screen x origin
        // (xgroup.xx - xmin) : xgroup x position relative to first drawn x
        // * scale_width_factor : puton scale, retavive to min and max x
        // x_space_left : static offset
        var xdraw = xorig + (xgroup.xx - xmin) * scale_width_factor + x_space_left;
        // xgroup.xx equals every pt.xx of this xgroup.
        
        var local_xorig = xdraw - x_space_left;
        
        if (g_multiple_xaxis && (xgroup.xx != 0) ) {
            draw_set_color(merge_colour(c_white, c_black, secondary_arrows_alpha));
            draw_set_alpha(1);
            draw_arrow(local_xorig, yorig - 1, local_xorig, yorig - graph_height + yarrow_reduce, origin_arrow_size_secondary);
            draw_set_alpha(1);
        }
        
        var median_y_sum = 0;
        var median_y_sum_value = 0;
        var median_color = c_black;
        
        // classer les positions en ydraw croissant
        // pour chaque ydraw (donc en ordre croissant)
        // retrouver le point associé et dessiner dès que possible ses infos
        // dessiner un trait si la position n'est pas dispo (dessiner au y compatible
        // le plus petit possible) 
        var ydraw_sorted = ds_list_create();
        var points_copy = ds_list_create();
        
        // For each point of an xgroup, draw the line
        for (var i = 0; i < ds_list_size(xgroup.points); ++i) {
            var pt = ds_list_find_value(xgroup.points, i);
            median_color = pt.color;
            
            // ydraw on surface
            // yorig : on screen y origin
            // (pt.yy - ymin) : point y position relative to first drawn y
            // * scale_height_factor : put on scale, retavive to min and max y
            // x_space_left : static offset
            // 
            var ydraw = yorig - (pt.yy - local_ymin) * local_scale_height_factor - y_space_left;
            ds_list_add(ydraw_sorted, ydraw);
            ds_list_add(points_copy, pt);
            pt.temp_ydraw = ydraw; // pour le retrouver facilemet
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
            median_y_sum_value += pt.yy;
            
            // Draw y label, if possible
            /*var can_draw = true;
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
            }*/
            
        }
        
        /* DESORMAIS AFFICHAGE MEDIANNE UNIQUEMENT
        var next_ydraw = y_orig_and_offset;
        ds_list_sort(ydraw_sorted, true);
        for (var iyd = 0; iyd < ds_list_size(ydraw_sorted); ++iyd) {
            var ydraw = ds_list_find_value(ydraw_sorted, iyd);
            var original_ydraw = ydraw;
            
            var pt;
            var found_pt = false;
            for (var ipt = 0; ipt < ds_list_size(points_copy); ++ipt) {
                pt = ds_list_find_value(points_copy, ipt);
                if (pt == -1) continue;
                if (pt.temp_ydraw == ydraw) {
                    found_pt = true;
                    ds_list_replace(points_copy, ipt, -1);
                    break;
                }
            }
            if ( ! found_pt ) {
                show_message("ERROR draw_graph_objs_v2 : not found_pt");
            }
            
            
            var xtext_draw = local_xorig - 6;
            var xline1_draw = local_xorig;
            var yline1_draw = local_xorig;
            // Si mon ydraw actuel est trop grand
            if (next_ydraw < ydraw) {
                // reculer en x de quelques pixels
                // tracer un trait de (local_xorig, ydraw) à (local_xorig - xpx, next_ydraw)
                // afficher ce qu'il y a à afficher en fa_right
                ydraw = next_ydraw;
            }
            next_ydraw = ydraw - min_distance_draw_label_y;
            
            draw_set_font(ft_small_number_ylabel);
            //draw_set_color(c_black);
            draw_set_color(pt.color);
            draw_set_alpha(1);
            draw_set_halign(fa_right);
            draw_set_valign(fa_middle);
            
            var text_xoffset = 8;
            
            draw_text(round(local_xorig - text_xoffset), round(ydraw), pt.ylabel);
            // ligne droite à droite de l'axe origine
            draw_line(local_xorig, original_ydraw, local_xorig + 6, original_ydraw);
            // ligne menant de l'axe origine au texte
            draw_line(local_xorig - (text_xoffset - 1), ydraw, local_xorig, original_ydraw);
            
            draw_set_valign(fa_top);
            
            //points_copy
        } */
        
        
        // Draw maximum and minimum y for this xgroup
        draw_line_width(xdraw - line_width_div2_max, xgroup_ymin, xdraw + line_width_div2_max, xgroup_ymin, line_height_max);
        draw_line_width(xdraw - line_width_div2_max, xgroup_ymax, xdraw + line_width_div2_max, xgroup_ymax, line_height_max);
        draw_line_width(xdraw, xgroup_ymin, xdraw, xgroup_ymax, line_width_div2_vertical);
        
        // emulate a median
        var median_ydraw = (xgroup_ymin + xgroup_ymax) / 2;
        if (ds_list_size(xgroup.points) != 0) {
            //median_ydraw = round(median_y_sum / ds_list_size(xgroup.points));
            var med_value = round(median_y_sum_value / ds_list_size(xgroup.points));
            var med_ydraw = yorig - (med_value - local_ymin) * local_scale_height_factor - y_space_left;
            median_ydraw = med_ydraw;
            
            var med = instance_create(0, 0, graph_single_point_median);
            med.xdraw = xdraw;
            med.ydraw = med_ydraw;
            // Ajouter le nom de la courbe devant le nombre seulement s'il y a plus d'une seule courbe
            if ( ds_list_size(graph_list) >= 2) {
                med.ylabel = string(gp.dataset_index) + " ";
            } else {
                med.ylabel = "";
            }
            med.ylabel += split_thousands(round( med_value / 1) ); // en millisecondes plutôt qu'en us
            med.color = median_color;
            if ( g_multiple_xaxis ) {
                med.xorig = local_xorig;
            } else {
                med.xorig = xorig;
            }
            ds_list_add(median_list, med);
            
            var xgroup_xaxis_position;
            if ( g_multiple_xaxis ) {
                xgroup_xlabel_position = med.xdraw;
            } else {
                xgroup_xlabel_position = xorig + x_space_left;
            }
            
            // Ajout de la médiane au groupe x
            var mxgroup;
            var found_mxgroup = false;
            for (var img = 0; img < ds_list_size(median_xgroup_list); ++img) {
                var mxgroup = ds_list_find_value(median_xgroup_list, img);
                if (mxgroup.xdraw == xgroup_xlabel_position) { // med.xdraw
                    ds_list_add(mxgroup.medians, med);
                    ds_list_add(mxgroup.medians_ysorted, med.ydraw);
                    found_mxgroup = true;
                    break;
                }
            }
            // Création du groupe x si inexistant
            if ( ! found_mxgroup ) {
                var mxgroup = instance_create(0, 0, median_xgroup);
                ds_list_add(median_xgroup_list, mxgroup);
                mxgroup.xdraw = xgroup_xlabel_position; // med.xdraw;
                mxgroup.medians = ds_list_create();
                mxgroup.medians_ysorted = ds_list_create();
                ds_list_add(mxgroup.medians, med);
                ds_list_add(mxgroup.medians_ysorted, med.ydraw);
                
            }
            ds_list_add(median_list_ydraw_sorted, med.ydraw);
        }
        
        
        // Dessin des liens entre les paquets de points regroupés en xgroup
        // pour une courbe donnée
        if ( (gp_xdraw_old != -1) ) {
            draw_set_alpha(0.4);
            draw_line_width(gp_xdraw_old, gp_ydraw_old, xdraw, median_ydraw, g_line_pts_link_width);
        }
        
        gp_xdraw_old = xdraw;
        gp_ydraw_old = median_ydraw;
        
        
        draw_set_font(ft_very_small_number);
        draw_set_alpha(1);
        draw_set_halign(fa_center);
        var sh = string_height(gp.name);
        //draw_text(round(xdraw), round(xgroup_ymin - sh + 1), gp.name + " d(" + string(xgroup.deleted_strange_points) + ")");
        // dernier en date : draw_text(round(xdraw), round(xgroup_ymin - sh + 2), string(gp.dataset_index));
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
}

// Dessin des labels

//g_multiple_xaxis
for (var imxg = 0; imxg < ds_list_size(median_xgroup_list); ++imxg) {
    
    var mxgroup = ds_list_find_value(median_xgroup_list, imxg);    

    var next_ydraw = y_orig_and_offset;
    
    var next_y_draw_pos = y_orig_and_offset;
    
    var last_ydraw;
    var last_median_ydraw;
    var median_list_ydraw_sorted = mxgroup.medians_ysorted;
    
    ds_list_sort(mxgroup.medians_ysorted, false);
    
    var ydsize = ds_list_size(mxgroup.medians_ysorted);
    
    for (var iyd = 0; iyd < ydsize; ++iyd) {
        var ydraw = ds_list_find_value(mxgroup.medians_ysorted, iyd);
        var original_ydraw = ydraw;
        
        // Si un seul axe global, limiter le nombre de labels dessinés
        // Affichage obligatoire de la première et de la dernière valeur (minimal et maximal)
        if ( ( ! g_multiple_xaxis ) && (iyd != 0) && (iyd != ydsize - 1) ) {
            // Ne pas dessiner le label si le dernier point est distant de
            // moins de g_same_xgroup_min_label_distance pixels.
            // On est assuré qu'aucun point n'est plus proche dans le sens des y décroissants.
            // (car liste classée en fonction des y)
            if ( last_median_ydraw - ydraw < g_same_xgroup_min_label_distance) {
                //last_median_ydraw = ydraw;
                continue;
            } 
        }
        last_median_ydraw = ydraw;
        
        if (iyd != 0) {
            if (last_ydraw - ydraw < min_distance_draw_label_y) {
                ydraw = last_ydraw - min_distance_draw_label_y; // au minimum 20 px d'écart
            }
        }
        last_ydraw = ydraw;
        
        // Je trouve la médiane correspondant à cette valeur de ydraw
        var med;
        var found_pt = false;
        for (var ipt = 0; ipt < ds_list_size(mxgroup.medians); ++ipt) {
            med = ds_list_find_value(mxgroup.medians, ipt);
            if (med == -1) continue;
            if (med.ydraw == original_ydraw) {
                found_pt = true;
                ds_list_replace(mxgroup.medians, ipt, -1);
                break;
            }
        }
        if ( ! found_pt ) {
            show_message("ERROR draw_graph_objs_v2 : not found_pt on median");
        }
        
        /*if ( ydraw > next_y_draw_pos ) {
            ydraw = next_y_draw_pos;
        }
        next_y_draw_pos = ydraw - 20;*/
        
        //var xtext_draw = med.xorig - 6;
        // Si mon ydraw actuel est trop grand
        /*if (ydraw > next_ydraw) {
            // reculer en x de quelques pixels
            // tracer un trait de (local_xorig, ydraw) à (local_xorig - xpx, next_ydraw)
            // afficher ce qu'il y a à afficher en fa_right
            //ydraw = next_ydraw;
        }
        next_ydraw = ydraw - min_distance_draw_label_y;*/
        
        draw_set_font(ft_small_number_ylabel);
        //draw_set_color(c_black);
        draw_set_color(med.color);
        draw_set_alpha(1);
        draw_set_halign(fa_right);
        draw_set_valign(fa_middle);
        
        var text_xoffset = 16;
        
        //ydraw = original_ydraw;
        
        draw_text(round(med.xorig - text_xoffset), round(ydraw), med.ylabel);
        // ligne droite à droite de l'axe origine
        draw_line(med.xorig, original_ydraw, med.xorig + 6, original_ydraw);
        
        //draw_line(med.xorig, ydraw, med.xorig - 6, ydraw);
        // ligne menant de l'axe origine au texte
        draw_line(med.xorig - (text_xoffset - 1), ydraw, med.xorig, original_ydraw);
        
        // Dessin d'une ligne pour guider le regard, seulement si un seul axe en x
        if ( ! g_multiple_xaxis ) {
            draw_set_alpha(0.12);
            draw_line(med.xorig, original_ydraw, xend, original_ydraw);
            draw_set_alpha(1);
        }
        
        draw_set_valign(fa_top);
        
        //points_copy
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

//show_message("drawn pt_count = " + string(pt_count));
ds_list_destroy(drawn_labels_x);
ds_list_destroy(drawn_labels_y);
ds_list_destroy(drawn_labels_str);
draw_set_alpha(1);
