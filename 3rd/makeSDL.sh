#!/bin/bash

# 任何命令失败则退出脚本
set -e

BUILD_SDL=0
BUILD_TTF=0
BUILD_IMAGE=0
BUILD_GFX=0

# ------------------------- 解析命令行参数 -------------------------
if [[ $# -eq 0 ]]; then
    # 无参数：默认行为
    BUILD_SDL=1
    BUILD_TTF=1
else
    for arg in "$@"; do
        case "$arg" in
            --sdl)   BUILD_SDL=1 ;;
            --ttf)   BUILD_TTF=1 ;;
            --image) BUILD_IMAGE=1 ;;
            --gfx)   BUILD_GFX=1 ;;
            --all)
                BUILD_SDL=1
                BUILD_TTF=1
                BUILD_IMAGE=1
                BUILD_GFX=1
                ;;
            *)
                echo "未知参数: $arg"
                echo "用法: $0 [--sdl] [--ttf] [--all]"
                echo "默认（无参数）编译 SDL3 和 SDL_ttf"
                exit 1
                ;;
        esac
    done
fi

# ------------------------- 检测操作系统 -------------------------
OS="$(uname -s)"
if [ "$OS" = "Darwin" ]; then
    IS_MACOS=1
    # 可自定义最低支持版本和架构，如兼容 Intel 和 Apple Silicon
    MACOS_DEPLOY_TARGET="11.0"
    MACOS_ARCHS="x86_64;arm64"
    echo "✅ 检测到 macOS，将设置部署目标 ${MACOS_DEPLOY_TARGET}，架构 ${MACOS_ARCHS}"
else
    IS_MACOS=0
fi

# ------------------------- 获取 CPU 核心数（跨平台） -------------------------
if command -v nproc >/dev/null 2>&1; then
    NPROC=$(nproc)
elif command -v sysctl >/dev/null 2>&1 && sysctl -n hw.ncpu >/dev/null 2>&1; then
    NPROC=$(sysctl -n hw.ncpu)
else
    NPROC=$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 1)
fi
echo "使用 $NPROC 个并行任务进行编译"

# ------------------------- 通用 CMake 参数 -------------------------
CMAKE_FLAGS=()
if [ $IS_MACOS -eq 1 ]; then
    CMAKE_FLAGS+=("-DCMAKE_OSX_DEPLOYMENT_TARGET=${MACOS_DEPLOY_TARGET}")
    #CMAKE_FLAGS+=("-DCMAKE_OSX_ARCHITECTURES=${MACOS_ARCHS}")
fi

if [ $BUILD_SDL -eq 1 ]; then
	echo "========== 编译 SDL3 =========="
	cd SDL
	rm -rf build/
	cmake -S . -B build -DSDL_X11_XSCRNSAVER=OFF "${CMAKE_FLAGS[@]}"
	cmake --build build --parallel $NPROC
	sudo cmake --install build
	cd ..
fi

if [ $BUILD_TTF -eq 1 ]; then
	echo "========== 编译 SDL_ttf =========="
	cd SDL_ttf
	rm -rf build/
	TTF_FLAGS=("${CMAKE_FLAGS[@]}")
	TTF_FLAGS+=("-DSDLTTF_VENDORED=ON")
	cmake -S . -B build "${TTF_FLAGS[@]}"
	cmake --build build --parallel $NPROC
	sudo cmake --install build
	cd ..
fi


if [ $BUILD_IMAGE -eq 1 ]; then
	#echo "========== 编译 SDL_image =========="
	#cd SDL_image
	#rm -rf build/
	#cmake -S . -B build
	#cmake --build build --parallel $NPROC
	#sudo cmake --install build
	#cd ..
fi


if [ $BUILD_GFX -eq 1 ]; then
	#echo "========== 编译 SDL_gfx =========="
	#cd SDL_gfx
	#rm -rf build/
	#cmake -S . -B build
	#cmake --build build --parallel $NPROC
	#sudo cmake --install build
	#cd ..
fi

echo "========== 所有 SDL3 库编译并安装完成 =========="