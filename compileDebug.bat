echo off
cls

echo d | xcopy /S /Y headers ..\headers
xcopy /Y main.c ..
xcopy /Y initialize.c ..
xcopy /Y data.c ..
xcopy /Y drawFunctions.c ..
xcopy /Y tileMap.c ..
xcopy /Y collision.c ..
xcopy /Y inventory.c ..

cd ..


echo.
echo.
echo.

tcc -Werror -Wall -lSDL2 -lSDL2_image -lSDL2_net -o main.exe main.c data.c initialize.c drawFunctions.c tileMap.c collision.c inventory.c
REM Production Compile (Exclude command prompt)
REM tcc -Wl,-subsystem=gui -Werror -Wall -lSDL2 -lSDL2_image -lSDL2_net -o main.exe main.c data.c initialize.c drawFunctions.c tileMap.c collision.c inventory.c

echo.
echo.
echo.

del main.c
del initialize.c
del data.c
del drawFunctions.c
del tileMap.c
del collision.c
del inventory.c
rmdir /S /Q headers

xcopy /Y main.exe World_Builder\
del main.exe

REM pause