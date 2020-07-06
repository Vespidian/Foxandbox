echo off
cls

set buildType=%~1

echo.
echo.
echo.

if "%buildType%"=="debug" (
	make debug
) else if "%buildType%"=="build" (
	REM Production Compile (Exclude command prompt)
	make build
)

echo.
echo.
echo.