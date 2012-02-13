#!/bin/sh
make clean
CFLAGS=-O3 make all

TGT=nikwideluxe-linux-`uname -m`

rm -fr $TGT
mkdir $TGT

cp -R manual license.txt nikwi.ico nikwi justdata.up $TGT
strip --strip-all $TGT/nikwi
tar cJvf $TGT.xz $TGT

echo Done

