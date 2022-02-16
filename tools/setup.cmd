:; 
:; # mbuild convenience installation script
:; 
:; SCRIPT_DIR="$( cd "$( dirname "$0" )" && pwd )"
:; if [ "$(uname)" = 'Darwin' ]; then
:;   echo "darwin"
:;   # OSX
:;   cp "${SCRIPT_DIR}/osx_build" "${SCRIPT_DIR}/../mbuild"
:;   chmod +x "${SCRIPT_DIR}/../mbuild"
:; elif [ "$(expr substr $(uname -s) 1 5)" = 'Linux' ]; then
:;   # linux
:;   cp "${SCRIPT_DIR}/lin_build" "${SCRIPT_DIR}/../mbuild"
:;   chmod +x "${SCRIPT_DIR}/../mbuild"
:; elif grep -q Microsoft /proc/version; then
:;   # windows (WSL/WSL2)
:;   cp "${SCRIPT_DIR}/win_build.exe" "${SCRIPT_DIR}/../mbuild.exe"
:; elif [ "$(expr substr $(uname -s) 1 10)" = 'MINGW32_NT' ]; then
:;   # windows (MSYS/MinGW)
:;   cp "${SCRIPT_DIR}/win_build.exe" "${SCRIPT_DIR}/../mbuild.exe"
:; elif [ "$(expr substr $(uname -s) 1 10)" = 'MINGW64_NT' ]; then
:;   # windows (MSYS/MinGW)
:;   cp "${SCRIPT_DIR}/win_build.exe" "${SCRIPT_DIR}/../mbuild.exe"
:; fi
:; echo "created: mbuild"
:; echo "Run ./mbuild to build MABE"
:; exit
@echo off
SET THISDIR=%~dp0
copy %THISDIR%\win_build.exe %THISDIR%\..\mbuild.exe
echo created: mbuild.exe
echo Run mbuild.exe to build MABE
echo Run mbuild.exe -h to see other module tool commands
