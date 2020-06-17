echo off
cls

set sourceDir=%CD%
set buildType=%~1


echo d | xcopy /S /Y headers ..\headers
xcopy /Y main.c ..
xcopy /Y initialize.c ..
xcopy /Y data.c ..
xcopy /Y tileMap.c ..
xcopy /Y collision.c ..
xcopy /Y inventory.c ..
xcopy /Y mapGeneration.c ..
xcopy /Y renderSystems.c ..

cd ..


echo.
echo.
echo.

if "%buildType%"=="debug" (
	tcc -Werror -Wall -lSDL2 -lSDL2_image -lSDL2_net -o main.exe main.c data.c initialize.c tileMap.c collision.c inventory.c mapGeneration.c renderSystems.c
	REM echo test
) else if "%buildType%"=="build" (
	REM Production Compile (Exclude command prompt)
	tcc -Wl,-subsystem=gui -Werror -Wall -lSDL2 -lSDL2_image -lSDL2_net -o main.exe main.c data.c initialize.c tileMap.c collision.c inventory.c mapGeneration.c renderSystems.c
	REM echo build
)

echo.
echo.
echo.

del main.c
del initialize.c
del data.c
del tileMap.c
del collision.c
del inventory.c
del mapGeneration.c
del renderSystems.c

rmdir /S /Q headers

xcopy /Y main.exe %sourceDir%
del main.exe
cd %sourceDir%

REM pause