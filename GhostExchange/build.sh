#!/bin/sh

set -v

for i in GhostExchange*
do
   cd $i
   mkdir build; cd build
   cmake -Wno-dev ..
   make
   cd ../..
done

for i in CartExchange*
do
   cd $i
   mkdir build; cd build
   cmake -Wno-dev ..
   make
   cd ../..
done

