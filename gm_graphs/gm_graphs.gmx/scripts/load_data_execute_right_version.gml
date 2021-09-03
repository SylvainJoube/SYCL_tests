/// load_data_execute_right_version(version, file);

var version = argument0;
var file = argument1;

var konwn_version = false;

if (version == 2) { load_data_v2(file); konwn_version = true; }
if (version == 3) { load_data_v3(file); konwn_version = true; }
if (version == 4) { load_data_v4(file) konwn_version = true; };
if (version == 5) { load_data_v5(file); konwn_version = true; }
if (version == 6) { load_data_v6(file); konwn_version = true; }
if (version == 7) { load_data_v7(file); konwn_version = true; }

// erruer de parcours, v100 idem que v102
if ( (version == 100) || (version == 102) ) { 
    load_data_v100(file);
    konwn_version = true;
    
}

if (version == 103) { load_data_v103(file); konwn_version = true; }


if ( ! konwn_version ) {
    show_message("load_data_execute_right_version : "
    + "version du fichier non prise en charge. "
    + "version(" + string(version) + ")");
}

