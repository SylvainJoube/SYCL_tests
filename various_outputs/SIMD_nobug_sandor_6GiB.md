# Le log complet - 6GiB sur Sandor : pas de bug

**2021-07-29 vers 23h**

```
sylvain@sandor:~/noob/SYCL_tests/mem_bench$ make
syclcc -O2 -std=c++17  -o bin/memory_benchmark_file_output_cmp_all memory_benchmark_file_output_cmp_all.cpp
./bin/memory_benchmark_file_output_cmp_all

current_path     = /home/sylvain/noob/SYCL_tests/mem_bench/
output_file_name = /home/sylvain/noob/SYCL_tests/mem_bench/sandor_simd_6GiB_O2_debug_simd_temp.t
OK, fichier bien ouvert.

============================
   SYCL memory benchmark.   
============================
sandor_simd_6GiB_O2_debug_simd_temp.t

============    - L = VECTOR_SIZE_PER_ITERATION = 128
============    - M = PARALLEL_FOR_SIZE = 12582912
Mode(shared_USM)  SIMD_FOR_LOOP(0)
Generating data...
Input data size  : 6442450944 (6144 MiB)
Output data size : 50331648 (48 MiB)
--   Quadro RTX 5000   --
 -NOT simd- VALID - Right data size ! (123607373)
5%  -NOT simd- VALID - Right data size ! (123607373)
11%  -NOT simd- VALID - Right data size ! (123607373)
16% done.

Mode(device_USM)  SIMD_FOR_LOOP(0)
Input data size  : 6442450944 (6144 MiB)
Output data size : 50331648 (48 MiB)
--   Quadro RTX 5000   --
 -NOT simd- VALID - Right data size ! (123607373)
22%  -NOT simd- VALID - Right data size ! (123607373)
27%  -NOT simd- VALID - Right data size ! (123607373)
33% done.

Mode(host_USM)  SIMD_FOR_LOOP(0)
Input data size  : 6442450944 (6144 MiB)
Output data size : 50331648 (48 MiB)
--   Quadro RTX 5000   --
 -NOT simd- VALID - Right data size ! (123607373)
38%  -NOT simd- VALID - Right data size ! (123607373)
44%  -NOT simd- VALID - Right data size ! (123607373)
50% done.

Mode(shared_USM)  SIMD_FOR_LOOP(1)
Input data size  : 6442450944 (6144 MiB)
Output data size : 50331648 (48 MiB)
--   Quadro RTX 5000   --
 -IS simd- VALID - Right data size ! (123607373)
55%  -IS simd- VALID - Right data size ! (123607373)
61%  -IS simd- VALID - Right data size ! (123607373)
66% done.

Mode(device_USM)  SIMD_FOR_LOOP(1)
Input data size  : 6442450944 (6144 MiB)
Output data size : 50331648 (48 MiB)
--   Quadro RTX 5000   --
 -IS simd- VALID - Right data size ! (123607373)
72%  -IS simd- VALID - Right data size ! (123607373)
77%  -IS simd- VALID - Right data size ! (123607373)
83% done.

Mode(host_USM)  SIMD_FOR_LOOP(1)
Input data size  : 6442450944 (6144 MiB)
Output data size : 50331648 (48 MiB)
--   Quadro RTX 5000   --
 -IS simd- VALID - Right data size ! (123607373)
88%  -IS simd- VALID - Right data size ! (123607373)
94%  -IS simd- VALID - Right data size ! (123607373)
100% done.

OK, done.
```


Avec (même output copié sur les deux fichiers 6Gib et 8GiB):

```
sylvain@sandor:~/noob/SYCL_tests/mem_bench$ nvidia-smi
Thu Jul 29 23:11:25 2021       
+-----------------------------------------------------------------------------+
| NVIDIA-SMI 460.73.01    Driver Version: 460.73.01    CUDA Version: 11.2     |
|-------------------------------+----------------------+----------------------+
| GPU  Name        Persistence-M| Bus-Id        Disp.A | Volatile Uncorr. ECC |
| Fan  Temp  Perf  Pwr:Usage/Cap|         Memory-Usage | GPU-Util  Compute M. |
|                               |                      |               MIG M. |
|===============================+======================+======================|
|   0  Quadro RTX 5000     Off  | 00000000:81:00.0 Off |                  Off |
| 33%   40C    P0    29W / 230W |      0MiB / 16125MiB |      0%      Default |
|                               |                      |                  N/A |
+-------------------------------+----------------------+----------------------+
                                                                               
+-----------------------------------------------------------------------------+
| Processes:                                                                  |
|  GPU   GI   CI        PID   Type   Process name                  GPU Memory |
|        ID   ID                                                   Usage      |
|=============================================================================|
|  No running processes found                                                 |
+-----------------------------------------------------------------------------+
```