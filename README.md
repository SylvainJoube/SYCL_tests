# SYCL_tests

## Objectif

L'objectif de ce code est de comparer en détail les comportements des différents modèles mémoire de SYCL.


## Lancement des tests depuis mon MSI

Dossier du projet :  
```
cd /home/data_sync/academique/M2/StageM2/SYCL_tests/mem_bench
```

Accès à syncthing :  
```
ssh 192.168.0.169
ou
ssh 192.168.1.170
syncthing
```

Puis depuis un navigateur sur le réseau, 192.168.0.169:33112 compte sylbe pass coucou64.

Définir les variables d'environnement nécessaires à la compilation via syclcc :
```
export HIPSYCL_TARGETS="cuda:sm_35" && \
export HIPSYCL_GPU_ARCH="sm_35" && \
export HIPSYCL_CUDA_PATH="/usr/local/cuda-10.1"
```

Blop Nvidia : (sm_30)
```
export HIPSYCL_TARGETS="cuda:sm_30" && \
export HIPSYCL_GPU_ARCH="sm_30" && \
export HIPSYCL_CUDA_PATH="/usr/local/cuda-10.1"
```

Pour dpcpp :
```
source /opt/intel/oneapi/setvars.sh
```

Depuis Sandor :
```
export HIPSYCL_TARGETS="cuda:sm_75" && \
export HIPSYCL_GPU_ARCH="sm_75" && \
export HIPSYCL_CUDA_PATH="/usr/local/cuda-10.1"
```


## Premier objectif

Pour simplifier les choses, je propose de déjà tout faire uniquement via USM explicite.

Étudier les coûts de migration des données VS quand les données sont déjà sur place
- Lancer des exécutions avec mouvement des données à chaque fois (alloc + free)
- Exécutions sur le même jeu de données (donc avec une seule fois malloc / free pour plusieurs itérations)


Étudier l'impact de la talle du parallel for sur la rapidité de l'exécution :
- Fichier avec, pour chaque ligne : taille du parallel for, taille du vecteur pour une itération, le détail des temps pris
- exécuter sur plusieurs machines et en tirer des graphes (taille parallel_for en x et temps pris en y, avec plusieurs valeurs)
- plus tard, je pourrais essayer de vraiment comprendre ces valeurs si j'ai le temps et que c'est intéressant (probablement)


Étudier l'impact du changement de jeu de données : voir si le kernel est recompilé lors de la soumission d'un nouveau parallel for sur un nouveau jeu de données (probablement pas !).

Comparer le temps de transfert des données avec le débit théorique maximal de mon matériel (déjà écrit dans nvvp, mais à vérifier avec des données constructeur).

## Bug SIMD

- Sur mon MSI avec 1 GiB et Sandor avec 6 GiB pas de souci, les résultats sont valides et cohérents entre CPU et GPU.
- Sur Sandor avec 8 GiB : 
    - Pour tous les non-simd : -NOT simd- ERROR on compute - expected size 535165021 but found 1855233814.
    - Pour tous les SIMD     : -IS simd- VALID - Right data size ! (535165021)

Donc manifestement en fait le souci est avec la boucle for classique, je ne sais pas trop où, ça peut être quelque chose d'intéressant à creuser si j'ai le temps, mais comme j'ai pas le temps ça va juste ne pas être résolu, sauf si ça survient à des moments qui me gènent (genre 1 GiB sur le MSI). Là pour l'instant, pour 1 GiB (MSI, Sandor) comme 6 GiB (Sandor) pas de souci avec la boucle for, donc pas de souci, et surtout pas le temps.


## Pour ne rien casser depuis mon MSI, je hold tant que ça marche

Je n'ai vraiment pas envie qu'une mise à jour me casse quelque chose.

```
apt-mark hold code
sudo apt-mark hold libnvidia*
sudo apt-mark hold hipsycl*
sudo apt-mark hold cuda*
sudo apt-mark hold nvidia*
```

ou plus simplement :
```
apt hold code
apt hold libnvidia*
apt hold hipsycl*
apt hold cuda*
apt hold nvidia*
```

Bon, j'ai un conflit à la con quand je fais apt upgrade, je verrai ça un jour quand j'aurai du temps...


Sur Blop, dpcpp :
```
== List of available devices ==
    Intel(R) Core(TM) i3-4360 CPU @ 3.70GHz (cpu) - score 299
    Intel(R) FPGA Emulation Device (unknown type) - score 74
    SYCL host device (host) - score -1
```

Sur Blop, syclcc :
```
== List of available devices ==
    hipSYCL OpenMP host device (cpu) - score 1
    NVIDIA GeForce GTX 780 (gpu) - score 2
    NVIDIA GeForce GTX 780 (gpu) - score 2
    hipSYCL OpenMP host device (cpu) - score 1
```


[Quel flag sm utiliser ?](https://arnon.dk/matching-sm-architectures-arch-and-gencode-for-various-nvidia-cards/)