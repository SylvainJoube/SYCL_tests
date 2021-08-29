/// error_add(error_text, level);

var level = argument1;

// 10 pas important
// 0 à afficher forcément
if (level < 2) {
    
    ds_list_add(g_display_error_list, argument0);
}
