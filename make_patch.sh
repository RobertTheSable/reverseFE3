#!/bin/bash

# script to make generating 
# run after running make

do_patch() 
{
    ./sort $1
    mv FE3_RR.smc $2.$3
    flips --create -i $1 $2.$3
    rm -f $2.$3
}

if [ ! -f "build/sort" ]; then
    exit 1
fi

cd build

rm -f FE3_RR.smc

rm -Rf *.ips

for x in 1*.sfc ; do
    do_patch $x ${x%.*}_noheader sfc
done

for x in 1*.smc ; do
    do_patch $x ${x%.*}_headered smc
done

cd ..
