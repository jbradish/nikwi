#!/bin/sh

make clean
CFLAGS=-O3 make all
strip --strip-all nikwi.exe

start nikwisetup.iss

