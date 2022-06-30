#!/usr/bin/python

# Hello world python program

# Lancer le script : python3 ./2022-02-03_datastructures.py


# Adapté pour le graphique 1 : microbenchmark version copie explicite

import matplotlib.pyplot as plt
import numpy as np
import statistics as stat

MY_SIZE = 22

plt.rc('font', size=MY_SIZE)          # controls default text sizes
plt.rc('axes', titlesize=MY_SIZE)     # fontsize of the axes title
plt.rc('axes', labelsize=MY_SIZE)     # fontsize of the x and y labels
plt.rc('xtick', labelsize=MY_SIZE)    # fontsize of the tick labels
plt.rc('ytick', labelsize=MY_SIZE)    # fontsize of the tick labels
plt.rc('legend', fontsize=MY_SIZE)    # legend fontsize
plt.rc('figure', titlesize=MY_SIZE)   # fontsize of the figure title

plt.rcParams.update({'font.size': MY_SIZE})

## ============ CHARGEMENT ============

# Lecture du fichier d'entrée
filepath = 'v08_alloc_sandor_ST_6GiB_O2_RUN2.t' #'acts06_generalFlatten_sandor_AT_ld100_RUN1.t'

# Tous les headers : a chaque header est associé une série de paramètres
header_list = []

with open(filepath) as fp:
    version = fp.readline() # version du fichier actuel (doit être 7 pour le microbenchmark)
    print("Version du fichier : {}".format(version))

    header_line = fp.readline()

    while header_line:

        header_split_words = header_line.split(" ")
        # header_split_words.remove("\n") osef du dernier champ de toute façon
        print(header_split_words)

        header = {} # dictionnaire vide
        header["DATASET_NUMBER"] = header_split_words[0]
        header["in_total_size"]  = header_split_words[1]  # = INPUT_DATA_SIZE
        header["out_total_size"] = header_split_words[2] # = OUTPUT_DATA_SIZE
        # skip 2 champs :
        # PARALLEL_FOR_SIZE
        # VECTOR_SIZE_PER_ITERATION
        header["REPEAT_COUNT_REALLOC"] = header_split_words[5]
        header["REPEAT_COUNT_ONLY_PARALLEL"] = header_split_words[6]
        # skip 3 champs
        # REPEAT_COUNT_ONLY_PARALLEL
        # gtimer.t_data_generation_and_ram_allocation
        # gtimer.t_queue_creation
        header["sycl_mode"] = int(header_split_words[9]) # = mode_to_int(mode)
        # sycl_mode : 0 shared, 1 device, 2 host, 3 accessors, 20 glibc
        header["MEMCOPY_IS_SYCL"] = int(header_split_words[10]) # 0 = direct, 1 = copie SYCL explicite
        # skip 4 champs
        # SIMD_FOR_LOOP
        # USE_NAMED_KERNEL
        # USE_HOST_SYCL_BUFFER_DMA
        # REPEAT_COUNT_SUM How many times the sum should be repeated 
        # header["mstrat"] = header_split_words[15]
        # header["implicit_use_unique_module"] = header_split_words[16]
        header["iterations_realloc"] = [] # liste d'itérations
        header["iterations_only_kernel"] = [] # liste d'itérations
        header["iterations_only_kernel_mutualized"] = {} # dictionnaire : alloc, free etc. partagés par les only_kernel

        #print("REPEAT_COUNT_REALLOC : " + header["REPEAT_COUNT_REALLOC"] + "\n")
        #print("REPEAT_COUNT_ONLY_PARALLEL : " + header["REPEAT_COUNT_ONLY_PARALLEL"] + "\n")
        

        # DATASET_NUMBER is supposed to be 1 here.
        # for i in range(int(header["DATASET_NUMBER"])):
        header["DATASET_SEED_realloc"] = int(fp.readline())

        for i in range(int(header["REPEAT_COUNT_REALLOC"])):
            iter_line = fp.readline()
            iter_list = iter_line.split(" ")
            #iter_list.remove("\n") osef
            iteration = {}
            iteration["t_allocation"]       = int(iter_list[0])
            iteration["t_sycl_host_alloc"]  = int(iter_list[1])
            iteration["t_sycl_host_copy"]   = int(iter_list[2])
            iteration["t_copy_to_device"]   = int(iter_list[3])
            iteration["t_sycl_host_free"]   = int(iter_list[4])
            iteration["t_parallel_for"]     = int(iter_list[5])
            iteration["t_read_from_device"] = int(iter_list[6])
            iteration["t_free_gpu"]         = int(iter_list[7])
            header["iterations_realloc"].append(iteration)
        
        # === La partie mutualisée avec uniquement les kernels ne sera pas utilisée ici. ===

        header["DATASET_SEED_only_kernel"] = int(fp.readline())

        for i in range(int(header["REPEAT_COUNT_ONLY_PARALLEL"])):
            iter_line = fp.readline()
            iter_list = iter_line.split(" ")
            #iter_list.remove("\n") osef
            iteration = {}
            iteration["t_parallel_for"]     = int(iter_list[0])
            iteration["t_read_from_device"] = int(iter_list[1])
            header["iterations_only_kernel"].append(iteration)

        mutualized_line = fp.readline()
        mutualized_list = mutualized_line.split(" ")

        header["iterations_only_kernel_mutualized"]["t_allocation"]      = int(mutualized_list[0])
        header["iterations_only_kernel_mutualized"]["t_sycl_host_alloc"] = int(mutualized_list[1])
        header["iterations_only_kernel_mutualized"]["t_sycl_host_copy"]  = int(mutualized_list[2])
        header["iterations_only_kernel_mutualized"]["t_copy_to_device"]  = int(mutualized_list[3])
        header["iterations_only_kernel_mutualized"]["t_sycl_host_free"]  = int(mutualized_list[4])
        header["iterations_only_kernel_mutualized"]["t_free_gpu"]        = int(mutualized_list[5])

        header_list.append(header)

        # Lecture de la prochaine ligne
        header_line = fp.readline()
    # while line fermé
# with file fermé

# #print("header_list:")
# #print(header_list)


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

x_list_host = []
y_list_host = []
y_median_host = []

divide_by = 1000

# 0 shared, 1 device, 2 host, 3 accessors, 20 glibc
# Préparation des données : sélection du run 5 uniquement
for header in header_list:
    found = False
    # print(header["sycl_mode"])

    if header["sycl_mode"] == 0:
        x_list = x_list_shared
        y_list = y_list_shared
        y_median = y_median_shared
        found = True

    if header["sycl_mode"] == 1:
        x_list = x_list_device
        y_list = y_list_device
        y_median = y_median_device
        found = True

    if header["sycl_mode"] == 2:
        x_list = x_list_host
        y_list = y_list_host
        y_median = y_median_host
        found = True

    if header["sycl_mode"] == 3:
        x_list = x_list_acc
        y_list = y_list_acc
        y_median = y_median_acc
        found = True

    # 0 = copie glibc, 1 = copie SYCL explicite
    if header["MEMCOPY_IS_SYCL"] == 0:
        found = False # ignorer ce cas
    
    # N'est trouvé qu'une seule fois (un seul header correspond)
    if found:

        x_list.append("alloc")
        x_list.append("copy/access")
        x_list.append("kernel 1")
        x_list.append("kernel 2")
        x_list.append("free")

        # x_list.append("SYCL alloc") # SYCL alloc
        # x_list.append("copy/access to SYCL mem") # fill SYCL mem
        # x_list.append("GPU kernel 1")
        # x_list.append("GPU kernel 2")
        # x_list.append("free SYCL mem")

        y_list.append([]) # t_allocation
        y_list.append([]) # t_copy_to_device
        y_list.append([]) # t_parallel_for
        y_list.append([]) # t_read_from_device
        y_list.append([]) # t_free_gpu
        
        icount = 0
        for iteration in header["iterations_realloc"]:
            y_list[0].append(iteration["t_allocation"]  / divide_by)
            y_list[1].append(iteration["t_copy_to_device"]   / divide_by)
            y_list[2].append(iteration["t_parallel_for"] / divide_by)
            y_list[3].append(iteration["t_read_from_device"]    / divide_by)
            y_list[4].append(iteration["t_free_gpu"]    / divide_by)
            icount += 1
        
        print("SYCL MODE = " + str(header["sycl_mode"]))
        # Calcul des médianes de chaque courbe
        for im in range(5): # de 0 à 4 compris
            y_median.append(stat.median(y_list[im]))
            # print("im : " + str(im) + " : " + str(stat.median(y_list[im])))

def draw_tab_item(y_median_g):
    st = ""
    for im in range(5): # de 0 à 4 compris
        val = y_median_g[im]
        if (val < 0.01):
            st = st + str(round(val * 1000)/1000)
        elif (val < 0.1):
            st = st + str(round(val * 100)/100)
        elif (val < 1):
            st = st + str(round(val * 10)/10)
        else:
            st = st + str(round(y_median_g[im]))
        if (im != 4):
            st = st + " & "
        else:
            st = st + " \\\\"
    return st

# Affichage dans le terminal du tableau à mettre dans le LaTeX
def draw_tab():
    print("\\begin{center}")
    print("\\begin{tabular}{||c c c c c c||} ")
    print("\\hline")
    print("& alloc & copy/access & kernel 1 & kernel 2 & free \\\\ [0.5ex]")
    print("\\hline\\hline")
    # print("device & " + str(y_median_device[0]) + " & " + str(round(y_median_device[1])) + " & " + str(y_median_device[2]) + " & " + str(y_median_device[3]) + " & " + str(y_median_device[4]) + " \\\\")
    print("device & " + draw_tab_item(y_median_device))
    print("\\hline")
    print("shared & " + draw_tab_item(y_median_shared))
    print("\\hline")
    print("host & " + draw_tab_item(y_median_host))
    print("\\hline")
    print("accessors & " + draw_tab_item(y_median_acc))
    print("\\hline")
    print("\\end{tabular}")
    print("\\end{center}")
    print("")

# \begin{center}
# \begin{tabular}{||c c c c c c||} 
#     \hline
#     & alloc & copy/access & kernel 1 & kernel 2 & free \\ [0.5ex] 
#     \hline\hline
#     device & 6 & 87837 & 787 & a & a \\ 
#     \hline
#     shared & 7 & 78 & 5415 & a & a \\
#     \hline
#     host & 545 & 778 & 7507 & a & a \\
#     \hline
#     accessors & 545 & 18744 & 7560 & a & a \\ [1ex] 
#     \hline
# \end{tabular}
# \end{center}

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
    plt.plot([1, 2, 3, 4, 5], y_median, color=color, label=name, linestyle=linestyle)


# TODO : virer les valeurs trop aberrantes

#plt.grid(True)
plt.rcParams['grid.linestyle'] = "-"
plt.rcParams['grid.alpha'] = 0.15
plt.rcParams['grid.color'] = "black" ##cccccc
plt.grid()
# plt.rcParams.update({'font.size': 22})

SMALL_SIZE = 14
MEDIUM_SIZE = 14
BIGGER_SIZE = 14

# for item in ([plt.title, plt.xaxis.label, plt.yaxis.label] + plt.get_xticklabels() + plt.get_yticklabels()):
#     item.set_fontsize(20)


draw_curve("USM device", "green", y_list_device, y_median_device, "solid")
draw_curve("USM shared", "blue", y_list_shared, y_median_shared, "dotted")
draw_curve("USM host", "red", y_list_host, y_median_host, "dashed")
draw_curve("accessors", "maroon", y_list_acc, y_median_acc, "dashdot")

# y_median_device
# plt.plot(x_list_shared, y_list_shared, color='blue', label='USM shared')
# plt.plot(x_list_acc, y_list_acc, color='maroon', label='buffers')
plt.ylabel('Elapsed time ms')
#plt.ylim([-5, 100])
plt.legend()
plt.xticks([1, 2, 3, 4, 5], x_list_device) # = x_list_shared et x_list_acc
plt.title("SparseCCL - flat arrays")





# print("y_median_device len = " + str(len(y_median_device)) + "  en x : " + str(len([1, 2, 3, 4, 5])))


# colors = ['green', 'blue', 'maroon']
 
# for patch in bp_device['boxes']:
#     patch.set_facecolor('green')

#plt.xticks([1, 2, 3], ['mon', 'tue', 'wed'])

print ("Hello World!")

draw_tab()


plt.show()
# line = fp.readline()
# # Lecture du header
# cnt = 1
# while line:

#     print("Line {}: {}".format(cnt, line.strip()))
#     line = fp.readline()
#     cnt += 1

# plt.plot([1, 4, 9, 160])
# plt.ylabel('some numbers')
# plt.show()