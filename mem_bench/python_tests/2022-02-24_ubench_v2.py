#!/usr/bin/python

# Hello world python program

# Lancer le script : python3 ./2022-02-08_sparseccl.py

import matplotlib.pyplot as plt
import numpy as np
import statistics as stat

## ============ CHARGEMENT ============

#FLAT_ARRAYS = False
FLAT_ONLY = True
tableau_horizontal = True # vertical pas encore géré

VERSION_ATTENDUE = 2

# TODO affichage de la figure 4 et 3
# Il faut charger deux fichiers, un flatten et l'autre graphPtr.
# Tout charger dans une même liste (header_list) avec un flag pour indiquer
# s'il s'agit d'un graphe de pointeur ou d'une structure aplatie.
# flag IS_FLATTEN qui indique si c'est flatten ou graphe de ptr


# ============= Gestion de la taille
my_dpi = 96
output_image_name = "le nom de ma belle image"
output_image_name_ver = "4_br" # 3_dl

image_width = 1280
image_height = 1
image_scale_factor = image_width / 640
line_width = image_scale_factor * 1.5

if FLAT_ONLY:
    image_height = (image_width / 640) * 160 #198
    plt.figure(figsize=(image_width/my_dpi, image_height/my_dpi) , dpi=my_dpi)
    output_image_name = "ubench2_onlyFlat" + output_image_name_ver + ".png"
else:
    image_height = (image_width / 640) * 258 # 273
    plt.figure(figsize=(image_width/my_dpi, image_height/my_dpi), dpi=my_dpi)
    output_image_name = "ubench2_ptrAndFlat" + output_image_name_ver + ".png"

MY_SIZE = (10 * image_scale_factor)
TITLE_SIZE = (12 * image_scale_factor)

#plt.rc('font', size=MY_SIZE)          # controls default text sizes
plt.rc('axes', titlesize=TITLE_SIZE)     # fontsize of the axes title
plt.rc('axes', labelsize=MY_SIZE)     # fontsize of the x and y labels
plt.rc('xtick', labelsize=MY_SIZE)    # fontsize of the tick labels
plt.rc('ytick', labelsize=MY_SIZE)    # fontsize of the tick labels
plt.rc('legend', fontsize=MY_SIZE)    # legend fontsize
#plt.rc('figure', titlesize=MY_SIZE)   # fontsize of the figure title
# plt.rcParams.update({'font.size': MY_SIZE})

# fin gestion de la taille =============

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

import sys

global_kernel_count = 0

global_kernel_retain = 2

def load_file(filename, isFlatten, multiplyFactor):
    global global_kernel_count
    global global_kernel_retain
    global VERSION_ATTENDUE

    absolute_filepath = "/home/data_sync/academique/These/SYCL_tests/mem_bench/output_bench/" + filename

    with open(absolute_filepath) as fp:
        version = fp.readline() # version du fichier actuel (doit être 106 et non plus 105)
        print("Version du fichier : {}".format(version))

        if (int(version) != VERSION_ATTENDUE):
            #print("ERREUR, VERSION DU FICHIER NON COMPATIBLE : " + str(int(version)) + ".  VERSION ATTENDUE = 106")
            sys.exit("ERREUR, VERSION DU FICHIER NON COMPATIBLE : " + str(int(version)) + ".  VERSION ATTENDUE = " + str(VERSION_ATTENDUE))

        print("isFlatten = " + str(isFlatten))

        header_line = fp.readline()

        while header_line:

            header_split_words = header_line.split(" ")
            header_split_words.remove("\n")
            print(header_split_words)

            header = {} # dictionnaire vide
            header["in_total_size"] = header_split_words[0]
            header["out_total_size"] = header_split_words[1]
            header["b_INPUT_OUTPUT_FACTOR"] = header_split_words[2]
            header["REPEAT_COUNT_REALLOC"] = header_split_words[3]
            header["sycl_mode"] = int(header_split_words[4])
            header["explicit_copy"] = int(header_split_words[5]) # 1 pour oui, 0 pour non

            print("---- SYCL mode = " + str(header["sycl_mode"]) + "----")
            print("SYCL mode = " + usm_code_to_str(header["sycl_mode"]))
            # if (header["sycl_mode"] == 0):  print("SYCL mode = shared")
            # if (header["sycl_mode"] == 1):  print("SYCL mode = device")
            # if (header["sycl_mode"] == 2):  print("SYCL mode = host")
            # if (header["sycl_mode"] == 3):  print("SYCL mode = accessors")
            # if (header["sycl_mode"] == 20): print("SYCL mode = CPU")
            # 0 shared, 1 device, 2 host, 3 accessors, 20 glibc
            # skip 5 champs
            header["iterations"] = [] # liste d'itérations

            # header["IS_FLATTEN"] = isFlatten # True or False

            #print(header)

            # print(" _____ REPEAT_COUNT_REALLOC = " + header["REPEAT_COUNT_REALLOC"])

            for i in range(int(header["REPEAT_COUNT_REALLOC"])):
                iter_line = fp.readline()
                iter_list = iter_line.split(" ")
                iter_list.remove("\n")
                iteration = {}
                iteration["t_alloc_native"]  = int(iter_list[0]) * multiplyFactor
                iteration["t_alloc_sycl"]  = int(iter_list[1]) * multiplyFactor
                iteration["t_fill"]  = int(iter_list[2]) * multiplyFactor
                iteration["t_copy"]  = int(iter_list[3]) * multiplyFactor
                iteration["t_read"]  = int(iter_list[4]) * multiplyFactor
                iteration["t_dealloc_sycl"]  = int(iter_list[5]) * multiplyFactor
                iteration["t_dealloc_native"]  = int(iter_list[6]) * multiplyFactor
                iteration["kernel_count"]  = int(iter_list[7])

                
                iteration["t_kernel"] = [] # liste de valeurs temps kernel
                global_kernel_count = int(iteration["kernel_count"])
                # print(" _____ kernel_count = " + iter_list[7])
                # print(" _____ global_kernel_count = " + str(global_kernel_count))
                for ik in range(global_kernel_retain):
                    v = int(iter_list[8 + ik]) * multiplyFactor
                    #if (ik <= 1):
                    iteration["t_kernel"].append(v)

                # iteration["t_alloc_only"]  = int(iter_list[0]) * multiplyFactor
                # iteration["t_fill_only"]   = int(iter_list[3]) * multiplyFactor
                # iteration["t_alloc_fill"]  = int(iter_list[8]) * multiplyFactor
                # iteration["t_copy_kernel"] = int(iter_list[9]) * multiplyFactor
                # iteration["t_read"]        = int(iter_list[10]) * multiplyFactor
                # iteration["t_free_mem"]    = int(iter_list[11]) * multiplyFactor
                #iteration["t_flatten_alloc"] = iter_list[12] osef, c'est la même chose que t_alloc_only
                #iteration["t_flatten_fill"] = iter_list[13]
                header["iterations"].append(iteration)

                #print("Itération {}:".format(i))
                #print(iteration)

            header_list.append(header)
            # print(" __ end header ___ global_kernel_count = " + str(global_kernel_count))
        
            # Lecture de la prochaine ligne
            header_line = fp.readline()
        # while line fermé
    # with file fermé
    # print(" ________ FNALLY :  global_kernel_count = " + str(global_kernel_count))

#print("header_list:")
#print(header_list)

# Chargement des résultats
load_file('ubench2_2_sandor_6GiB_RUN1.t', True, 1)

print(" ________ AFTER LOAD :  global_kernel_count = " + str(global_kernel_count))

global_drawn_x_variables_number = 6 + global_kernel_retain

# Chargement des résultats en graphe de pointeurs
# load_file('acts06_generalGraphPtr_uniqueModules_blopNvidia_AT_ld10_RUN1.t', False)
#   load_file('acts06_generalGraphPtr_uniqueModules_sandor_AT_ld100_RUN1.t', False, 1)


## ============ DESSIN ============

# liste de valeurs en x et liste de valeurs en y

# x_list = []
# y_list = []

x_list_shared_direct = []
y_list_shared_direct = []
y_median_shared_direct = []

x_list_shared_copy = []
y_list_shared_copy = []
y_median_shared_copy = []

x_list_host_direct = []
y_list_host_direct = []
y_median_host_direct = []

x_list_host_copy = []
y_list_host_copy = []
y_median_host_copy = []

x_list_device = []
y_list_device = []
y_median_device = []

x_list_glibc = []
y_list_glibc = []
y_median_glibc = []

x_list_acc = []
y_list_acc = []
y_median_acc = []

divide_by = 1000 # div par 100 seulement pour "simuler" + de données

def nz(value):
    # if (value < 0):
    #     return 0
    return value

# 0 shared, 1 device, 2 host, 3 accessors, 20 glibc
# Préparation des données : sélection du run 5 uniquement
for header in header_list:
    found = False
    print("SYCL mode = " + usm_code_to_str(header["sycl_mode"]) + " explicit_copy = " + str(header["explicit_copy"]))
    
    # print(header["sycl_mode"] + header["IS_FLATTEN"])
    
    # accessors
    if header["sycl_mode"] == 3:
        x_list = x_list_acc
        y_list = y_list_acc
        y_median = y_median_acc
        found = True

    # glibc
    if header["sycl_mode"] == 20:
        x_list = x_list_glibc
        y_list = y_list_glibc
        y_median = y_median_glibc
        found = True

    # device
    if header["sycl_mode"] == 1:
        x_list = x_list_device
        y_list = y_list_device
        y_median = y_median_device
        found = True

    # Copie explicite
    if (header["explicit_copy"] == 1) :

        # shared copie explicite
        if header["sycl_mode"] == 0:
            x_list = x_list_shared_copy
            y_list = y_list_shared_copy
            y_median = y_median_shared_copy
            found = True

        # host copie explicite
        if header["sycl_mode"] == 2:
            x_list = x_list_host_copy
            y_list = y_list_host_copy
            y_median = y_median_host_copy
            found = True
        
    else: # copie implicite

        # shared copie implicite
        if header["sycl_mode"] == 0:
            x_list = x_list_shared_direct
            y_list = y_list_shared_direct
            y_median = y_median_shared_direct
            found = True

        # host copie implicite
        if header["sycl_mode"] == 2:
            x_list = x_list_host_direct
            y_list = y_list_host_direct
            y_median = y_median_host_direct
            found = True
        
    
    # N'est trouvé qu'une seule fois (un seul header correspond)
    if found:

        x_list.append("native-a")
        x_list.append("sycl-a")
        x_list.append("fill")
        x_list.append("copy")
        for ik in range(global_kernel_retain):
            x_list.append("ker" + str(ik + 1))
        x_list.append("sycl-d")
        x_list.append("native-d")

        for ips in range(len(x_list)):
            y_list.append([])
        

        icount = 0
        for iteration in header["iterations"]:
            
            y_list[0].append(nz(iteration["t_alloc_native"])  / divide_by)
            y_list[1].append(nz(iteration["t_alloc_sycl"])    / divide_by)
            y_list[2].append(nz(iteration["t_fill"])          / divide_by)
            y_list[3].append(nz(iteration["t_copy"])          / divide_by)
            # read osef
            # CONTINUER D'ICI

            ker_list = iteration["t_kernel"]
            
            # normalement len(ker_list) == global_kernel_retain
            if (len(ker_list) != global_kernel_retain):
                sys.exit("--- ERREUR, len(ker_list)(" + str(len(ker_list)) + ") != global_kernel_retain(" + str(global_kernel_retain) + ")")

            # Ajout des temps de kernel
            for ik in range(global_kernel_retain):
                y_list[4 + ik].append(nz(ker_list[ik]          / divide_by))
                #y_list[4 + ik].append(nz(iteration["ker" + str(ik)]    / divide_by))

            ik = 4 + global_kernel_retain
            y_list[ik].append(nz(iteration["t_dealloc_sycl"])       / divide_by)
            y_list[ik + 1].append(nz(iteration["t_dealloc_native"])     / divide_by)

            # y_list[0].append(iteration["t_alloc_only"]  / divide_by)
            # y_list[1].append(iteration["t_fill_only"]   / divide_by)
            # y_list[2].append(iteration["t_copy_kernel"] / divide_by)
            # y_list[3].append(iteration["t_free_mem"]    / divide_by)
            icount += 1
        
        # Calcul des médianes de chaque courbe
        for ii in range(len(x_list)):
            y_median.append(stat.median(y_list[ii]))

        # y_median.append(stat.median(y_list[1]))
        # y_median.append(stat.median(y_list[2]))
        # y_median.append(stat.median(y_list[3]))


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
    plt.plot(range(1, global_drawn_x_variables_number+1), y_median, color=color, label=name, linestyle=linestyle, linewidth=line_width)

plt.rcParams['grid.linestyle'] = "-"
plt.rcParams['grid.alpha'] = 0.15
plt.rcParams['grid.color'] = "black" ##cccccc
plt.grid(linewidth=line_width/2)


if FLAT_ONLY:
    #plt.title("Microbenchmark - accessors & USM device & shared/host direct")
    plt.title("Microbenchmark - accessors & USM device & shared/host direct")
    draw_curve("USM device", "green", y_list_device, y_median_device, "solid")

    # draw_curve("USM shared copy", "blue", y_list_shared_copy, y_median_shared_copy, "dotted")
    # draw_curve("USM host copy", "red", y_list_host_copy, y_median_host_copy, "dashdot")

    draw_curve("USM shared direct", "blue", y_list_shared_direct, y_median_shared_direct, "dotted")
    draw_curve("USM host direct", "red", y_list_host_direct, y_median_host_direct, "dashdot")

    draw_curve("accessors", "maroon", y_list_acc, y_median_acc, "dashed")




    # draw_curve("USM shared direct", "blue", y_list_shared_direct, y_median_shared_direct, "solid")
    # draw_curve("USM host direct", "red", y_list_host_direct, y_median_host_direct, "solid")
else:
    plt.title("SparseCCL - pointer graph vs flat arrays")
    #draw_curve("USM device", "green", y_list_device, y_median_device, "solid")
    draw_curve("shared flat", "blue", y_list_shared_flat, y_median_shared_flat, "solid")
    print("len y_list_shared_flat = " + str(len(y_list_shared_flat)))
    print("len y_median_shared_flat = " + str(len(y_median_shared_flat)))
    print("len y_list_shared_ptr = " + str(len(y_list_shared_ptr)))
    print("len y_median_shared_ptr = " + str(len(y_median_shared_ptr)))

    print("len y_list_host_ptr = " + str(len(y_list_host_ptr)))
    print("len y_median_host_ptr = " + str(len(y_median_host_ptr)))
    draw_curve("shared ptr", "navy", y_list_shared_ptr, y_median_shared_ptr, "dashdot")
    #draw_curve("host flat", "red", y_list_host_flat, y_median_host_flat, "solid")
    draw_curve("host ptr", "maroon", y_list_host_ptr, y_median_host_ptr, "dashdot")




# ======== Dessin du tableau ======== 


def array_value_to_str(val):
    if (val < 0):
        return "-"
    if (val < 0.01):
        return str(round(val * 1000)/1000)
    elif (val < 0.1):
        return str(round(val * 100)/100)
    elif (val < 1):
        return str(round(val * 10)/10)
    else:
        return str(round(val))

# Ancienne version
# def draw_tab_item(y_median_g):
#     st = ""
#     su = 0
#     for im in range(4): # de 0 à 3 compris
#         st = st + array_value_to_str(y_median_g[im])
#         su += round(y_median_g[im])
#         if (im != 3):
#             st = st + " & "
#         else:
#             st = st + " & " + array_value_to_str(su) + " \\\\"
#     return st

def draw_tab_item(cname, y_median_g):
    st = cname + " & "
    ssum = 0
    # Les premiers champs + 2 kernels
    for im in range(global_drawn_x_variables_number): # de 0 à 5 compris
        ssum += round(y_median_g[im])
        st = st + array_value_to_str(y_median_g[im])
        if (im != global_drawn_x_variables_number-1):
            st = st + " & "
    # skip de 6 et 7, deux kernels
    # dealloc sycl + dealloc host
    # for im in range(8, 10): # de 8 à 9 compris
    #     ssum += round(y_median_g[im])
    #     st = st + array_value_to_str(y_median_g[im])
    #     if (im != global_drawn_x_variables_number-1):
    #         st = st + " & "

        #else: # affichage du total et fin de ligne
    
    sum_str = array_value_to_str(ssum)
    st = st + " & " + sum_str + " & " + cname + " \\\\"
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
        print("\\begin{tabular}{||c c c c c c c c c c c||} ")
        print("\\hline")
        # tableau flat seulement
        if (FLAT_ONLY):
            print("& native-a & sycl-a & fill & copy & ker\\textsubscript{1} & ker\\textsubscript{2} & sycl-d & native-d & total & \\\\ [0.5ex]")
            print("\\hline\\hline")
            print(draw_tab_item("device", y_median_device))
            print("\\hline")
            print(draw_tab_item("accessors", y_median_acc))
            print("\\hline")
            print(draw_tab_item("shared\\_c", y_median_shared_copy))
            print("\\hline")
            print(draw_tab_item("shared\\_d", y_median_shared_direct))
            print("\\hline")
            print(draw_tab_item("host\\_c", y_median_host_copy))
            print("\\hline")
            print(draw_tab_item("host\\_d", y_median_host_direct))
        else: # tableau flat vs graphe ptr
            print("& alloc & fill & kernel & dealloc & total & \\\\ [0.5ex]")
            print("\\hline\\hline")
            # Bien penser à vérifier que les valeurs sont les mêmes partout dans la publi
            print(draw_tab_item("shared flat", y_median_shared_flat))
            print("\\hline")
            print(draw_tab_item("shared ptr", y_median_shared_ptr))
            print("\\hline")
            print("TODO : mettre host flat ici")
            print("\\hline")
            # Host flat à remplacer par la valeur déjà dans le papier
            #print(draw_tab_item("host flat", y_median_host_flat))
            #print("\\hline")
            print(draw_tab_item("host ptr", y_median_host_ptr))
            print("\\hline")
            print(draw_tab_item("cpu flat", y_median_glibc_flat))
            print("\\hline")
            print(draw_tab_item("cpu ptr", y_median_glibc_ptr))

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

# if FLAT_ONLY:
#     plt.figure(figsize=(800/my_dpi, 800/my_dpi), dpi=my_dpi)
# else:
#     plt.figure(figsize=(800/my_dpi, 800/my_dpi), dpi=my_dpi)

draw_tab()

plt.ylabel('Elapsed time (ms)')
#plt.ylim([-5, 100])
plt.legend()
plt.xticks(range(1, global_drawn_x_variables_number+1), x_list_device) # = x_list_shared et x_list_acc

plt.savefig(output_image_name, format='png') #, dpi=my_dpi)

plt.show()
print ("Hello World!")
