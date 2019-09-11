#!/bin/sh

set -v

for (( j=0; j<11; ++j)); do

   for i in GhostExchange_*
   do
      cd $i
      #mpirun -n 176 --bind-to hwthread --mca btl_base_warn_component_unused 0 ./GhostExchange -x 11 -y 16 -i 20000 -j 20000 -h 1 -t
      mpirun -n 176 --bind-to hwthread --mca btl_base_warn_component_unused 0 ./GhostExchange -x 11 -y 16 -i 20000 -j 20000 -h 2 -t
      #mpirun -n 176 --bind-to hwthread --mca btl_base_warn_component_unused 0 ./GhostExchange -x 11 -y 16 -i 20000 -j 20000 -h 2 -t -c
      cd ..
   done

   for i in CartExchange*
   do
      if [ $i != "CartExchange" ]; then
         cd $i
         #mpirun -n 176 --bind-to hwthread --mca btl_base_warn_component_unused 0 ./CartExchange -x 11 -y 16 -i 20000 -j 20000 -h 1 -t
         mpirun -n 176 --bind-to hwthread --mca btl_base_warn_component_unused 0 ./CartExchange -x 11 -y 16 -i 20000 -j 20000 -h 2 -t
         #mpirun -n 176 --bind-to hwthread --mca btl_base_warn_component_unused 0 ./CartExchange -x 11 -y 16 -i 20000 -j 20000 -h 2 -t -c
         cd ..
      fi
   done

done
