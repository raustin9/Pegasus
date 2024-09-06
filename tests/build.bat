REM Build script for the Engine tests
@ECHO OFF
SetLocal EnableDelayedExpansion

REM Get list of all .cc files
SET cFilenames=
FOR /R %%f in (*.cc) do (SET cFilenames=!cFilenames! %%f)

SET assembly=tests
SET cFlags=-g -std=c++17 -Wno-missing-braces
SET Includes=-Isrc -I../engine/src/
SET ldflags= -L../bin/ -lengine.lib
SET defines=-DP_DEBUG -DQIMPORT

ECHO "Building %assembly%%..."
clang++ %cFilenames% %cFlags% -o ../bin/%assembly%.exe %defines% %Includes% %ldflags%