/// mem_strategy_to_name(mem strategy id) : string;

var smem = argument0;

switch (smem) {
case 1 : return "graphe pointeurs";
case 2 : return "aplati";
}

return "stratégie mémoire non connue";
