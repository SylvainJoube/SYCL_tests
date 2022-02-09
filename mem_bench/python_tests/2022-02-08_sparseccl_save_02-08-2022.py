#!/usr/bin/python

# Hello world python program

# Lancer le script : python3 ./2t_datastructures.py

import matplotlib.pyplot as plt
import numpy as np
import statistics as stat

## ============ CHARGEMENT ============

FLAT_ARRAYS = True

# Lecture du fichier d'entrée
filepath = 'acts06_generalFlatten_sandor_AT_ld100_RUN1.t'

# Tous les headers : a chaque header est associé une série de paramètres
header_list = []

with open(filepath) as fp:
    version = fp.readline() # version du fichier actuel (doit être 105)
    print("Version du fichier : {}".format(version))

    header_line = fp.readline()

    while header_line:

        header_split_words = header_line.split(" ")
        header_split_words.remove("\n")
        print(header_split_words)

        header = {} # dictionnaire vide
        header["DATASET_NUMBER"] = header_split_words[0]
        header["in_total_size"] = header_split_words[1]
        header["out_total_size"] = header_split_words[2]
        # skip 2 champs
        header["REPEAT_COUNT_REALLOC"] = header_split_words[5]
        # skip 3 champs
        header["sycl_mode"] = int(header_split_words[9])
        # 0 shared, 1 device, 2 host, 3 accessors, 20 glibc
        # skip 5 champs
        header["mstrat"] = header_split_words[15]
        header["implicit_use_unique_module"] = header_split_words[16]
        header["iterations"] = [] # liste d'itérations

        #print(header)

        for i in range(int(header["REPEAT_COUNT_REALLOC"])):
            iter_line = fp.readline()
            iter_list = iter_line.split(" ")
            iter_list.remove("\n")
            iteration = {}
            iteration["t_alloc_only"]  = int(iter_list[0])
            iteration["t_fill_only"]   = int(iter_list[3])
            iteration["t_alloc_fill"]  = int(iter_list[8])
            iteration["t_copy_kernel"] = int(iter_list[9])
            iteration["t_read"]        = int(iter_list[10])
            iteration["t_free_mem"]    = int(iter_list[11])
            #iteration["t_flatten_alloc"] = iter_list[12] osef, c'est la même chose que t_alloc_only
            #iteration["t_flatten_fill"] = iter_list[13]
            header["iterations"].append(iteration)

            #print("Itération {}:".format(i))
            #print(iteration)

        header_list.append(header)
    
        # Lecture de la prochaine ligne
        header_line = fp.readline()
    # while line fermé
# with file fermé

#print("header_list:")
#print(header_list)


## ============ DESSIN ============

# liste de valeurs en x et liste de valeurs en y

# x_list = []
# y_list = []

x_list_shared = []
y_list_shared = []
y_median_shared = []

x_list_device = []
y_list_device = []
y_median_device = []

x_list_acc = []
y_list_acc = []
y_median_acc = []

divide_by = 1000 # div par 100 seulement pour "simuler" + de données

# 0 shared, 1 device, 2 host, 3 accessors, 20 glibc
# Préparation des données : sélection du run 5 uniquement
for header in header_list:
    found = False
    print(header["sycl_mode"])
    if header["sycl_mode"] == 1:
        x_list = x_list_device
        y_list = y_list_device
        y_median = y_median_device
        found = True

    if header["sycl_mode"] == 0:
        x_list = x_list_shared
        y_list = y_list_shared
        y_median = y_median_shared
        found = True

    if header["sycl_mode"] == 3:
        x_list = x_list_acc
        y_list = y_list_acc
        y_median = y_median_acc
        found = True
    
    # N'est trouvé qu'une seule fois (un seul header correspond)
    if found:
        x_list.append("SYCL alloc")
        x_list.append("fill SYCL mem")
        x_list.append("GPU kernel")
        x_list.append("free SYCL mem")

        y_list.append([]) # t_alloc_only
        y_list.append([]) # t_fill_only
        y_list.append([]) # t_copy_kernel
        y_list.append([]) # t_free_mem
        
        icount = 0
        for iteration in header["iterations"]:
            y_list[0].append(iteration["t_alloc_only"]  / divide_by)
            y_list[1].append(iteration["t_fill_only"]   / divide_by)
            y_list[2].append(iteration["t_copy_kernel"] / divide_by)
            y_list[3].append(iteration["t_free_mem"]    / divide_by)
            icount += 1
        
        # Calcul des médianes de chaque courbe
        y_median.append(stat.median(y_list[0]))
        y_median.append(stat.median(y_list[1]))
        y_median.append(stat.median(y_list[2]))
        y_median.append(stat.median(y_list[3]))


def draw_violin_plot(name, color, y_list, y_median, linestyle):
    c = color
    bp_ = plt.violinplot(y_list, showextrema=False)
    for pc in bp_['bodies']:
        pc.set_facecolor(c)
        pc.set_edgecolor(c) # #D43F3A black
        pc.set_alpha(1)

def draw_boxplot(name, color, y_list, y_median, linestyle):
    c = color
    bp_ = plt.boxplot(y_list,
          notch=False, patch_artist=True,
          boxprops=dict(facecolor=c, color=c),
          capprops=dict(color=c),
          whiskerprops=dict(color=c),
          flierprops=dict(color=c, markeredgecolor=c),
          medianprops=dict(color=c))

def draw_curve(name, color, y_list, y_median, linestyle):
    draw_violin_plot(name, color, y_list, y_median, linestyle)
    plt.plot([1, 2, 3, 4], y_median, color=color, label=name, linestyle=linestyle)

plt.rcParams['grid.linestyle'] = "-"
plt.rcParams['grid.alpha'] = 0.15
plt.rcParams['grid.color'] = "black" ##cccccc
plt.grid()

draw_curve("USM device", "green", y_list_device, y_median_device, "solid")
draw_curve("USM device", "blue", y_list_shared, y_median_shared, "dotted")
draw_curve("USM device", "maroon", y_list_acc, y_median_acc, "dashed")

plt.ylabel('Elapsed time ms')
#plt.ylim([-5, 100])
plt.legend()
plt.xticks([1, 2, 3, 4], x_list_device) # = x_list_shared et x_list_acc
plt.title("SparseCCL - flat arrays")
plt.show()
print ("Hello World!")
