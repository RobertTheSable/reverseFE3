#!/bin/bash

# script to make generating 
# run after running make

do_patch() 
{
    ./sort $1
    outfile=`basename $2-$4.$3`
    echo $outfile
    mv FE3_RR.smc $outfile
    flips --create -i $1 $outfile
    rm -f $outfile
}

patch_lang()
{
    for x in ${1}roms/1*.sfc ; do
        do_patch $x ${x%.*}_noheader sfc $1
    done

    for x in ${1}roms/1*.smc ; do
        do_patch $x ${x%.*}_headered smc $1
    done
}

if [ ! -f "build/sort" ]; then
    exit 1
fi

cd build

rm -f FE3_RR.smc

rm -Rf *.ips

patch_lang jp


patch_lang eng

cd ..
