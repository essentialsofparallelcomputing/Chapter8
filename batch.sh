#!/bin/sh

set -v

for i in GhostExchange*
do
   cd $i
   mpirun -n 176 --bind-to hwthread --mca btl_base_warn_component_unused 0 ./GhostExchange -x 11 -y 16 -i 20000 -j 20000 -h 1
   mpirun -n 176 --bind-to hwthread --mca btl_base_warn_component_unused 0 ./GhostExchange -x 11 -y 16 -i 20000 -j 20000 -h 2
   mpirun -n 176 --bind-to hwthread --mca btl_base_warn_component_unused 0 ./GhostExchange -x 11 -y 16 -i 20000 -j 20000 -h 2 -c
   cd ..
done

for i in CartExchange*
do
   cd $i
   mpirun -n 176 --bind-to hwthread --mca btl_base_warn_component_unused 0 ./CartExchange -x 11 -y 16 -i 20000 -j 20000 -h 1
   mpirun -n 176 --bind-to hwthread --mca btl_base_warn_component_unused 0 ./CartExchange -x 11 -y 16 -i 20000 -j 20000 -h 2
   mpirun -n 176 --bind-to hwthread --mca btl_base_warn_component_unused 0 ./CartExchange -x 11 -y 16 -i 20000 -j 20000 -h 2 -c
   cd ..
done

