@echo off

setlocal EnableDelayedExpansion

rem LF start
set LF=^


rem LF end -- The two empty lines are required here

set DIRS=client/ server/ hoibase/

where /q clang-format
if %errorLevel% neq 0 (
   echo Clang-format is not installed or not inside Windows PATH. Aborting. Please install Clang-format from https://llvm.org/builds
   goto end
)

(for %%d in (%DIRS%) do (
   set FILES=
   for /F "delims=" %%f in ('powershell "Get-ChildItem -Path %%d -Recurse -Include *.cpp,*.hpp | %% { $_.FullName }"') do (
      if defined FILES set "FILES=!FILES!!LF!"
      set "FILES=!FILES!%%f"
   )
   clang-format -i !FILES!
))

:end
exit /B %errorLevel%