echo off
cls

echo d | xcopy /S /Y headers ..\headers
xcopy /Y main.c ..
xcopy /Y initialize.c ..
xcopy /Y data.c ..
xcopy /Y drawFunctions.c ..
xcopy /Y tileMap.c ..

cd ..


echo.
echo.
echo.

tcc -Wl,-subsystem=gui -Werror -Wall -lSDL2 -lSDL2_ttf -lSDL2_image -o main.exe main.c data.c initialize.c drawFunctions.c tileMap.c
REM tcc -Werror -Wall -lSDL2 -lSDL2_ttf -lSDL2_image -o main.exe main.c data.c initialize.c drawFunctions.c tileMap.c

echo.
echo.
echo.

del main.c
del initialize.c
del data.c
del drawFunctions.c
del tileMap.c
rmdir /S /Q headers

xcopy /Y main.exe World_Builder\
del main.exe

pause