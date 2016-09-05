
@echo off

cd ./
if exist "build" (rd /s /q "build")
call premake5.exe vs2015

pause