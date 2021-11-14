/// split_string(str, delimiter) : list of words

var header = argument0;
var delimiter = argument1;

var word_list = ds_list_create();
var current_word = "";

for (var i = 1; i <= string_length(header); ++i) {
    var char = string_char_at(header, i);
    
    if (ord(char) == 10) continue; // no newline
    
    if (char == delimiter) {
        if (current_word != "") {
            ds_list_add(word_list, current_word);
        }
        current_word = "";
    } else {
        current_word += char;
    }
}

if (current_word != "") {
    ds_list_add(word_list, current_word);
}

return word_list;
