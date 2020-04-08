@echo off
setlocal EnableDelayedExpansion

@rem ##############################################################################
@rem ##                                                                          ##
@rem ## Tool to fetch openhoi dependencies for Win64                             ##
@rem ## !YOU MUST RUN THIS SCRIPT IN THE MSVC X64 DEVELOPER COMMAND PROMPT!      ##
@rem ## !ONLY RUN THIS SCRIPT AS ADMIN IF YOU ARE ASKED FOR IT!                  ##
@rem ##                                                                          ##
@rem ##############################################################################




@rem Do some preparations...
set CHECKMARK=[32m[Y][0m
set CROSSMARK=[91m[X][0m
set LINEBEG=[36m:: [0m
set CWD=%CD%



@rem At first, check the Windows environment
echo %LINEBEG% Checking for Windows environment...

for /f "tokens=4-5 delims=. " %%i in ('ver') do set VERSION=%%i.%%j
if "%VERSION%" == "10.0" (
    echo %CHECKMARK% Windows 10 is supported.
) else (
    echo %CROSSMARK% Your Windows Version %VERSION% is not supported. Aborting. You may have to install all dependencies manually.
    goto end
)

if "%PROGRAMFILES%" == "C:\Program Files" (
    echo %CHECKMARK% 64 bit is supported.
) else (
    echo %CROSSMARK% 32 bit is not supported. Aborting. You may have to install all dependencies manually.
    goto end
)

where /q MSBuild
if %errorLevel% == 0 (
    echo %CHECKMARK% Inside MSVC developer x64 command promt
) else (
    echo %CROSSMARK% You must call this batch file from a running MSVC x64 developer command prompt
    goto end
)



@rem Then, check for required tools
echo %LINEBEG% Checking for required tools...

where /q choco
if %errorLevel% == 0 (
    echo %CHECKMARK% Chocolatey is installed and found.
    set CHOCOLATEY_INSTALLED=y
) else (
    echo %CROSSMARK% Chocolatey is not installed or not inside Windows PATH. This is okay, but you may have to install required dependencies manually. If you want, install Chocolatey from https://chocolatey.org/install
    set CHOCOLATEY_INSTALLED=n
    goto end
)

where /q git
if %errorLevel% == 0 (
    echo %CHECKMARK% Git is installed and found.
) else (
    if "%CHOCOLATEY_INSTALLED%" == "y" (
        echo %LINEBEG% Installing Git...
        choco install git.install --yes --force --params "/GitAndUnixToolsOnPath /SChannel"
        set SCRIPT_INSTALLED_SOMETHING=y
    ) else (
        echo %CROSSMARK% Git is not installed or not inside Windows PATH. Aborting. Please install Git from https://git-scm.com/download/win
        goto end
    )
)

where /q cmake
if %errorLevel% == 0 (
    echo %CHECKMARK% CMake is installed and found.
) else (
    if "%CHOCOLATEY_INSTALLED%" == "y" (
        echo %LINEBEG% Installing CMake...
        choco install cmake.install --yes --force --installargs 'ADD_CMAKE_TO_PATH=System'
        set SCRIPT_INSTALLED_SOMETHING=y
    ) else (
        echo %CROSSMARK% CMake is not installed or not inside Windows PATH. Aborting. Please install CMake from https://cmake.org/download
        goto end
    )
)

where /q ninja
if %errorLevel% == 0 (
    echo %CHECKMARK% Ninja is installed and found.
) else (
    if "%CHOCOLATEY_INSTALLED%" == "y" (
        echo %LINEBEG% Installing Ninja...
        choco install ninja --yes --force
        set SCRIPT_INSTALLED_SOMETHING=y
    ) else (
        echo %CROSSMARK% Ninja is not installed or not inside Windows PATH. Aborting. Please install Ninja from https://github.com/ninja-build/ninja/releases
        goto end
    )
)

where /q 7z
if %errorLevel% == 0 (
    echo %CHECKMARK% 7-Zip is installed and found.
) else (
    if "%CHOCOLATEY_INSTALLED%" == "y" (
        echo %LINEBEG% Installing 7-Zip...
        choco install 7zip.install --yes
        set SCRIPT_INSTALLED_SOMETHING=y
    ) else (
        echo %CROSSMARK% 7-Zip is not installed or not inside Windows PATH. Aborting. Please install 7-Zip from https://www.7-zip.org
        goto end
    )
)

where /q nuget
if %errorLevel% == 0 (
    echo %CHECKMARK% NuGet is installed and found.
) else (
    if "%CHOCOLATEY_INSTALLED%" == "y" (
        echo %LINEBEG% Installing NuGet...
        choco install nuget.commandline --yes --force
        set SCRIPT_INSTALLED_SOMETHING=y
    ) else (
        echo %CROSSMARK% NuGet is not installed or not inside Windows PATH. Aborting. Please install NuGet from https://www.nuget.org/downloads
        goto end
    )
)

where /q clang-format
if %errorLevel% == 0 (
    echo %CHECKMARK% Clang-format is installed and found.
) else (
    if "%CHOCOLATEY_INSTALLED%" == "y" (
        echo %LINEBEG% Installing Clang-format ^(LLVM^)...
        choco install llvm --yes --force
        set SCRIPT_INSTALLED_SOMETHING=y
    ) else (
        echo %CROSSMARK% Clang-format is not installed or not inside Windows PATH. Aborting. Please install Clang-format from https://llvm.org/builds
        goto end
    )
)


@rem Refresh environmental variables if required
if "%SCRIPT_INSTALLED_SOMETHING%" == "y" (
    echo %LINEBEG% Refreshing environmental variables...
    refreshenv
)


@rem Install required libs...
echo %LINEBEG% Checking for required libraries...

for /F "tokens=1* delims==" %%A in (prebuilt-dep-versions) do (
  set %%A=%%B
)

setlocal enableextensions
if not exist thirdparty\manual-build (
  mkdir thirdparty\manual-build
)
if not exist thirdparty\manual-build\lib (
    mkdir thirdparty\manual-build\lib
)
if not exist thirdparty\manual-build\precompiled (
    mkdir thirdparty\manual-build\precompiled
)
endlocal

echo %LINEBEG% Boost...
set BOOST_NAME=boost
set BOOST_VERSION_MAJOR=1
set BOOST_VERSION_MINOR=69
set BOOST_VERSION=%BOOST_VERSION_MAJOR%.%BOOST_VERSION_MINOR%.0
set BOOST_FETCH_INCLUDES=y
if exist "%CWD%\thirdparty\manual-build\precompiled\boost\include\boost\version.hpp" (
  find /c /i "#define BOOST_LIB_VERSION ""%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%""" "%CWD%\thirdparty\manual-build\precompiled\boost\include\boost\version.hpp"
  if %errorLevel% == 0 (
    set BOOST_FETCH_INCLUDES=n
  )
)
if "%BOOST_FETCH_INCLUDES%" == "y" (
  nuget install %BOOST_NAME% -Version %BOOST_VERSION% -OutputDirectory thirdparty\manual-build\lib
  @rd /s /q "%CWD%\thirdparty\manual-build\precompiled\boost\include" 2>nul
  robocopy "thirdparty\manual-build\lib\%BOOST_NAME%.%BOOST_VERSION%.0\lib\native\include" "%CWD%\thirdparty\manual-build\precompiled\boost\include" /mir
)
set BOOST_PO_NAME=boost_program_options-vc141
set BOOST_LIBS_INSTALLED=n
if not exist "%CWD%\thirdparty\manual-build\precompiled\boost\lib\%BOOST_PO_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib" (
  if "%BOOST_LIBS_INSTALLED%" == "n" (
    nuget install boost-vc141 -Version %BOOST_VERSION% -OutputDirectory thirdparty\manual-build\lib
    set BOOST_LIBS_INSTALLED=y
  )
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_PO_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_PO_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_PO_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_PO_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.dll
)
set BOOST_LS_NAME=boost_log_setup-vc141
if not exist "%CWD%\thirdparty\manual-build\precompiled\boost\lib\%BOOST_LS_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib" (
  if "%BOOST_LIBS_INSTALLED%" == "n" (
    nuget install boost-vc141 -Version %BOOST_VERSION% -OutputDirectory thirdparty\manual-build\lib
    set BOOST_LIBS_INSTALLED=y
  )
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_LS_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_LS_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_LS_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_LS_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.dll
)
set BOOST_LOC_NAME=boost_locale-vc141
if not exist "%CWD%\thirdparty\manual-build\precompiled\boost\lib\%BOOST_LOC_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib" (
  if "%BOOST_LIBS_INSTALLED%" == "n" (
    nuget install boost-vc141 -Version %BOOST_VERSION% -OutputDirectory thirdparty\manual-build\lib
    set BOOST_LIBS_INSTALLED=y
  )
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_LOC_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_LOC_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_LOC_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_LOC_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.dll
)
set BOOST_DT_NAME=boost_date_time-vc141
if not exist "%CWD%\thirdparty\manual-build\precompiled\boost\lib\%BOOST_DT_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib" (
  if "%BOOST_LIBS_INSTALLED%" == "n" (
    nuget install boost-vc141 -Version %BOOST_VERSION% -OutputDirectory thirdparty\manual-build\lib
    set BOOST_LIBS_INSTALLED=y
  )
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_DT_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_DT_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_DT_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_DT_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.dll
)
set BOOST_FS_NAME=boost_filesystem-vc141
if not exist "%CWD%\thirdparty\manual-build\precompiled\boost\lib\%BOOST_FS_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib" (
  if "%BOOST_LIBS_INSTALLED%" == "n" (
    nuget install boost-vc141 -Version %BOOST_VERSION% -OutputDirectory thirdparty\manual-build\lib
    set BOOST_LIBS_INSTALLED=y
  )
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_FS_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_FS_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_FS_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_FS_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.dll
)
set BOOST_TH_NAME=boost_thread-vc141
if not exist "%CWD%\thirdparty\manual-build\precompiled\boost\lib\%BOOST_TH_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib" (
  if "%BOOST_LIBS_INSTALLED%" == "n" (
    nuget install boost-vc141 -Version %BOOST_VERSION% -OutputDirectory thirdparty\manual-build\lib
    set BOOST_LIBS_INSTALLED=y
  )
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_TH_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_TH_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_TH_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_TH_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.dll
)
set BOOST_RE_NAME=boost_regex-vc141
if not exist "%CWD%\thirdparty\manual-build\precompiled\boost\lib\%BOOST_RE_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib" (
  if "%BOOST_LIBS_INSTALLED%" == "n" (
    nuget install boost-vc141 -Version %BOOST_VERSION% -OutputDirectory thirdparty\manual-build\lib
    set BOOST_LIBS_INSTALLED=y
  )
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_RE_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_RE_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_RE_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_RE_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.dll
)
set BOOST_CR_NAME=boost_chrono-vc141
if not exist "%CWD%\thirdparty\manual-build\precompiled\boost\lib\%BOOST_CR_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib" (
  if "%BOOST_LIBS_INSTALLED%" == "n" (
    nuget install boost-vc141 -Version %BOOST_VERSION% -OutputDirectory thirdparty\manual-build\lib
    set BOOST_LIBS_INSTALLED=y
  )
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_CR_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_CR_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_CR_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_CR_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.dll
)
set BOOST_ATO_NAME=boost_atomic-vc141
if not exist "%CWD%\thirdparty\manual-build\precompiled\boost\lib\%BOOST_ATO_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib" (
  if "%BOOST_LIBS_INSTALLED%" == "n" (
    nuget install boost-vc141 -Version %BOOST_VERSION% -OutputDirectory thirdparty\manual-build\lib
    set BOOST_LIBS_INSTALLED=y
  )
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_ATO_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_ATO_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_ATO_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_ATO_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.dll
)
set BOOST_SYS_NAME=boost_system-vc141
if not exist "%CWD%\thirdparty\manual-build\precompiled\boost\lib\%BOOST_SYS_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib" (
  if "%BOOST_LIBS_INSTALLED%" == "n" (
    nuget install boost-vc141 -Version %BOOST_VERSION% -OutputDirectory thirdparty\manual-build\lib
    set BOOST_LIBS_INSTALLED=y
  )
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_SYS_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_SYS_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.lib
  robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_SYS_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_SYS_NAME%-mt-x64-%BOOST_VERSION_MAJOR%_%BOOST_VERSION_MINOR%.dll
)

echo %LINEBEG% OpenSSL...
set OPENSSL_VERSION=1.1.4
if not exist "%CWD%\thirdparty\manual-build\precompiled\openssl\openhoi-version-%OPENSSL_VERSION%" (
  set OPENSSL_NAME=openssl-vc140-vc141-x86_64
  @rd /s /q %CWD%\thirdparty\manual-build\precompiled\openssl 2>nul
  nuget install %OPENSSL_NAME% -Version %OPENSSL_VERSION% -OutputDirectory thirdparty\manual-build\lib
  mkdir "%CWD%\thirdparty\manual-build\precompiled\openssl\lib"
  mkdir "%CWD%\thirdparty\manual-build\precompiled\openssl\bin"
  copy "%CWD%\thirdparty\manual-build\lib\%OPENSSL_NAME%.%OPENSSL_VERSION%\build\native\lib\vc141\x64\libcrypto.lib" "%CWD%\thirdparty\manual-build\precompiled\openssl\lib\libcrypto.lib"
  copy "%CWD%\thirdparty\manual-build\lib\%OPENSSL_NAME%.%OPENSSL_VERSION%\build\native\lib\vc141\x64\libssl.lib" "%CWD%\thirdparty\manual-build\precompiled\openssl\lib\libssl.lib"
  copy "%CWD%\thirdparty\manual-build\lib\%OPENSSL_NAME%.%OPENSSL_VERSION%\build\native\lib\vc141\x64\libcrypto-1_1-x64.dll" "%CWD%\thirdparty\manual-build\precompiled\openssl\bin\libcrypto-1_1-x64.dll"
  copy "%CWD%\thirdparty\manual-build\lib\%OPENSSL_NAME%.%OPENSSL_VERSION%\build\native\lib\vc141\x64\libssl-1_1-x64.dll" "%CWD%\thirdparty\manual-build\precompiled\openssl\bin\libssl-1_1-x64.dll"
  robocopy "%CWD%\thirdparty\manual-build\lib\%OPENSSL_NAME%.%OPENSSL_VERSION%\build\native\include" "%CWD%\thirdparty\manual-build\precompiled\openssl\include" /mir
  type nul >>thirdparty\manual-build\precompiled\openssl\openhoi-version-%OPENSSL_VERSION%
  cd %CWD%
)

echo %LINEBEG% OpenAL...
set OPENAL_BRANCH=openal-soft-1.19.1
if not exist "%CWD%\thirdparty\manual-build\precompiled\openal\openhoi-branch-%OPENAL_BRANCH%" (
  @rd /s /q %CWD%\thirdparty\manual-build\precompiled\openal 2>nul
  mkdir %CWD%\thirdparty\manual-build\precompiled\openal
  if not exist thirdparty\manual-build\lib\openal (
      git clone https://github.com/kcat/openal-soft --branch %OPENAL_BRANCH% thirdparty\manual-build\lib\openal
  ) else (
      git -C thirdparty\manual-build\lib\openal reset --hard
      git -C thirdparty\manual-build\lib\openal fetch --tags
      git -C thirdparty\manual-build\lib\openal checkout %OPENAL_BRANCH%
      git -C thirdparty\manual-build\lib\openal pull
  )
  cd thirdparty\manual-build\lib\openal
  @rd /s /q build 2>nul
  git reset --hard
  cd build
  cmake -DALSOFT_TESTS=OFF -DALSOFT_UTILS=OFF -DCMAKE_BUILD_TYPE=Release -G Ninja ..
  ninja
  robocopy "%CWD%\thirdparty\manual-build\lib\openal\build" "%CWD%\thirdparty\manual-build\precompiled\openal\lib" OpenAL32.lib
  robocopy "%CWD%\thirdparty\manual-build\lib\openal\build" "%CWD%\thirdparty\manual-build\precompiled\openal\bin" OpenAL32.dll
  cd ..
  robocopy "include" "%CWD%\thirdparty\manual-build\precompiled\openal\include" /mir
  type nul >>thirdparty\manual-build\precompiled\openal\openhoi-branch-%OPENAL_BRANCH%
  cd %CWD%
)

echo %LINEBEG% zlib...
set ZLIB_VERSION=1.2.11.8900
if not exist "%CWD%\thirdparty\manual-build\precompiled\zlib\openhoi-version-%ZLIB_VERSION%" (
  set ZLIB_NAME=zlib-msvc-x64
  @rd /s /q %CWD%\thirdparty\manual-build\precompiled\zlib 2>nul
  mkdir %CWD%\thirdparty\manual-build\precompiled\zlib
  type nul >>thirdparty\manual-build\precompiled\zlib\openhoi-version-%ZLIB_VERSION%
  nuget install %ZLIB_NAME% -Version %ZLIB_VERSION% -OutputDirectory thirdparty\manual-build\lib
  robocopy "%CWD%\thirdparty\manual-build\lib\%ZLIB_NAME%.%ZLIB_VERSION%\build\native\lib_release" "%CWD%\thirdparty\manual-build\precompiled\zlib\lib" zlib.lib
  robocopy "%CWD%\thirdparty\manual-build\lib\%ZLIB_NAME%.%ZLIB_VERSION%\build\native\bin_release" "%CWD%\thirdparty\manual-build\precompiled\zlib\bin" zlib.dll
  robocopy "%CWD%\thirdparty\manual-build\lib\%ZLIB_NAME%.%ZLIB_VERSION%\build\native\include" "%CWD%\thirdparty\manual-build\precompiled\zlib\include" /mir
)

echo %LINEBEG% Eigen...
set EIGEN_VERSION_MAJOR=3
set EIGEN_BRANCH=%EIGEN_VERSION_MAJOR%.3.7
set EIGEN_REQUIRE_BUILD=y
if exist "%CWD%\thirdparty\manual-build\lib\eigen\build\Eigen%EIGEN_VERSION_MAJOR%ConfigVersion.cmake" (
  find /c /i "set(PACKAGE_VERSION ""%EIGEN_BRANCH%"")" "%CWD%\thirdparty\manual-build\lib\eigen\build\Eigen%EIGEN_VERSION_MAJOR%ConfigVersion.cmake"
  if %errorLevel% == 0 (
    set EIGEN_REQUIRE_BUILD=n
  )
)
if "%EIGEN_REQUIRE_BUILD%" == "y" (
  set EIGEN_BRANCH=3.3.7
  if not exist thirdparty\manual-build\lib\eigen (
      git clone https://github.com/eigenteam/eigen-git-mirror --branch %EIGEN_BRANCH% thirdparty\manual-build\lib\eigen
  ) else (
      git -C thirdparty\manual-build\lib\eigen reset --hard
      git -C thirdparty\manual-build\lib\eigen fetch
      git -C thirdparty\manual-build\lib\eigen checkout %EIGEN_BRANCH%
      git -C thirdparty\manual-build\lib\eigen pull
  )
  cd thirdparty\manual-build\lib\eigen
  @rd /s /q build 2>nul
  mkdir build
  cd build
  cmake -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release -G Ninja ..
  ninja
  cd %CWD%
)

echo %LINEBEG% GMP / MPFR...
if not exist "%CWD%\thirdparty\manual-build\precompiled\gmp\include\gmp.h" (
  powershell -Command "$progressPreference = 'silentlyContinue'; Invoke-WebRequest https://dependencies.openhoi.net/gmp_10-mpfr_4.7z -OutFile %CWD%\thirdparty\manual-build\lib\gmp-mpfr.7z"
  @rd /s /q "%CWD%\thirdparty\manual-build\precompiled\gmp" 2>nul
  7z x "%CWD%\thirdparty\manual-build\lib\gmp-mpfr.7z" -o"%CWD%\thirdparty\manual-build\precompiled\gmp"
)

echo %LINEBEG% CGAL...
set CGAL_VERSION=5.0.2
if not exist "%CWD%\thirdparty\manual-build\precompiled\cgal\lib\CGAL-vc142-mt-%CGAL_VERSION%.lib" (
  rem We need to download the source ZIP because the directory structure is different to the GitHub/development version
  rem See https://www.cgal.org/download/windows.html
  echo Downloading CGAL v%CGAL_VERSION%...
  powershell -Command "$progressPreference = 'silentlyContinue'; Invoke-WebRequest https://github.com/CGAL/cgal/releases/download/releases/CGAL-%CGAL_VERSION%/CGAL-%CGAL_VERSION%.zip -OutFile %CWD%\thirdparty\manual-build\lib\cgal.zip"
  echo Expanding CGAL v%CGAL_VERSION% ZIP archive...
  powershell -Command "$progressPreference = 'silentlyContinue'; Expand-Archive -Force %CWD%\thirdparty\manual-build\lib\cgal.zip %CWD%\thirdparty\manual-build\lib"
  @rd /s /q %CWD%\thirdparty\manual-build\lib\cgal 2>nul
  @rd /s /q %CWD%\thirdparty\manual-build\precompiled\cgal 2>nul
  move "%CWD%\thirdparty\manual-build\lib\CGAL-%CGAL_VERSION%" "%CWD%\thirdparty\manual-build\lib\cgal"
  cd %CWD%\thirdparty\manual-build\lib\cgal
  mkdir build
  cd build
  set CWD_FS=%CWD:\=/%
  if /I "%CI%" neq "true" (
    cmake -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=ON -DCGAL_HEADER_ONLY=OFF -DWITH_CGAL_Qt5=OFF -DCMAKE_INSTALL_PREFIX=%CWD_FS%/thirdparty/manual-build/precompiled/cgal -DBOOST_INCLUDEDIR=%CWD_FS%/thirdparty/manual-build/precompiled/boost/include -DGMP_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/include -DGMP_LIBRARIES=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/lib/libgmp-10.lib -DMPFR_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/include -DMPFR_LIBRARIES=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/lib/libmpfr-4.lib -DWITH_Eigen3=ON -DEIGEN3_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/lib/eigen -D CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE=TRUE -DCMAKE_BUILD_TYPE=Debug -G Ninja ..
    ninja
    ninja install
    cd ..
    @rd /s /q build 2>nul
    mkdir build
    cd build
  )
  cmake -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=ON -DCGAL_HEADER_ONLY=OFF -DWITH_CGAL_Qt5=OFF -DCMAKE_INSTALL_PREFIX=%CWD_FS%/thirdparty/manual-build/precompiled/cgal -DBOOST_INCLUDEDIR=%CWD_FS%/thirdparty/manual-build/precompiled/boost/include -DGMP_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/include -DGMP_LIBRARIES=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/lib/libgmp-10.lib -DMPFR_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/include -DMPFR_LIBRARIES=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/lib/libmpfr-4.lib -DWITH_Eigen3=ON -DEIGEN3_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/lib/eigen -DCMAKE_BUILD_TYPE=Release -G Ninja ..
  ninja
  ninja install
  robocopy "%CWD%\thirdparty\manual-build\lib\cgal\build\include\CGAL" "%CWD%\thirdparty\manual-build\precompiled\cgal\include\CGAL" compiler_config.h
  robocopy "%CWD%\thirdparty\manual-build\lib\cgal\build\include\CGAL" "%CWD%\thirdparty\manual-build\lib\cgal\include\CGAL" compiler_config.h
  cd %CWD%
)

echo %LINEBEG% Lua...
set LUA_VERSION=5.3.5.1
if not exist "%CWD%\thirdparty\manual-build\precompiled\lua\openhoi-version-%LUA_VERSION%" (
  @rd /s /q %CWD%\thirdparty\manual-build\precompiled\lua 2>nul
  mkdir %CWD%\thirdparty\manual-build\precompiled\lua
  type nul >>thirdparty\manual-build\precompiled\lua\openhoi-version-%LUA_VERSION%
  set LUA_NAME=lua
  nuget install %LUA_NAME% -Version %LUA_VERSION% -OutputDirectory thirdparty\manual-build\lib
  @rd /s /q "%CWD%\thirdparty\manual-build\precompiled\lua" 2>nul
  robocopy "thirdparty\manual-build\lib\%LUA_NAME%.%LUA_VERSION%\build\native\include" "%CWD%\thirdparty\manual-build\precompiled\lua\include" /mir
  robocopy "%CWD%\thirdparty\manual-build\lib\%LUA_NAME%.%LUA_VERSION%\build\native\lib\v142\x64\Release" "%CWD%\thirdparty\manual-build\precompiled\lua\lib" lua.lib
  robocopy "%CWD%\thirdparty\manual-build\lib\%LUA_NAME%.redist.%LUA_VERSION%\build\native\bin\v142\x64\Release" "%CWD%\thirdparty\manual-build\precompiled\lua\bin" lua.dll
  cd %CWD%
)

echo %LINEBEG% Protocol Buffers...
set PROTOBUF_BRANCH=v3.11.4
if not exist "%CWD%\thirdparty\manual-build\precompiled\protobuf\openhoi-branch-%PROTOBUF_BRANCH%" (
  @rd /s /q %CWD%\thirdparty\manual-build\precompiled\protobuf 2>nul
  mkdir %CWD%\thirdparty\manual-build\precompiled\protobuf
  if not exist thirdparty\manual-build\lib\protobuf (
      git clone https://github.com/protocolbuffers/protobuf --branch %PROTOBUF_BRANCH% thirdparty\manual-build\lib\protobuf
  ) else (
      git -C thirdparty\manual-build\lib\protobuf reset --hard
      git -C thirdparty\manual-build\lib\protobuf fetch --tags
      git -C thirdparty\manual-build\lib\protobuf checkout %PROTOBUF_BRANCH%
      git -C thirdparty\manual-build\lib\protobuf pull
  )
  cd thirdparty\manual-build\lib\protobuf\cmake
  @rd /s /q build 2>nul
  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_SHARED_LIBS=ON -DZLIB_LIBRARY="%CWD%\thirdparty\manual-build\precompiled\zlib\lib\zlib.lib" -DZLIB_INCLUDE_DIR="%CWD%\thirdparty\manual-build\precompiled\zlib\include" -G Ninja ..
  ninja
  @rd /s /q %CWD%\thirdparty\manual-build\precompiled\protobuf 2>nul
  robocopy "%CWD%\thirdparty\manual-build\lib\protobuf\cmake\build" "%CWD%\thirdparty\manual-build\precompiled\protobuf\lib" libprotobuf.lib libprotobuf-lite.lib libprotoc.lib
  robocopy "%CWD%\thirdparty\manual-build\lib\protobuf\cmake\build" "%CWD%\thirdparty\manual-build\precompiled\protobuf\bin" protoc.exe libprotobuf.dll libprotobuf-lite.dll libprotoc.dll
  copy "%CWD%\thirdparty\manual-build\precompiled\zlib\bin\zlib.dll" "%CWD%\thirdparty\manual-build\precompiled\protobuf\bin\zlib.dll" 
  robocopy "%CWD%\thirdparty\manual-build\lib\protobuf\src" "%CWD%\thirdparty\manual-build\precompiled\protobuf\include" /mir
  type nul >>"%CWD%\thirdparty\manual-build\precompiled\protobuf\openhoi-branch-%PROTOBUF_BRANCH%"
  cd %CWD%
)

echo %LINEBEG% GameNetworkingSockets...
set GAMENETWORKINGSOCKETS_BRANCH=v1.1.0
if not exist "%CWD%\thirdparty\manual-build\precompiled\gamenetworkingsockets\openhoi-branch-%GAMENETWORKINGSOCKETS_BRANCH%" (
  @rd /s /q %CWD%\thirdparty\manual-build\precompiled\gamenetworkingsockets 2>nul
  if not exist thirdparty\manual-build\lib\gamenetworkingsockets (
      git clone https://github.com/ValveSoftware/GameNetworkingSockets --branch %GAMENETWORKINGSOCKETS_BRANCH% thirdparty\manual-build\lib\gamenetworkingsockets
  ) else (
      git -C thirdparty\manual-build\lib\gamenetworkingsockets reset --hard
      git -C thirdparty\manual-build\lib\gamenetworkingsockets fetch --tags
      git -C thirdparty\manual-build\lib\gamenetworkingsockets checkout %GAMENETWORKINGSOCKETS_BRANCH%
      git -C thirdparty\manual-build\lib\gamenetworkingsockets pull
  )
  cd thirdparty\manual-build\lib\gamenetworkingsockets
  @rd /s /q build 2>nul
  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release -DProtobuf_USE_STATIC_LIBS=OFF -DProtobuf_LIBRARY_RELEASE="%CWD%\thirdparty\manual-build\precompiled\protobuf\lib\libprotobuf.lib" -DProtobuf_PROTOC_EXECUTABLE="%CWD%\thirdparty\manual-build\precompiled\protobuf\bin\protoc.exe" -DProtobuf_INCLUDE_DIR="%CWD%\thirdparty\manual-build\precompiled\protobuf\include" -DOPENSSL_CRYPTO_LIBRARY="%CWD%\thirdparty\manual-build\precompiled\openssl\lib\libcrypto.lib" -DOPENSSL_INCLUDE_DIR="%CWD%\thirdparty\manual-build\precompiled\openssl\include" -DOPENSSL_ROOT_DIR="%CWD%\thirdparty\manual-build\precompiled\openssl" -G Ninja ..
  ninja
  robocopy "%CWD%\thirdparty\manual-build\lib\gamenetworkingsockets\build\src" "%CWD%\thirdparty\manual-build\precompiled\gamenetworkingsockets\lib" GameNetworkingSockets_s.lib
  robocopy "%CWD%\thirdparty\manual-build\lib\gamenetworkingsockets\include" "%CWD%\thirdparty\manual-build\precompiled\gamenetworkingsockets\include" /mir
  type nul >>"%CWD%\thirdparty\manual-build\precompiled\gamenetworkingsockets\openhoi-branch-%GAMENETWORKINGSOCKETS_BRANCH%"
  cd %CWD%
)

echo %LINEBEG% RapidJSON...
set RAPIDJSON_BRANCH=v1.1.0
if not exist "%CWD%\thirdparty\manual-build\precompiled\rapidjson\openhoi-branch-%RAPIDJSON_BRANCH%" (
  @rd /s /q %CWD%\thirdparty\manual-build\precompiled\rapidjson 2>nul
  if not exist thirdparty\manual-build\lib\rapidjson (
      git clone https://github.com/Tencent/rapidjson --branch %RAPIDJSON_BRANCH% thirdparty\manual-build\lib\rapidjson
  ) else (
      git -C thirdparty\manual-build\lib\rapidjson reset --hard
      git -C thirdparty\manual-build\lib\rapidjson fetch
      git -C thirdparty\manual-build\lib\rapidjson checkout %RAPIDJSON_BRANCH%
      git -C thirdparty\manual-build\lib\rapidjson pull
  )
  robocopy "%CWD%\thirdparty\manual-build\lib\rapidjson\include" "%CWD%\thirdparty\manual-build\precompiled\rapidjson\include" /mir
  type nul >>"%CWD%\thirdparty\manual-build\precompiled\rapidjson\openhoi-branch-%RAPIDJSON_BRANCH%"
  cd %CWD%
)

echo %LINEBEG% OGRE / SDL2...
for /f "tokens=1,2,3 delims=." %%a IN ("%OGRE_VERSION%") do (
  set OGRE_VERSION_MAJOR=%%a
  set OGRE_VERSION_MINOR=%%b
  set OGRE_VERSION_PATCH=%%c
)
set OGRE_FETCH=y
if exist "%CWD%\thirdparty\manual-build\precompiled\ogre3d\include\OGRE\OgreBuildSettings.h" (
  find /c /i "#define OGRE_VERSION_MAJOR %OGRE_VERSION_MAJOR%" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\include\OGRE\OgreBuildSettings.h"
  if %errorLevel% == 0 (
    find /c /i "#define OGRE_VERSION_MINOR %OGRE_VERSION_MINOR%" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\include\OGRE\OgreBuildSettings.h"
    if %errorLevel% == 0 (
      find /c /i "#define OGRE_VERSION_PATCH %OGRE_VERSION_PATCH%" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\include\OGRE\OgreBuildSettings.h"
      if %errorLevel% == 0 (
        set OGRE_FETCH=n
      )
    )
  )
)
if "%OGRE_FETCH%" == "y" (
  powershell -Command "$progressPreference = 'silentlyContinue'; Invoke-WebRequest https://dependencies.openhoi.net/ogre/ogre_msvc_%OGRE_VERSION%.7z -OutFile %CWD%\thirdparty\manual-build\lib\ogre.7z"
  @rd /s /q "%CWD%\thirdparty\manual-build\precompiled\ogre3d" 2>nul
  @rd /s /q "%CWD%\thirdparty\manual-build\precompiled\sdl2" 2>nul
  7z x "%CWD%\thirdparty\manual-build\lib\ogre.7z" -o"%CWD%\thirdparty\manual-build\precompiled"
)

echo %LINEBEG% ImGui...
set OGRE_PACKAGE_BRANCH=v%OGRE_VERSION%
if not exist "%CWD%\thirdparty\manual-build\precompiled\imgui\openhoi-branch-%OGRE_PACKAGE_BRANCH%" (
  @rd /s /q %CWD%\thirdparty\manual-build\precompiled\imgui 2>nul
  if not exist thirdparty\manual-build\lib\imgui (
      git clone --recurse-submodules https://github.com/openhoi/ogre-package --branch %OGRE_PACKAGE_BRANCH% thirdparty\manual-build\lib\ogre-package
  ) else (
      git -C thirdparty\manual-build\lib\ogre-package reset --hard
      git -C thirdparty\manual-build\lib\ogre-package fetch
      git -C thirdparty\manual-build\lib\ogre-package checkout %OGRE_PACKAGE_BRANCH%
      git -C thirdparty\manual-build\lib\ogre-package pull
      git -C thirdparty\manual-build\lib\ogre-package submodule update --init --recursive
  )
  robocopy "%CWD%\thirdparty\manual-build\lib\ogre-package\imgui" "%CWD%\thirdparty\manual-build\precompiled\imgui" /mir
  type nul >>"%CWD%\thirdparty\manual-build\precompiled\imgui\openhoi-branch-%OGRE_PACKAGE_BRANCH%"
  cd %CWD%
)



set FETCH_FINISHED=y

:end

if "%FETCH_FINISHED%" == "y" (
    echo %CHECKMARK% Finished! Please check the above command line output in order to verify that everything was okay.
)