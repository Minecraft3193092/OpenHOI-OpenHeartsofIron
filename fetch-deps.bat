@echo off

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
set BUILD_OGRE=n



@rem At first, check the Windows environment
echo %LINEBEG% Checking for Windows environment...

setlocal
for /f "tokens=4-5 delims=. " %%i in ('ver') do set VERSION=%%i.%%j
if "%VERSION%" == "10.0" (
    echo %CHECKMARK% Windows 10 is supported.
) else (
    echo %CROSSMARK% Your Windows Version %VERSION% is not supported. Aborting. You may have to install all dependencies manually.
    goto end
)
endlocal

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


@rem Cleanup and sync submodules
git submodule sync
git submodule update --init --recursive --remote
git -C thirdparty\ogre-package\imgui reset --hard
git -C thirdparty\ogre-package\imgui pull origin docking


@rem Install required libs...
echo %LINEBEG% Checking for required libraries...

if not exist thirdparty\manual-build (
    setlocal enableextensions
    mkdir thirdparty\manual-build
    if not exist thirdparty\manual-build\lib (
        mkdir thirdparty\manual-build\lib
    )
    endlocal
)
@rd /s /q thirdparty\manual-build\precompiled 2>nul
mkdir thirdparty\manual-build\precompiled


echo %LINEBEG% Prebuilt dependencies for 64-bit Windows...
if not exist thirdparty\manual-build\lib\win64-prebuilt-deps (
    git clone https://github.com/openhoi/win64-prebuilt-deps thirdparty\manual-build\lib\win64-prebuilt-deps
) else (
    git -C thirdparty\manual-build\lib\win64-prebuilt-deps reset --hard
    git -C thirdparty\manual-build\lib\win64-prebuilt-deps pull
)
cd thirdparty\manual-build\lib\win64-prebuilt-deps

echo %LINEBEG% GMP / MPFR...
robocopy "%CD%\gmp\lib" "%CWD%\thirdparty\manual-build\precompiled\gmp\lib" libgmp-10.lib libmpfr-4.lib
robocopy "%CD%\gmp\lib" "%CWD%\thirdparty\manual-build\precompiled\gmp\bin" libgmp-10.dll libmpfr-4.dll
robocopy "%CD%\gmp\include" "%CWD%\thirdparty\manual-build\precompiled\gmp\include" /mir

if "%BUILD_OGRE%" neq "y" (
    echo %LINEBEG% OGRE...
    robocopy "%CD%\ogre3d" "%CWD%\thirdparty\manual-build\precompiled\ogre3d" /mir
    echo Expanding OgreMain_d.pdb.zip archive...
    powershell -Command "Expand-Archive -Force %CWD%\thirdparty\manual-build\precompiled\ogre3d\bin\OgreMain_d.pdb.zip %CWD%\thirdparty\manual-build\precompiled\ogre3d\bin"
    del %CWD%\thirdparty\manual-build\precompiled\ogre3d\bin\OgreMain_d.pdb.zip

    echo %LINEBEG% SDL2...
    robocopy "%CD%\sdl2" "%CWD%\thirdparty\manual-build\precompiled\sdl2" /mir
)
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
mkdir "%CWD%\thirdparty\manual-build\precompiled\openssl\lib"
mkdir "%CWD%\thirdparty\manual-build\precompiled\openssl\bin"
endlocal
copy "%CWD%\thirdparty\manual-build\lib\%OPENSSL_NAME%.%OPENSSL_VERSION%\build\native\lib\vc141\x64\libcrypto.lib" "%CWD%\thirdparty\manual-build\precompiled\openssl\lib\libcrypto.lib"
copy "%CWD%\thirdparty\manual-build\lib\%OPENSSL_NAME%.%OPENSSL_VERSION%\build\native\lib\vc141\x64\libssl.lib" "%CWD%\thirdparty\manual-build\precompiled\openssl\lib\libssl.lib"
copy "%CWD%\thirdparty\manual-build\lib\%OPENSSL_NAME%.%OPENSSL_VERSION%\build\native\lib\vc141\x64\libcrypto-1_1-x64.dll" "%CWD%\thirdparty\manual-build\precompiled\openssl\bin\libcrypto-1_1-x64.dll"
copy "%CWD%\thirdparty\manual-build\lib\%OPENSSL_NAME%.%OPENSSL_VERSION%\build\native\lib\vc141\x64\libssl-1_1-x64.dll" "%CWD%\thirdparty\manual-build\precompiled\openssl\bin\libssl-1_1-x64.dll"
robocopy "%CWD%\thirdparty\manual-build\lib\%OPENSSL_NAME%.%OPENSSL_VERSION%\build\native\include" "%CWD%\thirdparty\manual-build\precompiled\openssl\include" /mir

echo %LINEBEG% OpenAL...
set OPENAL_BRANCH=openal-soft-1.19.1
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
robocopy "%CD%" "%CWD%\thirdparty\manual-build\precompiled\openal\lib" OpenAL32.lib
robocopy "%CD%" "%CWD%\thirdparty\manual-build\precompiled\openal\bin" OpenAL32.dll
cd ..
robocopy "include" "%CWD%\thirdparty\manual-build\precompiled\openal\include" /mir
cd %CWD%

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

echo %LINEBEG% CGAL...
@rd /s /q thirdparty\manual-build\lib\cgal 2>nul
rem We need to download the source ZIP because the directory structure is different to the GitHub/development version
rem See https://www.cgal.org/download/windows.html
set CGAL_VERSION=5.0
echo Downloading CGAL v%CGAL_VERSION%...
powershell -Command "Invoke-WebRequest https://github.com/CGAL/cgal/releases/download/releases/CGAL-%CGAL_VERSION%/CGAL-%CGAL_VERSION%.zip -OutFile %CWD%\thirdparty\manual-build\lib\cgal.zip"
echo Expanding CGAL v%CGAL_VERSION% ZIP archive...
powershell -Command "Expand-Archive -Force %CWD%\thirdparty\manual-build\lib\cgal.zip %CWD%\thirdparty\manual-build\lib"
move "%CWD%\thirdparty\manual-build\lib\CGAL-%CGAL_VERSION%" "%CWD%\thirdparty\manual-build\lib\cgal"
cd %CWD%\thirdparty\manual-build\lib\cgal
@rd /s /q build 2>nul
mkdir build
cd build
set CWD_FS=%CWD:\=/%
if /I "%CI%" neq "true" (
    cmake -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=ON -DCGAL_HEADER_ONLY=OFF -DWITH_CGAL_Qt5=OFF -DCMAKE_INSTALL_PREFIX=%CWD_FS%/thirdparty/manual-build/precompiled/cgal -DBOOST_INCLUDEDIR=%CWD_FS%/thirdparty/manual-build/precompiled/boost/include -DGMP_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/include -DGMP_LIBRARIES=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/lib/libgmp-10.lib -DMPFR_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/include -DMPFR_LIBRARIES=%CWD_FS%/thirdparty/manual-build/precompiled/gmp/lib/libmpfr-4.lib -DWITH_Eigen3=ON -DEIGEN3_INCLUDE_DIR=%CWD_FS%/thirdparty/manual-build/lib/eigen -DCMAKE_BUILD_TYPE=Debug -G Ninja ..
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

if "%BUILD_OGRE%" == "y" (
    echo %LINEBEG% OGRE...
    set OGRE_VERSION=1.12.4
    if not exist thirdparty\manual-build\lib\ogre3d (
        git clone https://github.com/OGRECave/ogre --branch v%OGRE_VERSION% thirdparty\manual-build\lib\ogre3d
    ) else (
        git -C thirdparty\manual-build\lib\ogre3d reset --hard
        git -C thirdparty\manual-build\lib\ogre3d fetch
        git -C thirdparty\manual-build\lib\ogre3d checkout v%OGRE_VERSION%
        git -C thirdparty\manual-build\lib\ogre3d pull
    )
    cd thirdparty\manual-build\lib\ogre3d
    @rd /s /q Components\Overlay\src\imgui 2>nul
    mklink /D Components\Overlay\src\imgui %CWD%\thirdparty\imgui
    @rd /s /q build 2>nul
    mkdir build
    cd build
    set OGRE_CMAKE_PARAMS=-DOGRE_BUILD_TESTS=OFF -DOGRE_BUILD_SAMPLES=OFF -DOGRE_INSTALL_SAMPLES=OFF -DOGRE_INSTALL_SAMPLES_SOURCE=OFF -DOGRE_BUILD_TOOLS=OFF -DOGRE_BUILD_TOOLS=OFF -DOGRE_INSTALL_TOOLS=OFF -DOGRE_INSTALL_DOCS=OFF -DOGRE_INSTALL_PDB=OFF -DOGRE_CONFIG_DOUBLE=OFF -DOGRE_CONFIG_ENABLE_DDS=ON -DOGRE_CONFIG_ENABLE_ETC=OFF -DOGRE_CONFIG_ENABLE_ZIP=OFF -DOGRE_CONFIG_ENABLE_ETC=OFF -DOGRE_STATIC=OFF -DOGRE_COPY_DEPENDENCIES=OFF -DOGRE_INSTALL_DEPENDENCIES=OFF -DZLIB_INCLUDE_DIR="%CWD%\thirdparty\manual-build\precompiled\zlib\include" -DZLIB_LIBRARY_RELEASE="%CWD%\thirdparty\manual-build\precompiled\zlib\lib\zlib.lib" -DOGRE_BUILD_PLUGIN_BSP=OFF -DOGRE_BUILD_PLUGIN_OCTREE=OFF -DOGRE_BUILD_PLUGIN_PCZ=OFF -DOGRE_BUILD_PLUGIN_PFX=ON -DOGRE_BUILD_PLUGIN_DOT_SCENE=OFF -DOGRE_BUILD_COMPONENT_PAGING=OFF -DOGRE_BUILD_COMPONENT_MESHLODGENERATOR=OFF -DOGRE_BUILD_COMPONENT_TERRAIN=OFF -DOGRE_BUILD_COMPONENT_RTSHADERSYSTEM=ON -DOGRE_BUILD_COMPONENT_VOLUME=OFF -DOGRE_BUILD_COMPONENT_BITES=OFF -DOGRE_BUILD_RENDERSYSTEM_GL=ON -DOGRE_BUILD_RENDERSYSTEM_GL3PLUS=ON -DOGRE_BUILD_RENDERSYSTEM_D3D9=OFF -DOGRE_BUILD_COMPONENT_OVERLAY_IMGUI=ON
    rem Build Release
    cmake %OGRE_CMAKE_PARAMS% -DCMAKE_BUILD_TYPE=Release -G Ninja ..
    ninja
    ninja install
    robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\sdk\include" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\include" /mir
    robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\sdk\lib" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\lib" OgreHLMS.lib OgreMain.lib OgreOverlay.lib OgreProperty.lib OgreRTShaderSystem.lib OgreGLSupport.lib
    robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\sdk\lib\OGRE" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\lib" Codec_STBI.lib Plugin_ParticleFX.lib RenderSystem_Direct3D11.lib RenderSystem_GL.lib RenderSystem_GL3Plus.lib
    robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\sdk\bin" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\bin" Codec_STBI.dll OgreBites.dll OgreHLMS.dll OgreMain.dll OgreOverlay.dll OgreProperty.dll OgreRTShaderSystem.dll Plugin_ParticleFX.dll RenderSystem_Direct3D11.dll RenderSystem_GL.dll RenderSystem_GL3Plus.dll
    robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\Dependencies\include\SDL2" "%CWD%\thirdparty\manual-build\precompiled\sdl2\include\SDL2" /mir
    robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\Dependencies\lib" "%CWD%\thirdparty\manual-build\precompiled\sdl2\lib" SDL2.lib SDL2main.lib
    robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\Dependencies\bin" "%CWD%\thirdparty\manual-build\precompiled\sdl2\bin" SDL2.dll
    rem Build Debug
    cmake %OGRE_CMAKE_PARAMS% -DCMAKE_BUILD_TYPE=Debug -G Ninja ..
    ninja
    ninja install
    robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\sdk\lib" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\lib" OgreHLMS_d.lib OgreMain_d.lib OgreOverlay_d.lib OgreProperty_d.lib OgreRTShaderSystem_d.lib OgreGLSupport_d.lib
    robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\sdk\lib\OGRE" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\lib" Codec_STBI_d.lib Plugin_ParticleFX_d.lib RenderSystem_Direct3D11_d.lib RenderSystem_GL_d.lib RenderSystem_GL3Plus_d.lib
    robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\sdk\bin" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\bin" Codec_STBI_d.dll Plugin_ParticleFX_d.dll OgreHLMS_d.dll OgreMain_d.dll OgreOverlay_d.dll OgreProperty_d.dll OgreRTShaderSystem_d.dll RenderSystem_Direct3D11_d.dll RenderSystem_GL_d.dll RenderSystem_GL3Plus_d.dll
    robocopy "%CWD%\thirdparty\manual-build\lib\ogre3d\build\bin" "%CWD%\thirdparty\manual-build\precompiled\ogre3d\bin" Codec_STBI_d.pdb Plugin_ParticleFX_d.pdb OgreHLMS_d.pdb OgreMain_d.pdb OgreOverlay_d.pdb OgreProperty_d.pdb OgreRTShaderSystem_d.pdb RenderSystem_Direct3D11_d.pdb RenderSystem_GL_d.pdb RenderSystem_GL3Plus_d.pdb
    cd %CWD%
)

echo %LINEBEG% RapidJSON...
set RAPIDJSON_BRANCH=v1.1.0
if not exist thirdparty\manual-build\lib\rapidjson (
    git clone https://github.com/Tencent/rapidjson/ --branch %RAPIDJSON_BRANCH% thirdparty\manual-build\lib\rapidjson
) else (
    git -C thirdparty\manual-build\lib\rapidjson reset --hard
    git -C thirdparty\manual-build\lib\rapidjson fetch
    git -C thirdparty\manual-build\lib\rapidjson checkout %EIGEN_BRANCH%
    git -C thirdparty\manual-build\lib\rapidjson pull
)
robocopy "%CWD%\thirdparty\manual-build\lib\rapidjson\include" "%CWD%\thirdparty\manual-build\precompiled\rapidjson\include" /mir
cd %CWD%

echo %LINEBEG% Lua...
set LUA_NAME=lua
set LUA_VERSION=5.3.5.1
nuget install %LUA_NAME% -Version %LUA_VERSION% -OutputDirectory thirdparty\manual-build\lib
robocopy "thirdparty\manual-build\lib\%LUA_NAME%.%LUA_VERSION%\build\native\include" "%CWD%\thirdparty\manual-build\precompiled\lua\include" /mir
robocopy "%CWD%\thirdparty\manual-build\lib\%LUA_NAME%.%LUA_VERSION%\build\native\lib\v142\x64\Release" "%CWD%\thirdparty\manual-build\precompiled\lua\lib" lua.lib
robocopy "%CWD%\thirdparty\manual-build\lib\%LUA_NAME%.redist.%LUA_VERSION%\build\native\bin\v142\x64\Release" "%CWD%\thirdparty\manual-build\precompiled\lua\bin" lua.dll
cd %CWD%

goto comment
echo %LINEBEG% Protocol Buffers...
set PROTOBUF_BRANCH=v3.11.2
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
cmake -DCMAKE_BUILD_TYPE=Release -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_SHARED_LIBS=OFF -G Ninja ..
ninja
robocopy "%CD%" "%CWD%\thirdparty\manual-build\precompiled\protobuf\lib" libprotobuf.lib
cd ..\..
robocopy "%CD%\src" "%CWD%\thirdparty\manual-build\precompiled\protobuf\include" /mir
cd %CWD%

echo %LINEBEG% GameNetworkingSockets...
set GAMENETWORKINGSOCKETS_BRANCH=master
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
cmake -DCMAKE_BUILD_TYPE=Release -DProtobuf_USE_STATIC_LIBS=ON -DProtobuf_LIBRARY_RELEASE="%CWD%\thirdparty\manual-build\precompiled\protobuf\lib\libprotobuf.lib" -DProtobuf_INCLUDE_DIR="%CWD%\thirdparty\manual-build\precompiled\protobuf\include" -DOPENSSL_CRYPTO_LIBRARY="%CWD%\thirdparty\manual-build\precompiled\openssl\lib\libcrypto.lib" -DOPENSSL_INCLUDE_DIR="%CWD%\thirdparty\manual-build\precompiled\openssl\include" -DOPENSSL_ROOT_DIR="%CWD%\thirdparty\manual-build\precompiled\openssl" -G Ninja ..
ninja
:comment
cd %CWD%



set FETCH_FINISHED=y

:end

if "%FETCH_FINISHED%" == "y" (
    echo %CHECKMARK% Finished! Please check the above command line output in order to verify that everything was okay.
)