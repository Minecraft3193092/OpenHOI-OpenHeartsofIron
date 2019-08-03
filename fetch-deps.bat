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

where /q clang-format
if %errorLevel% == 0 (
    echo %CHECKMARK% Clang-format is installed and found.
) else (
    echo %CROSSMARK% Clang-format is not installed or not inside Windows PATH. Aborting. Please install Clang-format from https://llvm.org/builds
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

echo %LINEBEG% GMP / MPFR...
robocopy "%CD%\gmp\lib" "%CWD%\thirdparty\manual-build\precompiled\gmp\lib" libgmp-10.lib libmpfr-4.lib
robocopy "%CD%\gmp\lib" "%CWD%\thirdparty\manual-build\precompiled\gmp\bin" libgmp-10.dll libmpfr-4.dll
robocopy "%CD%\gmp\include" "%CWD%\thirdparty\manual-build\precompiled\gmp\include" /mir

echo %LINEBEG% OGRE...
robocopy "%CD%\ogre3d" "%CWD%\thirdparty\manual-build\precompiled\ogre3d" /mir
cd %CWD%

echo %LINEBEG% Boost...
set BOOST_NAME=boost
set BOOST_VERSION=1.69.0
nuget install %BOOST_NAME% -Version %BOOST_VERSION% -OutputDirectory thirdparty\manual-build\lib
nuget install boost-vc141 -Version %BOOST_VERSION% -OutputDirectory thirdparty\manual-build\lib
robocopy "thirdparty\manual-build\lib\%BOOST_NAME%.%BOOST_VERSION%.0\lib\native\include" "%CWD%\thirdparty\manual-build\precompiled\boost\include" /mir
set BOOST_PO_NAME=boost_program_options-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_PO_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_PO_NAME%-mt-x64-1_69.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_PO_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_PO_NAME%-mt-x64-1_69.dll
set BOOST_LS_NAME=boost_log_setup-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_LS_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_LS_NAME%-mt-x64-1_69.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_LS_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_LS_NAME%-mt-x64-1_69.dll
set BOOST_LOC_NAME=boost_locale-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_LOC_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_LOC_NAME%-mt-x64-1_69.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_LOC_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_LOC_NAME%-mt-x64-1_69.dll
set BOOST_DT_NAME=boost_date_time-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_DT_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_DT_NAME%-mt-x64-1_69.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_DT_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_DT_NAME%-mt-x64-1_69.dll
set BOOST_FS_NAME=boost_filesystem-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_FS_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_FS_NAME%-mt-x64-1_69.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_FS_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_FS_NAME%-mt-x64-1_69.dll
set BOOST_TH_NAME=boost_thread-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_TH_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_TH_NAME%-mt-x64-1_69.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_TH_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_TH_NAME%-mt-x64-1_69.dll
set BOOST_RE_NAME=boost_regex-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_RE_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_RE_NAME%-mt-x64-1_69.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_RE_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_RE_NAME%-mt-x64-1_69.dll
set BOOST_CR_NAME=boost_chrono-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_CR_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_CR_NAME%-mt-x64-1_69.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_CR_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_CR_NAME%-mt-x64-1_69.dll
set BOOST_ATO_NAME=boost_atomic-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_ATO_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_ATO_NAME%-mt-x64-1_69.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_ATO_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_ATO_NAME%-mt-x64-1_69.dll
set BOOST_SYS_NAME=boost_system-vc141
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_SYS_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\lib" %BOOST_SYS_NAME%-mt-x64-1_69.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%BOOST_SYS_NAME%.%BOOST_VERSION%.0\lib\native" "%CWD%\thirdparty\manual-build\precompiled\boost\bin" %BOOST_SYS_NAME%-mt-x64-1_69.dll


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
set CGAL_VERSION=4.14
echo Downloading CGAL v%CGAL_VERSION%...
powershell -Command "Invoke-WebRequest https://github.com/CGAL/cgal/releases/download/releases/CGAL-%CGAL_VERSION%/CGAL-%CGAL_VERSION%.zip -OutFile %CWD%\thirdparty\manual-build\lib\cgal.zip"
echo Expanding CGAL v%CGAL_VERSION% ZIP archive...
powershell -Command "Expand-Archive -Force %CWD%\thirdparty\manual-build\lib\cgal.zip %CWD%\thirdparty\manual-build\lib"
move "%CWD%\thirdparty\manual-build\lib\CGAL-%CGAL_VERSION%" "%CWD%\thirdparty\manual-build\lib\cgal"
cd %CWD%\thirdparty\manual-build\lib\cgal
@rd /s /q build
mkdir build
cd build
set CWD_FS=%CWD:\=/%
if /I "%CI%" neq "true" (
    cmake -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=OFF -DCGAL_HEADER_ONLY=OFF -DWITH_CGAL_Qt5=OFF -DCMAKE_INSTALL_PREFIX=%CWD_FS%/thirdparty/manual-build/precompiled/cgal -DBOOST_INCLUDEDIR=%CWD_FS%/thirdparty/manual-build/precompiled/boost/include -DGMP_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/include -DGMP_LIBRARIES=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/lib/libgmp-10.lib -DMPFR_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/include -DMPFR_LIBRARIES=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/lib/libmpfr-4.lib -DWITH_Eigen3=ON -DEIGEN3_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/lib/eigen -DCMAKE_BUILD_TYPE=Debug -G Ninja ..
    ninja
    ninja install
    cd ..
    @rd /s /q build
    mkdir build
    cd build
)
cmake -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=OFF -DCGAL_HEADER_ONLY=OFF -DWITH_CGAL_Qt5=OFF -DCMAKE_INSTALL_PREFIX=%CWD_FS%/thirdparty/manual-build/precompiled/cgal -DBOOST_INCLUDEDIR=%CWD_FS%/thirdparty/manual-build/precompiled/boost/include -DGMP_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/include -DGMP_LIBRARIES=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/lib/libgmp-10.lib -DMPFR_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/include -DMPFR_LIBRARIES=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/lib/libmpfr-4.lib -DWITH_Eigen3=ON -DEIGEN3_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/lib/eigen -DCMAKE_BUILD_TYPE=Release -G Ninja ..
ninja
ninja install
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

rem Skip OGRE building because we now provide pre-build binaries via our win64-prebuilt-libs repository
goto skip_ogre
echo %LINEBEG% OGRE...
set OGRE_VERSION=1.12.1
if not exist thirdparty\manual-build\lib\ogre3d (
    git clone https://github.com/OGRECave/ogre --depth 1 --branch v%OGRE_VERSION% thirdparty\manual-build\lib\ogre3d
) else (
    git -C thirdparty\manual-build\lib\ogre3d reset --hard
    git -C thirdparty\manual-build\lib\ogre3d fetch
    git -C thirdparty\manual-build\lib\ogre3d checkout v%OGRE_VERSION%
    git -C thirdparty\manual-build\lib\ogre3d pull --depth 1
)
cd thirdparty\manual-build\lib\ogre3d
@rd /s /q build
mkdir build
cd build
set OGRE_CMAKE_PARAMS=-DOGRE_BUILD_TESTS=OFF -DOGRE_BUILD_SAMPLES=OFF -DOGRE_INSTALL_SAMPLES=OFF -DOGRE_INSTALL_SAMPLES_SOURCE=OFF -DOGRE_BUILD_TOOLS=OFF -DOGRE_BUILD_TOOLS=OFF -DOGRE_INSTALL_TOOLS=OFF -DOGRE_INSTALL_DOCS=OFF -DOGRE_INSTALL_PDB=OFF -DOGRE_CONFIG_DOUBLE=OFF -DOGRE_CONFIG_ENABLE_DDS=OFF -DOGRE_CONFIG_ENABLE_ZIP=OFF -DOGRE_CONFIG_ENABLE_ETC=OFF -DOGRE_STATIC=OFF -DOGRE_COPY_DEPENDENCIES=OFF -DOGRE_INSTALL_DEPENDENCIES=OFF -DZLIB_INCLUDE_DIR="%CWD%\thirdparty\manual-build\precompiled\zlib\include" -DZLIB_LIBRARY_RELEASE="%CWD%\thirdparty\manual-build\precompiled\zlib\lib\zlib.lib" -DOGRE_BUILD_PLUGIN_BSP=OFF -DOGRE_BUILD_PLUGIN_OCTREE=OFF -DOGRE_BUILD_PLUGIN_PCZ=OFF -DOGRE_BUILD_PLUGIN_PFX=OFF -DOGRE_BUILD_COMPONENT_PAGING=OFF -DOGRE_BUILD_COMPONENT_TERRAIN=OFF -DOGRE_BUILD_COMPONENT_RTSHADERSYSTEM=ON -DOGRE_BUILD_COMPONENT_VOLUME=OFF -DOGRE_BUILD_RENDERSYSTEM_GL=OFF -DOGRE_BUILD_RENDERSYSTEM_GL3PLUS=OFF -DOGRE_BUILD_RENDERSYSTEM_D3D9=OFF
rem Build Release
cmake %OGRE_CMAKE_PARAMS% -DCMAKE_BUILD_TYPE=Release -G Ninja ..
ninja
ninja install
robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\sdk\include" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\include" /mir
robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\sdk\lib" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\lib" OgreBites.lib OgreHLMS.lib OgreMain.lib OgreMeshLodGenerator.lib OgreOverlay.lib OgreProperty.lib OgreRTShaderSystem.lib
robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\sdk\lib\OGRE" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\lib" Codec_STBI.lib RenderSystem_Direct3D11.lib
robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\sdk\bin" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\bin" Codec_STBI.dll OgreBites.dll OgreHLMS.dll OgreMain.dll OgreMeshLodGenerator.dll OgreOverlay.dll OgreProperty.dll OgreRTShaderSystem.dll RenderSystem_Direct3D11.dll
robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\Dependencies\bin" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\bin" SDL2.dll
if /I NOT "%CI%" == "True" (
    rem Build Debug
    cmake %OGRE_CMAKE_PARAMS% -DCMAKE_BUILD_TYPE=Debug -G Ninja ..
    ninja
    ninja install
    robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\sdk\lib" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\lib" OgreBites_d.lib OgreHLMS_d.lib OgreMain_d.lib OgreMeshLodGenerator_d.lib OgreOverlay_d.lib OgreProperty_d.lib OgreRTShaderSystem_d.lib
    robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\sdk\lib\OGRE" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\lib" Codec_STBI_d.lib RenderSystem_Direct3D11_d.lib
    robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\sdk\bin" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\bin" Codec_STBI_d.dll OgreBites_d.dll OgreHLMS_d.dll OgreMain_D.dll OgreMeshLodGenerator_d.dll OgreOverlay_d.dll OgreProperty_d.dll OgreRTShaderSystem_d.dll RenderSystem_Direct3D11_d.dll
)
cd %CWD%
:skip_ogre

:end