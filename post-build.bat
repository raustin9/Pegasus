@ECHO off

REM Post Build Process

if not exist "%cd%\bin\assets\shaders\" mkdir "%cd%\bin\assets\shaders\"

echo "Compiling shaders..."
echo "assets\shaders\vert\Builtin.ObjectShader.vert.glsl -> bin\assets\shaders\Builtin.ObjectShader.vert.spv"
tooling\glslc.exe -fshader-stage=vert assets\shaders\vert\Builtin.ObjectShader.vert.glsl -o assets\shaders\Builtin.ObjectShader.vert.spv
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "assets\shaders\frag\Builtin.ObjectShader.frag.glsl -> bin\assets\shaders\Builtin.ObjectShader.frag.spv"
tooling\glslc.exe -fshader-stage=frag assets\shaders\frag\Builtin.ObjectShader.frag.glsl -o assets\shaders\Builtin.ObjectShader.frag.spv
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "Copying assets..."
echo xcopy "assets" "bin\assets\" /h /i /c /k /e /r /y
xcopy "assets" "bin\assets" /h /i /c /k /e /r /y

echo "Done."