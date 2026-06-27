@echo off
setlocal enabledelayedexpansion

REM ============================================================
REM Script: makeDuiLib_android.bat
REM Usage:  makeDuiLib_android.bat [ABI]
REM         makeDuiLib_android.bat help
REM
REM Builds DuiLib for Android using the specified ABI.
REM If no ABI is given, prints available ABIs and exits.
REM
REM Prerequisites:
REM   - SDL3 and SDL_ttf must already be built for the same ABI
REM     in ../3rd/SDL/build_android_%ABI% and
REM     ../3rd/SDL_ttf/build_android_%ABI% respectively.
REM ============================================================

set "SCRIPT_DIR=%~dp0"
cd /d "%SCRIPT_DIR%"

set "VALID_ABIS=arm64-v8a armeabi-v7a x86 x86_64"

REM 无参数 -> 显示帮助
if "%1"=="" goto :show_help

if /i "%1"=="help" goto :show_help

REM 校验 ABI
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

echo Building DuiLib for ABI: %ABI%
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

REM 检查依赖的 SDL3 和 SDL_ttf 构建目录是否存在（仅警告，不强制）
set "SDL3_DIR=..\3rd\SDL\build_android_%ABI%"
set "SDLTTF_DIR=..\3rd\SDL_ttf\build_android_%ABI%"

if not exist "%SDL3_DIR%" (
    echo WARNING: SDL3 build directory for %ABI% not found at %SDL3_DIR%.
    echo Please ensure SDL3 has been built for this ABI.
)
if not exist "%SDLTTF_DIR%" (
    echo WARNING: SDL_ttf build directory for %ABI% not found at %SDLTTF_DIR%.
    echo Please ensure SDL_ttf has been built for this ABI.
)

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
cmake .. -G "Ninja" -DCMAKE_TOOLCHAIN_FILE="%ANDROID_NDK%/build/cmake/android.toolchain.cmake" -DANDROID_ABI=%ABI% -DANDROID_PLATFORM=android-21 -DBUILD_SHARED_LIBS=OFF -DSDL3_DIR="%SDL3_DIR%" -DSDL3_ttf_DIR="%SDLTTF_DIR%"

if errorlevel 1 (
    echo ERROR: CMake configuration failed.
    cd /d "%SCRIPT_DIR%"
    exit /b 1
)

echo Building with Ninja (4 parallel jobs) ...
cmake --build . --parallel 4

if errorlevel 1 (
    echo ERROR: Build failed.
    cd /d "%SCRIPT_DIR%"
    exit /b 1
)

echo Build succeeded for %ABI%!

cd /d "%SCRIPT_DIR%"
echo Done.
exit /b 0