#!/usr/bin/python

# Hello world python program

# Lancer le script : python3 ./2t_datastructures.py

import matplotlib.pyplot as plt
import numpy as np
import statistics as stat

## ============ CHARGEMENT ============

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

divide_by = 1000

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
        iteration5 = header["iterations"][5]
        x_list.append("SYCL alloc")
        x_list.append("fill SYCL mem")
        x_list.append("GPU kernel")
        x_list.append("free SYCL mem")

        # y_list.append(iteration5["t_alloc_only"]  / divide_by)
        # y_list.append(iteration5["t_fill_only"]   / divide_by)
        # y_list.append(iteration5["t_copy_kernel"] / divide_by)
        # y_list.append(iteration5["t_free_mem"]    / divide_by)

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

# x_list.append(1)
# x_list.append(2)
# x_list.append(3)
# x_list.append(4)

# y_list.append(10)
# y_list.append(20)
# y_list.append(30)
# y_list.append(40)


# Creating dataset
# np.random.seed(10)
 
# data_1 = np.random.normal(100, 10, 200)
# data_2 = np.random.normal(90, 20, 200)
# data_3 = np.random.normal(80, 30, 200)
# data_4 = np.random.normal(70, 40, 200)
# data = [[1, 2, 3, 4, 5, 6, 7], data_2, data_3, data_4]

# # Creating plot
# bp = plt.boxplot(data)
 
# # show plot
# plt.show()

c = "green"
bp_device = plt.boxplot(y_list_device,
            notch=False, patch_artist=True,
            boxprops=dict(facecolor=c, color=c),
            capprops=dict(color=c),
            whiskerprops=dict(color=c),
            flierprops=dict(color=c, markeredgecolor=c),
            medianprops=dict(color=c))

# bp_acc_violin = plt.violinplot(y_list_device,
#                         showextrema=False,
#                         )
# for pc in bp_acc_violin['bodies']:
#     pc.set_facecolor(c)
#     pc.set_edgecolor(c) # #D43F3A black
#     pc.set_alpha(1)

plt.plot([1, 2, 3, 4], y_median_device, color='green', label='USM device')

c = "blue"
bp_shared = plt.boxplot(y_list_shared,
            notch=False, patch_artist=True,
            boxprops=dict(facecolor=c, color=c),
            capprops=dict(color=c),
            whiskerprops=dict(color=c),
            flierprops=dict(color=c, markeredgecolor=c),
            medianprops=dict(color=c))

# bp_acc_violin = plt.violinplot(y_list_shared,
#                         showextrema=False,
#                         )
# for pc in bp_acc_violin['bodies']:
#     pc.set_facecolor(c)
#     pc.set_edgecolor(c) # #D43F3A black
#     pc.set_alpha(1)

plt.plot([1, 2, 3, 4], y_median_shared, color='blue', label='USM shared')


c = "maroon"
bp_acc = plt.boxplot(y_list_acc,
            notch=False, patch_artist=True,
            boxprops=dict(facecolor=c, color=c),
            capprops=dict(color=c),
            whiskerprops=dict(color=c),
            flierprops=dict(color=c, markeredgecolor=c),
             medianprops=dict(color=c))

# bp_acc_violin = plt.violinplot(y_list_acc,
#                         showextrema=False,
#                         )
# for pc in bp_acc_violin['bodies']:
#     pc.set_facecolor(c)
#     pc.set_edgecolor(c) # #D43F3A black
#     pc.set_alpha(1)

plt.plot([1, 2, 3, 4], y_median_acc, color='maroon', label='buffers')



# colors = ['green', 'blue', 'maroon']
 
# for patch in bp_device['boxes']:
#     patch.set_facecolor('green')

# y_median_device
# plt.plot(x_list_shared, y_list_shared, color='blue', label='USM shared')
# plt.plot(x_list_acc, y_list_acc, color='maroon', label='buffers')
plt.ylabel('Elapsed time ms')
#plt.ylim([-5, 100])
plt.legend()
plt.xticks([1, 2, 3, 4], x_list_device) # = x_list_shared et x_list_acc
plt.title("SparseCCL - flat arrays")

#plt.xticks([1, 2, 3], ['mon', 'tue', 'wed'])

plt.show()
# line = fp.readline()
# # Lecture du header
# cnt = 1
# while line:

#     print("Line {}: {}".format(cnt, line.strip()))
#     line = fp.readline()
#     cnt += 1

print ("Hello World!")

# plt.plot([1, 4, 9, 160])
# plt.ylabel('some numbers')
# plt.show()