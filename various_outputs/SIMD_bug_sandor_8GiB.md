# Le log complet - 8GiB sur Sandor : un bug sur la boucle for classique

**2021-07-29 vers 23h**

```
sylvain@sandor:~/noob/SYCL_tests/mem_bench$ make
syclcc -O2 -std=c++17  -o bin/memory_benchmark_file_output_cmp_all memory_benchmark_file_output_cmp_all.cpp
./bin/memory_benchmark_file_output_cmp_all

current_path     = /home/sylvain/noob/SYCL_tests/mem_bench/
output_file_name = /home/sylvain/noob/SYCL_tests/mem_bench/sandor_simd_8GiB_O2_debug_simd_temp.t
OK, fichier bien ouvert.

============================
   SYCL memory benchmark.   
============================
sandor_simd_8GiB_O2_debug_simd_temp.t

============    - L = VECTOR_SIZE_PER_ITERATION = 128
============    - M = PARALLEL_FOR_SIZE = 16777216
Mode(shared_USM)  SIMD_FOR_LOOP(0)
Generating data...
Input data size  : 8589934592 (8192 MiB)
Output data size : 67108864 (64 MiB)
--   Quadro RTX 5000   --
 -NOT simd- ERROR on compute - expected size 535165021 but found 1855233814.
5%  -NOT simd- ERROR on compute - expected size 535165021 but found 1855233814.
11%  -NOT simd- ERROR on compute - expected size 535165021 but found 1855233814.
16% done.

Mode(device_USM)  SIMD_FOR_LOOP(0)
Input data size  : 8589934592 (8192 MiB)
Output data size : 67108864 (64 MiB)
--   Quadro RTX 5000   --
 -NOT simd- ERROR on compute - expected size 535165021 but found 1855233814.
22%  -NOT simd- ERROR on compute - expected size 535165021 but found 1855233814.
27%  -NOT simd- ERROR on compute - expected size 535165021 but found 1855233814.
33% done.

Mode(host_USM)  SIMD_FOR_LOOP(0)
Input data size  : 8589934592 (8192 MiB)
Output data size : 67108864 (64 MiB)
--   Quadro RTX 5000   --
 -NOT simd- ERROR on compute - expected size 535165021 but found 1855233814.
38%  -NOT simd- ERROR on compute - expected size 535165021 but found 1855233814.
44%  -NOT simd- ERROR on compute - expected size 535165021 but found 1855233814.
50% done.

Mode(shared_USM)  SIMD_FOR_LOOP(1)
Input data size  : 8589934592 (8192 MiB)
Output data size : 67108864 (64 MiB)
--   Quadro RTX 5000   --
 -IS simd- VALID - Right data size ! (535165021)
55%  -IS simd- VALID - Right data size ! (535165021)
61%  -IS simd- VALID - Right data size ! (535165021)
66% done.

Mode(device_USM)  SIMD_FOR_LOOP(1)
Input data size  : 8589934592 (8192 MiB)
Output data size : 67108864 (64 MiB)
--   Quadro RTX 5000   --
 -IS simd- VALID - Right data size ! (535165021)
72%  -IS simd- VALID - Right data size ! (535165021)
77%  -IS simd- VALID - Right data size ! (535165021)
83% done.

Mode(host_USM)  SIMD_FOR_LOOP(1)
Input data size  : 8589934592 (8192 MiB)
Output data size : 67108864 (64 MiB)
--   Quadro RTX 5000   --
 -IS simd- VALID - Right data size ! (535165021)
88%  -IS simd- VALID - Right data size ! (535165021)
94%  -IS simd- VALID - Right data size ! (535165021)
100% done.

OK, done.
```


Avec (même output copiée sur les deux fichiers 6Gib et 8GiB):

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