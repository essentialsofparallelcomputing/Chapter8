#!/bin/sh

set -v

for i in GhostExchange*
do
   cd $i
   make clean
   make distclean
   cd ..
done

for i in CartExchange*
do
   cd $i
   make clean
   make distclean
   cd ..
done

