@ECHO OFF
REM Build Everything

ECHO "Building everything..."

REM Engine
make -f "Makefile.engine.windows.mak" All
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

REM Testbed
make -f "Makefile.testbed.windows.mak" All
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

REM Build the tests. I dont feel like making a whole Makefile for this yet
PUSHD tests
CALL build.bat
POPD
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

@REM PUSHD engine
@REM CALL build.bat
@REM POPD
@REM IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

@REM PUSHD testbed
@REM CALL build.bat
@REM POPD
@REM IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

ECHO "All assemblies built successfully."

post-build.bat