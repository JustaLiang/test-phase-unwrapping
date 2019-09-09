echo off
if exist ..\build @RD /S /Q ..\build

if %1. ==. (
	echo Clean build folder...
	pause
)