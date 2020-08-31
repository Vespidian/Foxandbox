echo off
cls

set buildType=%~1

echo.
echo.
echo.

if "%buildType%"=="debug" (
	mingw32-make debug
) else if "%buildType%"=="build" (
	REM Production Compile (Exclude command prompt)
	mingw32-make build
)

echo.
echo.
echo.