/// mem_strategy_to_name(mem strategy id) : string;

var smem = argument0;

switch (smem) {
case 1 : return "aplati";
case 2 : return "graphe pointeurs";
}

return "stratégie mémoire non connue";
