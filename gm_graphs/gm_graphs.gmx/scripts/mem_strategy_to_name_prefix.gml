/// mem_strategy_to_name(mem strategy id) : string;

var smem = argument0;

switch (smem) {
case 1 : return "a";
case 2 : return "g";
}

return "-nc-";
