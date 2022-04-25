#!/bin/sh

set -v

for i in GhostExchange*
do
   cd $i
   rm -rf build
   cd ..
done

for i in CartExchange*
do
   cd $i
   rm -rf build
   cd ..
done

