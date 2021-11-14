/// dFileExists(fname) : boolean;

var fname = argument0;

external_call(dll_dFileExists, 0, 0);

var strl = string_length(fname);

for (var i = 1; i <= strl; ++i) {
    external_call(dll_dFileExists, 1, ord(string_char_at(fname, i)));
}

return external_call(dll_dFileExists, 2, 0);
