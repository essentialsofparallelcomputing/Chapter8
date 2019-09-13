#!/bin/sh
for i in GhostExchange_* CartExchange_* GhostExchange3D* CartExchange3D*
do
   echo $i
   echo -n "Median: "; grep Timing results.txt | grep $i | cut -f11 -d' ' | sort -n | awk '{arr[NR]=$1}
      END { if (NR%2==1) print arr[(NR+1)/2]; else print (arr[NR/2]+arr[NR/2+1])/2}'
   grep Timing results.txt | grep $i | cut -f11 -d' ' | tr '\n' ' ' |awk 'NF {sum=0;ssq=0;for (i=1;i<=NF;i++){sum+=$i;ssq+=$i**2}; print "Mean " sum/NF "Std Dev=" (ssq/NF-(sum/NF)**2)**0.5}'
   echo
done
