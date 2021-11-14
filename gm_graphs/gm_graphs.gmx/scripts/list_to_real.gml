/// list_to_real(list) : list of parsed reals

var string_list = argument0;
var real_list = ds_list_create();

for (var i = 0; i < ds_list_size(string_list); ++i) {
    var word = ds_list_find_value(string_list, i);
    var r = real(word);
    ds_list_add(real_list, r);
    //show_message("at pos " + string(i) + " |" + word + "| -> " + string(r));
}

return real_list;
