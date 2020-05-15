echo off
cls

call compileDebug.bat build

set "buildDir=git\release"

echo -----------------------------------
echo Copying data..
echo -----------------------------------
echo d | xcopy /S /Y maps %buildDir%\maps
echo d | xcopy /S /Y fonts %buildDir%\fonts
echo d | xcopy /S /Y images %buildDir%\images
echo d | xcopy /S /Y data %buildDir%\data


echo -----------------------------------
echo Copying binaries..
echo -----------------------------------
xcopy /Y main.exe %buildDir%

xcopy /Y SDL2.dll %buildDir%
xcopy /Y SDL2_image.dll %buildDir%
xcopy /Y libpng16-16.dll %buildDir%
xcopy /Y zlib1.dll %buildDir%

REM pause