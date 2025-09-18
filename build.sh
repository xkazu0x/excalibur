#!/bin/bash
set -eu
cd "$(dirname "$0")"

for arg in "$@"; do declare $arg='1'; done
if [ ! -v release ]; then debug=1; fi
if [ -v debug ];     then echo "[debug mode]"; fi
if [ -v release ];   then echo "[release mode]"; fi

compile_common="-I../src/ -pedantic -Wall -Wextra"
compile_debug="gcc -g -O0 ${compile_common}"
compile_release="gcc -g -O2 ${compile_common}"
link=""
out="-o"

if [ -v debug ]; then compile="$compile_debug"; fi
if [ -v release ]; then compile="$compile_release"; fi

mkdir -p build

cd build
$compile ../src/main.c $link $out EXCALIBUR 
cd ..

if [ -v run ]; then ./build/EXCALIBUR; fi
