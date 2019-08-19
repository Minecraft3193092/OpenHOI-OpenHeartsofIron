@echo off

set CMAKE_SOURCE_DIR=%1
set CMAKE_BINARY_DIR=%2
set DEBUG_MODE=%3

if "%DEBUG_MODE%"=="1" (
    rem Debug mode -> Copy PDB files
    robocopy "%CMAKE_SOURCE_DIR%\thirdparty\manual-build\precompiled\ogre3d\bin" "%CMAKE_BINARY_DIR%\output\Debug" *_d.pdb
)

exit 0