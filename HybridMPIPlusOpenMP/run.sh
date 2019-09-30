#!/bin/sh

set -v

export OMP_NUM_THREADS=2
mpirun -n 44 --bind-to cores --mca btl_base_warn_component_unused 0 ./CartExchange -x 12 -y 12 -i 20000 -j 20000 -h 2 -t -c
