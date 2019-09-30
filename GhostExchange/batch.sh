#!/bin/sh

set -v

for (( j=0; j<11; ++j)); do

   for i in GhostExchange_*
   do
      cd $i
      mpirun -n 144 --bind-to hwthread ./GhostExchange -x 12 -y 12 -i 20000 -j 20000 -h 2 -t -c
      cd ..
   done
 
   for i in CartExchange_*
   do
      cd $i
      mpirun -n 144 --bind-to hwthread ./CartExchange -x 12 -y 12 -i 20000 -j 20000 -h 2 -t -c
      cd ..
   done

   for i in GhostExchange3D*
   do
      cd $i
      mpirun -n 144 --bind-to hwthread ./GhostExchange -x 6 -y 4 -z 6 -i 700 -j 700 -k 700 -h 2 -t -c
      cd ..
   done

   for i in CartExchange3D*
   do
      cd $i
      mpirun -n 144 --bind-to hwthread ./CartExchange -x 6 -y 4 -z 6 -i 700 -j 700 -k 700 -h 2 -t -c
      cd ..
   done

done
