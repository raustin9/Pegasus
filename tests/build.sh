#!/bin/bash
set echo on
mkdir -p ../bin

ccFilenames=$(find . -type f -name "*.cc")

assembly="tests"
cFlags="-g -fdeclspec -fPIC -std=c++17"
ldflags="-L../bin/ -lengine -Wl,-rpath,."
Includes="-Isrc -I../engine/src/"
defines="-DP_DEBUG -DQIMPORT"

echo "Building $assembly..."
clang $ccFilenames $cFlags -o ../bin/$assembly $defines $Includes $ldflags