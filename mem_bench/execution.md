# Execution / build

## Sur Thinkpad

```
cd /home/data_sync/academique/These/SYCL_tests/mem_bench && \
make traccc_acat
```


## Sur Blop

```
export HIPSYCL_TARGETS="cuda:sm_35" && \
export HIPSYCL_GPU_ARCH="sm_35" && \
export HIPSYCL_CUDA_PATH="/usr/local/cuda-10.1"
```

```
cd /home/data_sync/academique/These/SYCL_tests/mem_bench && \
make traccc_acat
```

## Sur Sandor

```
cd ~/noob/SYCL_tests/mem_bench

export HIPSYCL_TARGETS="cuda:sm_75" && \
export HIPSYCL_GPU_ARCH="sm_75" && \
export HIPSYCL_CUDA_PATH="/usr/local/cuda-10.1"
```

**Execution** :
-   ./bin/bench mem_test_6GB
-   ./bin/bench mem_test_XGB 1

 start_test_index = argv[2];
            std::string stop_test_index  = argv[3];
            std::string run_count        = argv[4];
            std::string ld_repeat        = argv[5];

- ./bin/bench traccc_acat start_test_index stop_test_index run_count ld_repeat ubench_run_count
- exemple  ./bin/bench traccc_acat 1 1 1 10 0