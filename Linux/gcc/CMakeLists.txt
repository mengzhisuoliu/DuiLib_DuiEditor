cmake_minimum_required(VERSION 3.10)
project(TestDuiLib)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 优先查找静态库
set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})

# 定义输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)

# 添加预处理器宏（DuiLib 使用 SDL 后端）
add_definitions(-DDUILIB_SDL)

# 包含 DuiLib 头文件
set(DUILIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../DuiLib)
include_directories(
    ${DUILIB_ROOT}
)

# 包含 SDL 头文件
set(SDL3_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd/SDL)
set(SDL3_GFX_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd/SDL_gfx)
set(SDL3_IMAGE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd/SDL_image)
set(SDL3_TTF_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd/SDL_ttf)

include_directories(
    ${SDL3_ROOT}/include
    ${SDL3_GFX_ROOT}/include
    ${SDL3_IMAGE_ROOT}/include
    ${SDL3_TTF_ROOT}/include
)

# 查找 SDL 库（系统路径 /usr/local/lib）
find_library(SDL3_LIB NAMES SDL3 libSDL3 PATHS /usr/local/lib /usr/lib)
find_library(SDL3_IMAGE_LIB NAMES SDL3_image SDL_image PATHS /usr/local/lib /usr/lib)
find_library(SDL3_TTF_LIB NAMES SDL3_ttf SDL_ttf PATHS /usr/local/lib /usr/lib)
find_library(SDL3_GFX_LIB NAMES SDL3_gfx SDL_gfx PATHS /usr/local/lib /usr/lib)

# DuiLib 静态库（假设已编译到 Lib 目录）
set(DUILIB_LIB ${DUILIB_ROOT}/Lib/libDuiLib.a)

# 测试程序的源文件
set(SOURCES
    main.cpp
    App.cpp
    MainFrame.cpp
)

# 生成可执行文件
add_executable(TestDuiLib ${SOURCES})

# 链接库
target_link_libraries(TestDuiLib
    ${DUILIB_LIB}
    ${SDL3_LIB}
    ${SDL3_IMAGE_LIB}
    ${SDL3_TTF_LIB}
    ${SDL3_GFX_LIB}
)

# 平台特定链接库
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(TestDuiLib pthread dl)
    # 添加额外的链接标志
    target_link_options(TestDuiLib PRIVATE
        -Wl,--hash-style=sysv
        -Wl,-Bsymbolic
        -Wl,-rpath=./
        -no-pie
    )
elseif(APPLE)
    target_link_libraries(TestDuiLib
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework CoreFoundation"
    )
endif()