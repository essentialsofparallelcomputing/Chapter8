#!/bin/sh

set -v

AVAIL_CPUS=0
LSCPU=$(which lscpu)
if [ -x "${LSCPU}" ]; then
  AVAIL_CPUS=`lscpu |grep '^CPU(s)' |cut -d':' -f2`
fi
NUM_CPUS=144
echo ${AVAIL_CPUS} ${NUM_CPUS}
if [ "${AVAIL_CPUS}" -lt "${NUM_CPUS}" ]; then
  printf "Not enough processors. Need ${NUM_CPUS}\n"
  exit 0
fi

for j in {1..10}; do

   for i in GhostExchange_*
   do
      cd $i/build
      mpirun -n ${NUM_CPUS} --bind-to hwthread ./GhostExchange -x 12 -y 12 -i 20000 -j 20000 -h 2 -t -c
      cd ..
   done
 
   for i in CartExchange_*
   do
      cd $i/build
      mpirun -n ${NUM_CPUS} --bind-to hwthread ./CartExchange -x 12 -y 12 -i 20000 -j 20000 -h 2 -t -c
      cd ..
   done

   for i in GhostExchange3D*
   do
      cd $i/build
      mpirun -n ${NUM_CPUS} --bind-to hwthread ./GhostExchange -x 6 -y 4 -z 6 -i 700 -j 700 -k 700 -h 2 -t -c
      cd ..
   done

   for i in CartExchange3D*
   do
      cd $i/build
      mpirun -n ${NUM_CPUS} --bind-to hwthread ./CartExchange -x 6 -y 4 -z 6 -i 700 -j 700 -k 700 -h 2 -t -c
      cd ..
   done

done
