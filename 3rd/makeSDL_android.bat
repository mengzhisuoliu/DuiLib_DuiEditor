@echo off
setlocal enabledelayedexpansion

REM ============================================================
REM Script: makeSDL_android.bat
REM Usage:  makeSDL_android.bat [ABI]
REM         makeSDL_android.bat help
REM
REM Builds SDL3 for Android using the specified ABI.
REM If no ABI is given, prints available ABIs and exits.
REM ============================================================

set "SCRIPT_DIR=%~dp0"
cd /d "%SCRIPT_DIR%"

set "DEFAULT_ABI=arm64-v8a"

REM 定义有效的 ABI 列表（用于校验）
set "VALID_ABIS=arm64-v8a armeabi-v7a x86 x86_64"

REM 如果无参数，直接显示帮助并退出
if "%1"=="" goto :show_help

REM 处理 help 参数（不区分大小写）
if /i "%1"=="help" goto :show_help

REM 检查提供的 ABI 是否在有效列表中
set "ABI=%1"
set "VALID=0"
for %%a in (%VALID_ABIS%) do (
    if /i "%%a"=="%ABI%" set "VALID=1"
)
if %VALID%==0 (
    echo ERROR: Invalid ABI "%ABI%".
    echo.
    goto :show_help
)

echo Building for ABI: %ABI%
goto :build

:show_help
echo Usage: %~nx0 [ABI]
echo.
echo Available ABIs:
echo   arm64-v8a    - 64-bit ARM
echo   armeabi-v7a  - 32-bit ARM
echo   x86          - 32-bit x86
echo   x86_64       - 64-bit x86
echo.
echo Example: %~nx0 armeabi-v7a
exit /b 0

:build
REM 检查 ANDROID_NDK 环境变量
if "%ANDROID_NDK%"=="" (
    echo ERROR: Environment variable ANDROID_NDK is not set.
    echo Please set it to the path of your Android NDK.
    exit /b 1
)
echo Using Android NDK: %ANDROID_NDK%

REM 检查 SDL 目录是否存在
if not exist "SDL" (
    echo ERROR: SDL directory not found in current folder.
    exit /b 1
)
cd SDL

set "BUILD_DIR=build_android_%ABI%"

if exist "%BUILD_DIR%" (
    echo Build directory %BUILD_DIR% already exists. Removing...
    rmdir /s /q "%BUILD_DIR%"
)

mkdir "%BUILD_DIR%"
if errorlevel 1 (
    echo ERROR: Failed to create build directory.
    exit /b 1
)
cd "%BUILD_DIR%"

echo Configuring CMake for ABI %ABI% ...
cmake .. -G "Ninja" -DCMAKE_TOOLCHAIN_FILE="%ANDROID_NDK%/build/cmake/android.toolchain.cmake" -DANDROID_ABI=%ABI% -DANDROID_PLATFORM=android-21 -DBUILD_SHARED_LIBS=ON -DSDL_X11_XSCRNSAVER=OFF

if errorlevel 1 (
    echo ERROR: CMake configuration failed.
    exit /b 1
)

echo Building with Ninja (4 parallel jobs) ...
cmake --build . --parallel 4

if errorlevel 1 (
    echo ERROR: Build failed.
    exit /b 1
)

echo Build succeeded for %ABI%!

cd /d "%SCRIPT_DIR%"
echo Done.
exit /b 0