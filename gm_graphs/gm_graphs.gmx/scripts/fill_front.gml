/// fill_front(str, wanted str len, fill char) : padded str;

var str = argument0;
var wanted_len = argument1;
var fill_char = argument2;

var prefix = "";
var fill_repeat = wanted_len - string_length(str);
for (var i = 0; i < fill_repeat; ++i) {
    prefix += fill_char;
}

return prefix + str;
