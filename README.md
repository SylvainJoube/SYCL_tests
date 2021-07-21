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
ssh -X 192.168.0.169
syncthing
```

Puis depuis un navigateur sur le réseau, 192.168.0.169:33112 compte sylbe pass coucou64.

Définir les variables d'environnement nécessaires à la compilation via syclcc :
```
export HIPSYCL_TARGETS="cuda:sm_35" && \
export HIPSYCL_GPU_ARCH="sm_35" && \
export HIPSYCL_CUDA_PATH="/usr/local/cuda-10.1"
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