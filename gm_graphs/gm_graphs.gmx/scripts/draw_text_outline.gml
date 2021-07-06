/// draw_text_outline(x, y, text, outline_color);

var xx = argument0;
var yy = argument1;
var text = argument2;
var outline_color = argument3;

var col = draw_get_color();
draw_set_color(outline_color);
draw_text(xx-1, yy-1, text);
draw_text(xx-1, yy+1, text);
draw_text(xx+1, yy-1, text);
draw_text(xx+1, yy+1, text);

draw_set_color(col);
draw_text(xx, yy, text);
