@echo off
SET THISDIR=%~dp0
copy %THISDIR%\src\Utilities\win_build.exe %THISDIR%\mbuild.exe
echo "Installed mbuild in the mabe repo root"
