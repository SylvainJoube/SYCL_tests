#!/bin/bash
for i in {1..20}
do
   #echo "Welcome $i times"
   python3 ./2022-02-08_microbench.py $i
done