#!/usr/bin/python

# Hello world python program

# Lancer le script : python3 ./2022-02-08_sparseccl.py

import matplotlib.pyplot as plt
import numpy as np
import statistics as stat

## ============ CHARGEMENT ============

#FLAT_ARRAYS = False
FLAT_ONLY = False
tableau_horizontal = True # vertical pas encore géré

# TODO affichage de la figure 4 et 3
# Il faut charger deux fichiers, un flatten et l'autre graphPtr.
# Tout charger dans une même liste (header_list) avec un flag pour indiquer
# s'il s'agit d'un graphe de pointeur ou d'une structure aplatie.
# flag IS_FLATTEN qui indique si c'est flatten ou graphe de ptr


# Lecture du fichier d'entrée
# filepath = 'acts06_generalFlatten_sandor_AT_ld100_RUN1.t'
# filepath = 'acts06_generalFlatten_blopNvidia_AT_ld10_RUN1.t'
# filepath = 'acts06_generalGraphPtr_uniqueModules_sandor_AT_ld1_RUN1.t'
# filepath = 'acts06_generalGraphPtr_uniqueModules_blopNvidia_AT_ld10_RUN1.t'
# Bug manifeste sur le ld10 /!\

def usm_code_to_str(usm_code):
    if (usm_code == 0): return "shared"
    if (usm_code == 1): return "device"
    if (usm_code == 2): return "host"
    if (usm_code == 3): return "accessors"
    if (usm_code == 20): return "CPU"
    return "<unknown>"

# Tous les headers : a chaque header est associé une série de paramètres
header_list = []

def load_file(filepath, isFlatten, multiplyFactor):
    with open(filepath) as fp:
        version = fp.readline() # version du fichier actuel (doit être 105)
        print("Version du fichier : {}".format(version))
        print("isFlatten = " + str(isFlatten))

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
            print("---- SYCL mode = " + str(header["sycl_mode"]) + "----")
            print("SYCL mode = " + usm_code_to_str(header["sycl_mode"]))
            # if (header["sycl_mode"] == 0):  print("SYCL mode = shared")
            # if (header["sycl_mode"] == 1):  print("SYCL mode = device")
            # if (header["sycl_mode"] == 2):  print("SYCL mode = host")
            # if (header["sycl_mode"] == 3):  print("SYCL mode = accessors")
            # if (header["sycl_mode"] == 20): print("SYCL mode = CPU")
            # 0 shared, 1 device, 2 host, 3 accessors, 20 glibc
            # skip 5 champs
            header["mstrat"] = header_split_words[15]
            header["implicit_use_unique_module"] = header_split_words[16]
            header["iterations"] = [] # liste d'itérations

            header["IS_FLATTEN"] = isFlatten # True or False

            #print(header)

            for i in range(int(header["REPEAT_COUNT_REALLOC"])):
                iter_line = fp.readline()
                iter_list = iter_line.split(" ")
                iter_list.remove("\n")
                iteration = {}
                iteration["t_alloc_only"]  = int(iter_list[0]) * multiplyFactor
                iteration["t_fill_only"]   = int(iter_list[3]) * multiplyFactor
                iteration["t_alloc_fill"]  = int(iter_list[8]) * multiplyFactor
                iteration["t_copy_kernel"] = int(iter_list[9]) * multiplyFactor
                iteration["t_read"]        = int(iter_list[10]) * multiplyFactor
                iteration["t_free_mem"]    = int(iter_list[11]) * multiplyFactor
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

# Chargement des résultats en flatten
# load_file('acts06_generalFlatten_blopNvidia_AT_ld100_RUN1.t', True)
load_file('acts06_generalFlatten_sandor_AT_ld100_RUN1.t', True, 1)

# Chargement des résultats en graphe de pointeurs
# load_file('acts06_generalGraphPtr_uniqueModules_blopNvidia_AT_ld10_RUN1.t', False)
load_file('acts06_generalGraphPtr_uniqueModules_sandor_AT_ld1_RUN1.t', False, 100)


## ============ DESSIN ============

# liste de valeurs en x et liste de valeurs en y

# x_list = []
# y_list = []

x_list_shared_flat = []
y_list_shared_flat = []
y_median_shared_flat = []

x_list_shared_ptr = []
y_list_shared_ptr = []
y_median_shared_ptr = []

x_list_host_flat = []
y_list_host_flat = []
y_median_host_flat = []

x_list_host_ptr = []
y_list_host_ptr = []
y_median_host_ptr = []

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
    print("SYCL mode = " + usm_code_to_str(header["sycl_mode"]) + " flatten = " + str(header["IS_FLATTEN"]))
    # print(header["sycl_mode"] + header["IS_FLATTEN"])

    if (header["IS_FLATTEN"]) :
        # shared flatten
        if header["sycl_mode"] == 0:
            x_list = x_list_shared_flat
            y_list = y_list_shared_flat
            y_median = y_median_shared_flat
            found = True

        # device flatten
        if header["sycl_mode"] == 1:
            x_list = x_list_device
            y_list = y_list_device
            y_median = y_median_device
            found = True

        # host flatten
        if header["sycl_mode"] == 2:
            x_list = x_list_host_flat
            y_list = y_list_host_flat
            y_median = y_median_host_flat
            found = True

        # accessors flatten
        if header["sycl_mode"] == 3:
            x_list = x_list_acc
            y_list = y_list_acc
            y_median = y_median_acc
            found = True
        
        # osef glibc (20)
        
    else: # IS_FLATTEN = False

        # shared
        if header["sycl_mode"] == 0:
            x_list = x_list_shared_ptr
            y_list = y_list_shared_ptr
            y_median = y_median_shared_ptr
            found = True

        # host
        if header["sycl_mode"] == 2:
            x_list = x_list_host_ptr
            y_list = y_list_host_ptr
            y_median = y_median_host_ptr
            found = True
        
        # osef glibc (20)
        
    
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


if FLAT_ONLY:
    plt.title("SparseCCL - flat arrays")
    draw_curve("USM device", "green", y_list_device, y_median_device, "solid")
    draw_curve("USM shared", "blue", y_list_shared_flat, y_median_shared_flat, "dotted")
    draw_curve("accessors", "maroon", y_list_acc, y_median_acc, "dashed")
    #draw_curve("USM host", "red", y_list_host_flat, y_median_host_flat, "dashdot")
else:
    plt.title("SparseCCL - pointer graph vs flat arrays")
    #draw_curve("USM device", "green", y_list_device, y_median_device, "solid")
    draw_curve("shared flat", "blue", y_list_shared_flat, y_median_shared_flat, "solid")
    draw_curve("shared ptr", "navy", y_list_shared_ptr, y_median_shared_ptr, "dashdot")
    #draw_curve("host flat", "red", y_list_host_flat, y_median_host_flat, "solid")
    draw_curve("host ptr", "maroon", y_list_host_ptr, y_median_host_ptr, "dashdot")




# ======== Dessin du tableau ======== 


def array_value_to_str(val):
    if (val < 0.01):
        return str(round(val * 1000)/1000)
    elif (val < 0.1):
        return str(round(val * 100)/100)
    elif (val < 1):
        return str(round(val * 10)/10)
    else:
        return str(round(val))

def draw_tab_item(y_median_g):
    st = ""
    for im in range(4): # de 0 à 3 compris
        st = st + array_value_to_str(y_median_g[im])
        if (im != 3):
            st = st + " & "
        else:
            st = st + " \\\\"
    return st

def draw_tab_item_vert(y_median_a, y_median_b, index):
    return (
    array_value_to_str(y_median_a[index]) + " & "
    + array_value_to_str(y_median_b[index]) + " \\\\")

def draw_tab_item_vert3(y_median_a, y_median_b, y_median_c, index):
    return (
    array_value_to_str(y_median_a[index]) + " & "
    + array_value_to_str(y_median_b[index]) + " & "
    + array_value_to_str(y_median_c[index]) + " \\\\" )

# TODO : stocker dans des listes distinctes les cas USM copie explicite et USM accès direct
#        pour pouvoir les réutiliser ensuite dans le graphique.

# Affichage dans le terminal du tableau à mettre dans le LaTeX
def draw_tab():
    print("\\begin{center}")
    if tableau_horizontal:
        print("\\begin{tabular}{||c c c c c||} ")
        print("\\hline")
        # tableau flat seulement
        if (FLAT_ONLY):
            print("& alloc & fill & kernel & free \\\\ [0.5ex]")
            print("\\hline\\hline")
            print("device & " + draw_tab_item(y_median_device))
            print("\\hline")
            print("shared & " + draw_tab_item(y_median_shared_flat))
            print("\\hline")
            print("accessors & " + draw_tab_item(y_median_acc))
        else: # tableau flat vs graphe ptr
            print("& alloc & fill & kernel & free \\\\ [0.5ex]")
            print("\\hline\\hline")
            print("shared flat & " + draw_tab_item(y_median_shared_flat))
            print("\\hline")
            print("shared ptr & " + draw_tab_item(y_median_shared_ptr))

            # nothing yet
    #else:
        # Tableau vertical
        # pas encore géré
        # if (FLAT_ONLY):
        #     # tableau flat seulement
        #     print("\\begin{tabular}{||c c c||} ")
        #     print("\\hline")
        #     print("& s. direct & s. copy \\\\ [0.5ex]")
        #     print("\\hline\\hline")
        #     print("alloc & " + draw_tab_item_vert(y_median_shared_direct, y_median_shared_explicit, 0))
        #     print("\\hline")
        #     print("copy/write & " + draw_tab_item_vert(y_median_shared_direct, y_median_shared_explicit, 1))
        #     print("\\hline")
        #     print("ker\\textsubscript{1} & " + draw_tab_item_vert(y_median_shared_direct, y_median_shared_explicit, 2))
        #     print("\\hline")
        #     print("ker\\textsubscript{2} & " + draw_tab_item_vert(y_median_shared_direct, y_median_shared_explicit, 3))
        #     print("\\hline")
        #     print("free & " + draw_tab_item_vert(y_median_shared_direct, y_median_shared_explicit, 4))
        # else:
        #     # tableau flat vs graphe ptr
        #     print("\\begin{tabular}{||c c c c c||} ")
        #     print("\\hline")
        #     print("& device & shared & host & accessors \\\\ [0.5ex]")
        #     print("\\hline\\hline")
        #     print("alloc & " + draw_tab_item_vert4(y_median_device, y_median_shared_explicit, y_median_host_explicit, y_median_acc, 0))
        #     print("\\hline")
        #     print("copy & " + draw_tab_item_vert4(y_median_device, y_median_shared_explicit, y_median_host_explicit, y_median_acc, 1))
        #     print("\\hline")
        #     print("ker\\textsubscript{1} & " + draw_tab_item_vert4(y_median_device, y_median_shared_explicit, y_median_host_explicit, y_median_acc, 2))
        #     print("\\hline")
        #     print("ker\\textsubscript{2} & " + draw_tab_item_vert4(y_median_device, y_median_shared_explicit, y_median_host_explicit, y_median_acc, 3))
        #     print("\\hline")
        #     print("free & " + draw_tab_item_vert4(y_median_device, y_median_shared_explicit, y_median_host_explicit, y_median_acc, 4))

    print("\\hline")
    print("\\end{tabular}")
    print("\\end{center}")
    print("")


# TODO : relancer un bench sur Sandor (ou ls-cassidi si on y arrive)
# ls-cassidi :
# 2 CPU : Intel(R) Xeon(R) Gold 5218R CPU @ 2.10GHz
# GPU : Quadro RTX A6000
# 187 GiB RAM



# plt.title("SparseCCL - flat arrays")
# #draw_curve("USM device", "green", y_list_device, y_median_device, "solid")
# draw_curve("shared flat", "blue", y_list_shared_flat, y_median_shared_flat, "solid")
# draw_curve("shared ptr", "green", y_list_shared_ptr, y_median_shared_ptr, "dotted")
# #draw_curve("USM host", "maroon", y_list_acc, y_median_acc, "dashed")
# #draw_curve("USM host", "maroon", y_list_host, y_median_host, "dashed")

# Faire l'affichage du tableau de valeurs (x100) et voir ce que ça donne
# par rapport aux précédentes valeurs 

draw_tab()

plt.ylabel('Elapsed time ms')
#plt.ylim([-5, 100])
plt.legend()
plt.xticks([1, 2, 3, 4], x_list_device) # = x_list_shared et x_list_acc
plt.show()
print ("Hello World!")
