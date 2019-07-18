@echo off

@rem ##############################################################################
@rem ##                                                                          ##
@rem ## Tool to fetch openhoi dependencies for Win64                             ##
@rem ## !YOU MUST RUN THIS SCRIPT IN THE MSVC X64 DEVELOPER COMMAND PROMPT!      ##
@rem ## !DO NOT RUN THIS SCRIPT AS ADMIN!                                        ##
@rem ##                                                                          ##
@rem ##############################################################################




@rem Do some preparations...
set CHECKMARK=[32m[Y][0m
set CROSSMARK=[91m[X][0m
set LINEBEG=[36m:: [0m
set CWD=%CD%
set CI_BUILD="%APPVEYOR%"




@rem At first, check the Windows environment
echo %LINEBEG% Checking for Windows environment...

setlocal
for /f "tokens=4-5 delims=. " %%i in ('ver') do set VERSION=%%i.%%j
if "%VERSION%" == "10.0" (
    echo %CHECKMARK% Windows 10 is supported.
) else (
    echo %CROSSMARK% Your Windows Version %VERSION% is not supported. Aborting. You may have to install all dependencies manually.
)
endlocal

if "%PROGRAMFILES%" == "C:\Program Files" (
    echo %CHECKMARK% 64 bit is supported.
) else (
    echo %CROSSMARK% 32 bit is not supported. Aborting. You may have to install all dependencies manually.
)




@rem Then, check for required tools
echo %LINEBEG% Checking for required tools...

where /q git
if %errorLevel% == 0 (
    echo %CHECKMARK% Git is installed and found.
) else (
    echo %CROSSMARK% Git is not installed or not inside Windows PATH. Aborting. Please install Git from https://git-scm.com/download/win
    goto end
)

where /q cmake
if %errorLevel% == 0 (
    echo %CHECKMARK% CMake is installed and found.
) else (
    echo %CROSSMARK% CMake is not installed or not inside Windows PATH. Aborting. Please install CMake from https://cmake.org/download
    goto end
)

where /q ninja
if %errorLevel% == 0 (
    echo %CHECKMARK% Ninja is installed and found.
) else (
    echo %CROSSMARK% Ninja is not installed or not inside Windows PATH. Aborting. Please install Ninja from https://github.com/ninja-build/ninja/releases
    goto end
)

where /q nuget
if %errorLevel% == 0 (
    echo %CHECKMARK% NuGet is installed and found.
) else (
    echo %CROSSMARK% NuGet is not installed or not inside Windows PATH. Aborting. Please install NuGet from https://www.nuget.org/downloads
    goto end
)




@rem Install required libs...
echo %LINEBEG% Checking for required libraries...

if not exist thirdparty\manual-build (
    setlocal enableextensions
    md thirdparty\manual-build
    if not exist thirdparty\manual-build\lib (
        mkdir thirdparty\manual-build\lib
    )
    endlocal
)
@rd /s /q thirdparty\manual-build\precompiled
mkdir thirdparty\manual-build\precompiled


echo %LINEBEG% Prebuilt dependencies for 64-bit Windows...
if not exist thirdparty\manual-build\lib\win64-prebuilt-deps (
    git clone https://github.com/openhoi/win64-prebuilt-deps --depth 1 thirdparty\manual-build\lib\win64-prebuilt-deps
    cd thirdparty\manual-build\lib\win64-prebuilt-deps
) else (
    git -C thirdparty\manual-build\lib\win64-prebuilt-deps reset --hard
    cd thirdparty\manual-build\lib\win64-prebuilt-deps
)
git reset --hard
git pull --depth 1
robocopy "%CD%\gmp\lib" "%CWD%\thirdparty\manual-build\precompiled\gmp\lib" libgmp-10.lib libmpfr-4.lib
robocopy "%CD%\gmp\lib" "%CWD%\thirdparty\manual-build\precompiled\gmp\bin" libgmp-10.dll libmpfr-4.dll
robocopy "%CD%\gmp\include" "%CWD%\thirdparty\manual-build\precompiled\gmp\include" /mir
cd %CWD%

echo %LINEBEG% Boost...
set BOOST_NAME=boost-vc141
set BOOST_VERSION=1.70.0
nuget install %BOOST_NAME% -Version %BOOST_VERSION% -OutputDirectory thirdparty\manual-build\lib
robocopy "thirdparty\manual-build\lib\boost.%BOOST_VERSION%.0\lib\native\include" "%CWD%\thirdparty\manual-build\precompiled\boost\include" /mir
set BOOST_PO_NAME=boost_program_options-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_PO_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_PO_NAME%-mt-gd-x64-1_70.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_PO_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_PO_NAME%-mt-gd-x64-1_70.dll
set BOOST_LOG_NAME=boost_log-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_LOG_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_LOG_NAME%-mt-gd-x64-1_70.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_LOG_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_LOG_NAME%-mt-gd-x64-1_70.dll
set BOOST_LS_NAME=boost_log_setup-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_LS_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_LS_NAME%-mt-gd-x64-1_70.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_LS_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_LS_NAME%-mt-gd-x64-1_70.dll
set BOOST_LOC_NAME=boost_locale-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_LOC_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_LOC_NAME%-mt-gd-x64-1_70.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_LOC_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_LOC_NAME%-mt-gd-x64-1_70.dll
set BOOST_DT_NAME=boost_date_time-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_DT_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_DT_NAME%-mt-gd-x64-1_70.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_DT_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_DT_NAME%-mt-gd-x64-1_70.dll
set BOOST_FS_NAME=boost_filesystem-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_FS_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_FS_NAME%-mt-gd-x64-1_70.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_FS_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_FS_NAME%-mt-gd-x64-1_70.dll
set BOOST_TH_NAME=boost_thread-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_TH_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_TH_NAME%-mt-gd-x64-1_70.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_TH_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_TH_NAME%-mt-gd-x64-1_70.dll
set BOOST_RE_NAME=boost_regex-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_RE_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_RE_NAME%-mt-gd-x64-1_70.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_RE_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_RE_NAME%-mt-gd-x64-1_70.dll
set BOOST_CR_NAME=boost_chrono-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_CR_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_CR_NAME%-mt-gd-x64-1_70.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_CR_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_CR_NAME%-mt-gd-x64-1_70.dll
set BOOST_ATO_NAME=boost_atomic-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_ATO_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_ATO_NAME%-mt-gd-x64-1_70.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_ATO_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_ATO_NAME%-mt-gd-x64-1_70.dll
set BOOST_SYS_NAME=boost_system-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_SYS_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_SYS_NAME%-mt-gd-x64-1_70.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_SYS_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_SYS_NAME%-mt-gd-x64-1_70.dll

echo %LINEBEG% OpenSSL...
set OPENSSL_NAME=openssl-vc140-vc141-x86_64
set OPENSSL_VERSION=1.1.4
nuget install %OPENSSL_NAME% -Version %OPENSSL_VERSION% -OutputDirectory thirdparty\manual-build\lib
setlocal enableextensions
md "%CWD%\thirdparty\manual-build\precompiled\openssl\lib"
md "%CWD%\thirdparty\manual-build\precompiled\openssl\bin"
endlocal
copy "%CWD%\thirdparty\manual-build\lib\%OPENSSL_NAME%.%OPENSSL_VERSION%\build\native\lib\vc141\x64\libcrypto.lib" "%CWD%\thirdparty\manual-build\precompiled\openssl\lib\libcrypto.lib"
copy "%CWD%\thirdparty\manual-build\lib\%OPENSSL_NAME%.%OPENSSL_VERSION%\build\native\lib\vc141\x64\libssl.lib" "%CWD%\thirdparty\manual-build\precompiled\openssl\lib\libssl.lib"
copy "%CWD%\thirdparty\manual-build\lib\%OPENSSL_NAME%.%OPENSSL_VERSION%\build\native\lib\vc141\x64\libcrypto-1_1-x64.dll" "%CWD%\thirdparty\manual-build\precompiled\openssl\bin\libcrypto-1_1-x64.dll"
copy "%CWD%\thirdparty\manual-build\lib\%OPENSSL_NAME%.%OPENSSL_VERSION%\build\native\lib\vc141\x64\libssl-1_1-x64.dll" "%CWD%\thirdparty\manual-build\precompiled\openssl\bin\libssl-1_1-x64.dll"
robocopy "%CWD%\thirdparty\manual-build\lib\%OPENSSL_NAME%.%OPENSSL_VERSION%\build\native\include" "%CWD%\thirdparty\manual-build\precompiled\openssl\include" /mir

echo %LINEBEG% OpenAL...
set OPENAL_BRANCH=openal-soft-1.19.1
if not exist thirdparty\manual-build\lib\openal (
    git clone https://github.com/kcat/openal-soft --depth 1 --branch %OPENAL_BRANCH% thirdparty\manual-build\lib\openal
) else (
    git -C thirdparty\manual-build\lib\openal reset --hard
    git -C thirdparty\manual-build\lib\openal fetch --tags
    git -C thirdparty\manual-build\lib\openal checkout %OPENAL_BRANCH%
    git -C thirdparty\manual-build\lib\openal pull --depth 1
)
cd thirdparty\manual-build\lib\openal
@rd /s /q build
git reset --hard
cd build
cmake -DALSOFT_TESTS=OFF -DALSOFT_UTILS=OFF -DCMAKE_BUILD_TYPE=Release -G Ninja ..
ninja
robocopy "%CD%" "%CWD%\thirdparty\manual-build\precompiled\openal\lib" OpenAL32.lib
robocopy "%CD%" "%CWD%\thirdparty\manual-build\precompiled\openal\bin" OpenAL32.dll
cd ..
robocopy "include" "%CWD%\thirdparty\manual-build\precompiled\openal\include" /mir
cd %CWD%

echo %LINEBEG% RapidJSON...
set RAPIDJSON_NAME=rapidjson.v110
set RAPIDJSON_VERSION=1.1.0
nuget install %RAPIDJSON_NAME% -Version %RAPIDJSON_VERSION% -OutputDirectory thirdparty\manual-build\lib
robocopy "%CWD%\thirdparty\manual-build\lib\%RAPIDJSON_NAME%.%RAPIDJSON_VERSION%\build\native\include" "%CWD%\thirdparty\manual-build\precompiled\rapidjson\include" /mir
rem TODO: Use github instead because there is no DLL...

echo %LINEBEG% zlib...
set ZLIB_NAME=zlib-msvc-x64
set ZLIB_VERSION=1.2.11.8900
nuget install %ZLIB_NAME% -Version %ZLIB_VERSION% -OutputDirectory thirdparty\manual-build\lib
robocopy "%CWD%\thirdparty\manual-build\lib\%ZLIB_NAME%.%ZLIB_VERSION%\build\native\lib_release" "%CWD%\thirdparty\manual-build\precompiled\zlib\lib" zlib.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%ZLIB_NAME%.%ZLIB_VERSION%\build\native\bin_release" "%CWD%\thirdparty\manual-build\precompiled\zlib\bin" zlib.dll
robocopy "%CWD%\thirdparty\manual-build\lib\%ZLIB_NAME%.%ZLIB_VERSION%\build\native\include" "%CWD%\thirdparty\manual-build\precompiled\zlib\include" /mir

echo %LINEBEG% Eigen...
set EIGEN_BRANCH=3.3.7
if not exist thirdparty\manual-build\lib\eigen (
    git clone https://github.com/eigenteam/eigen-git-mirror --depth 1 --branch %EIGEN_BRANCH% thirdparty\manual-build\lib\eigen
) else (
    git -C thirdparty\manual-build\lib\eigen reset --hard
    git -C thirdparty\manual-build\lib\eigen fetch
    git -C thirdparty\manual-build\lib\eigen checkout %EIGEN_BRANCH%
    git -C thirdparty\manual-build\lib\eigen pull --depth 1
)
cd thirdparty\manual-build\lib\eigen
@rd /s /q build
mkdir build
cd build
cmake -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release -G Ninja ..
ninja
cd %CWD%

echo %LINEBEG% CGAL...
@rd /s /q thirdparty\manual-build\lib\cgal
rem We need to download the source ZIP because the directory structure is different to the GitHub/development version
rem See https://www.cgal.org/download/windows.html
set CGAL_VERSION=4.13
powershell -Command "Invoke-WebRequest https://github.com/CGAL/cgal/releases/download/releases/CGAL-%CGAL_VERSION%/CGAL-%CGAL_VERSION%.zip -OutFile %CWD%\thirdparty\manual-build\lib\cgal.zip"
powershell -Command "Expand-Archive -Force %CWD%\thirdparty\manual-build\lib\cgal.zip %CWD%\thirdparty\manual-build\lib"
move "%CWD%\thirdparty\manual-build\lib\CGAL-%CGAL_VERSION%" "%CWD%\thirdparty\manual-build\lib\cgal"
cd %CWD%\thirdparty\manual-build\lib\cgal
@rd /s /q build
mkdir build
cd build
set CWD_FS=%CWD:\=/%
cmake -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=OFF -DCGAL_HEADER_ONLY=OFF -DWITH_CGAL_Qt5=OFF -DCMAKE_INSTALL_PREFIX=%CWD_FS%/thirdparty/manual-build/precompiled/cgal -DBOOST_INCLUDEDIR=%CWD_FS%/thirdparty/manual-build/precompiled/boost/include -DGMP_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/include -DGMP_LIBRARIES=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/lib/libgmp-10.lib -DMPFR_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/include -DMPFR_LIBRARIES=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/lib/libmpfr-4.lib -DWITH_Eigen3=ON -DEIGEN3_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/lib/eigen -DCMAKE_BUILD_TYPE=Debug -G Ninja ..
ninja
ninja install
cd ..
@rd /s /q build
mkdir build
cd build
cmake -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=OFF -DCGAL_HEADER_ONLY=OFF -DWITH_CGAL_Qt5=OFF -DCMAKE_INSTALL_PREFIX=%CWD_FS%/thirdparty/manual-build/precompiled/cgal -DBOOST_INCLUDEDIR=%CWD_FS%/thirdparty/manual-build/precompiled/boost/include -DGMP_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/include -DGMP_LIBRARIES=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/lib/libgmp-10.lib -DMPFR_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/include -DMPFR_LIBRARIES=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/lib/libmpfr-4.lib -DWITH_Eigen3=ON -DEIGEN3_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/lib/eigen -DCMAKE_BUILD_TYPE=Release -G Ninja ..
ninja
robocopy "%CWD%\thirdparty\manual-build\lib\cgal\build\include\CGAL" "%CWD%\thirdparty\manual-build\precompiled\cgal\include\CGAL" compiler_config.h
robocopy "%CWD%\thirdparty\manual-build\lib\cgal\build\include\CGAL" "%CWD%\thirdparty\manual-build\lib\cgal\include\CGAL" compiler_config.h
cd %CWD%

echo %LINEBEG% V8...
set V8_NAME=v8-v141-x64
set V8_REDIST_NAME=v8.redist-v141-x64
set V8_VERSION=7.3.492.21
nuget install %V8_NAME% -Version %V8_VERSION% -OutputDirectory thirdparty\manual-build\lib
robocopy "%CWD%\thirdparty\manual-build\lib\%V8_NAME%.%V8_VERSION%\lib\Release" "%CWD%\thirdparty\manual-build\precompiled\v8\lib" /mir
robocopy "%CWD%\thirdparty\manual-build\lib\%V8_NAME%.%V8_VERSION%\include" "%CWD%\thirdparty\manual-build\precompiled\v8\include" /mir
robocopy "%CWD%\thirdparty\manual-build\lib\%V8_REDIST_NAME%.%V8_VERSION%\lib\Release" "%CWD%\thirdparty\manual-build\precompiled\v8\bin" /mir
cd %CWD%

echo %LINEBEG% FreeType...
set FREETYPE_NAME=freetype
set FREETYPE_VERSION=2.8.0.1
nuget install %FREETYPE_NAME% -Version %FREETYPE_VERSION% -OutputDirectory thirdparty\manual-build\lib
robocopy "%CWD%\thirdparty\manual-build\lib\%FREETYPE_NAME%.%FREETYPE_VERSION%\build\native\lib\x64\v141\dynamic\Release" "%CWD%\thirdparty\manual-build\precompiled\freetype\lib" /mir
robocopy "%CWD%\thirdparty\manual-build\lib\%FREETYPE_NAME%.redist.%FREETYPE_VERSION%\build\native\bin\x64\v141\dynamic\Release" "%CWD%\thirdparty\manual-build\precompiled\freetype\bin" /mir
robocopy "%CWD%\thirdparty\manual-build\lib\%FREETYPE_NAME%.%FREETYPE_VERSION%\build\native\include" "%CWD%\thirdparty\manual-build\precompiled\freetype\include" /mir
cd %CWD%

echo %LINEBEG% FreeImage...
set FREEIMAGE_NAME=native.freeimage
set FREEIMAGE_VERSION=3.17.0
nuget install %FREEIMAGE_NAME% -Version %FREEIMAGE_VERSION% -OutputDirectory thirdparty\manual-build\lib
robocopy "%CWD%\thirdparty\manual-build\lib\%FREEIMAGE_NAME%.%FREEIMAGE_VERSION%\build\native\lib\x64\dynamic" "%CWD%\thirdparty\manual-build\precompiled\freeimage\lib" /mir
robocopy "%CWD%\thirdparty\manual-build\lib\%FREEIMAGE_NAME%.redist.%FREEIMAGE_VERSION%\build\native\bin\x64\dynamic" "%CWD%\thirdparty\manual-build\precompiled\freeimage\bin" /mir
robocopy "%CWD%\thirdparty\manual-build\lib\%FREEIMAGE_NAME%.%FREEIMAGE_VERSION%\build\native\include" "%CWD%\thirdparty\manual-build\precompiled\freeimage\include" /mir
cd %CWD%

echo %LINEBEG% Ogre3D...
set OGRE_BRANCH=v1.12.0
if not exist thirdparty\manual-build\lib\ogre3d (
    git clone https://github.com/OGRECave/ogre --depth 1 --branch %OGRE_BRANCH% thirdparty\manual-build\lib\ogre3d
) else (
    git -C thirdparty\manual-build\lib\ogre3d reset --hard
    git -C thirdparty\manual-build\lib\ogre3d fetch
    git -C thirdparty\manual-build\lib\ogre3d checkout %OGRE_BRANCH%
    git -C thirdparty\manual-build\lib\ogre3d pull --depth 1
)
cd thirdparty\manual-build\lib\ogre3d
@rd /s /q build
mkdir build
cd build
rem OGRE 2: cmake -DOGRE_BUILD_TESTS=OFF -DOGRE_BUILD_SAMPLES2=OFF -DOGRE_INSTALL_SAMPLES=OFF -DOGRE_INSTALL_SAMPLES_SOURCE=OFF -DOGRE_BUILD_TOOLS=OFF -DOGRE_INSTALL_TOOLS=OFF -DOGRE_LEGACY_ANIMATIONS=OFF -DOGRE_CONFIG_DOUBLE=OFF -DOGRE_CONFIG_ENABLE_DDS=OFF -DOGRE_STATIC=OFF -DOGRE_COPY_DEPENDENCIES=OFF -DOGRE_INSTALL_DEPENDENCIES=OFF -DRapidjson_INCLUDE_DIR="%CWD%\thirdparty\manual-build\precompiled\rapidjson\include" -DZLIB_INCLUDE_DIR="%CWD%\thirdparty\manual-build\precompiled\zlib\include" -DZLIB_LIBRARY_REL="%CWD%\thirdparty\manual-build\precompiled\zlib\lib\zlib.lib" -DFREETYPE_LIBRARY_REL="%CWD%\thirdparty\manual-build\precompiled\freetype\lib\freetype28.lib" -DFREETYPE_INCLUDE_DIR="%CWD%\thirdparty\manual-build\precompiled\freetype\include" -DFREETYPE_FT2BUILD_INCLUDE_DIR="%CWD%\thirdparty\manual-build\precompiled\freetype\include" -DFreeImage_INCLUDE_DIR="%CWD%\thirdparty\manual-build\precompiled\freeimage\include" -DFreeImage_LIBRARY_REL="%CWD%\thirdparty\manual-build\precompiled\freeimage\lib\FreeImage.lib" -DCMAKE_BUILD_TYPE=Release -G Ninja ..
cmake -DOGRE_BUILD_TESTS=OFF -DOGRE_BUILD_SAMPLES=OFF -DOGRE_INSTALL_SAMPLES=OFF -DOGRE_INSTALL_SAMPLES_SOURCE=OFF -DOGRE_BUILD_TOOLS=OFF -DOGRE_BUILD_TOOLS=OFF -DOGRE_INSTALL_TOOLS=OFF -DOGRE_CONFIG_DOUBLE=OFF -DOGRE_CONFIG_ENABLE_DDS=OFF -DOGRE_CONFIG_ENABLE_ZIP=OFF -DOGRE_CONFIG_ENABLE_ETC=OFF -DOGRE_STATIC=OFF -DOGRE_COPY_DEPENDENCIES=OFF -DOGRE_INSTALL_DEPENDENCIES=OFF -DZLIB_INCLUDE_DIR="%CWD%\thirdparty\manual-build\precompiled\zlib\include" -DZLIB_LIBRARY_RELEASE="%CWD%\thirdparty\manual-build\precompiled\zlib\lib\zlib.lib" -DFREETYPE_LIBRARY_RELEASE="%CWD%\thirdparty\manual-build\precompiled\freetype\lib\freetype28.lib" -DFREETYPE_INCLUDE_DIR_freetype2="%CWD%\thirdparty\manual-build\precompiled\freetype\include" -DFREETYPE_INCLUDE_DIR_ft2build="%CWD%\thirdparty\manual-build\precompiled\freetype\include"-DFreeImage_INCLUDE_DIR="%CWD%\thirdparty\manual-build\precompiled\freeimage\include" -DFreeImage_LIBRARY_REL="%CWD%\thirdparty\manual-build\precompiled\freeimage\lib\FreeImage.lib" -DOGRE_BUILD_PLUGIN_BSP=OFF -DOGRE_BUILD_PLUGIN_OCTREE=OFF -DOGRE_BUILD_PLUGIN_PCZ=OFF -DOGRE_BUILD_PLUGIN_PFX=OFF -DOGRE_BUILD_COMPONENT_PAGING=OFF -DOGRE_BUILD_COMPONENT_TERRAIN=OFF -DOGRE_BUILD_COMPONENT_RTSHADERSYSTEM=OFF -DOGRE_BUILD_COMPONENT_VOLUME=OFF -DOGRE_BUILD_RENDERSYSTEM_GL=OFF -DOGRE_BUILD_RENDERSYSTEM_GL3PLUS=OFF -DOGRE_BUILD_RENDERSYSTEM_D3D9=OFF -DCMAKE_BUILD_TYPE=Release -G Ninja ..
ninja
ninja install
robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\sdk\include" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\include" /mir
robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\sdk\lib" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\lib" OgreBites.lib OgreGLSupport.lib OgreHLMS.lib OgreMain.lib OgreMeshLodGenerator.lib OgreOverlay.lib OgreProperty.lib
robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\sdk\lib\OGRE" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\lib" Codec_STBI.lib RenderSystem_Direct3D11.lib RenderSystem_GL3Plus.lib
robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\sdk\bin" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\bin" Codec_STBI.dll OgreBites.dll OgreHLMS.dll OgreMain.dll OgreMeshLodGenerator.dll OgreOverlay.dll OgreProperty.dll RenderSystem_Direct3D11.dll
cd %CWD%


:end


@rem Pause when we are not in AppVeyor CI
if /I "%CI_BUILD%" neq "true" (
    pause
)
