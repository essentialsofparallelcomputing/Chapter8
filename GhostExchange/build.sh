#!/bin/sh

set -v

for i in GhostExchange*
do
   cd $i
   cmake -Wno-dev .
   make
   cd ..
done

for i in CartExchange*
do
   cd $i
   cmake -Wno-dev .
   make
   cd ..
done

