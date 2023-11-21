#!/bin/bash

echo "Building everything..."

# Engine
make -f "Makefile.engine.linux.mak" all
errorlevel=$?
if [ $ERRORLEVEL -ne 0 ]
then
    echo "Error: $ERRORLEVEL" && exit
fi

# Testbed
make -f "Makefile.testbed.linux.mak" all
errorlevel=$?
if [ $ERRORLEVEL -ne 0 ]
then
    echo "Error: $ERRORLEVEL" && exit
fi

echo "All assemblies built successfully"

