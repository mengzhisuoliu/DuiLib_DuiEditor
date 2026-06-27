cmake_minimum_required(VERSION 3.10)
project(DuiLib VERSION 1.0.0 LANGUAGES CXX)

# 需要 C++11 或更高
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 优先查找静态库（在 find_library 之前设置）
set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})

# 定义库类型（静态库，可改为 SHARED）
option(BUILD_SHARED_LIBS "Build DuiLib as shared library" OFF)

# 添加预处理器宏：使用 SDL 后端
add_definitions(-DDUILIB_SDL)

# 取消平台相关的编译警告（可选）
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wno-unknown-pragmas)
endif()

# 收集源文件
file(GLOB_RECURSE CORE_SOURCES "Core/*.cpp")
file(GLOB_RECURSE CONTROL_SOURCES "Control/*.cpp")
file(GLOB_RECURSE UTILS_SOURCES "Utils/*.cpp")
file(GLOB_RECURSE LAYOUT_SOURCES "Layout/*.cpp")
file(GLOB_RECURSE RENDER_SOURCES "Render/*.cpp")

# 顶层源文件
set(TOP_SOURCES
    StdAfx.cpp
    UIlib.cpp
)

# 合并所有源文件
set(ALL_SOURCES
    ${TOP_SOURCES}
    ${CORE_SOURCES}
    ${CONTROL_SOURCES}
    ${UTILS_SOURCES}
    ${LAYOUT_SOURCES}
    ${RENDER_SOURCES}
)

# 剔除特定的源文件
list(REMOVE_ITEM ALL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/unzip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/UIDataExchange.cpp
)

# 包含目录（项目内部头文件）
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Control
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/Layout
    ${CMAKE_CURRENT_SOURCE_DIR}/Render
    ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/SDL/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/SDL_image/include
#    ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/SDL_ttf/include
#    ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/SDL_gfx
)


# 查找系统库

# 1. 尝试使用 Config 模式（从 SDL3Config.cmake 获取信息）
find_package(SDL3 CONFIG QUIET)
find_package(SDL3_ttf CONFIG QUIET)

# 2. 如果 Config 模式失败，回退到传统 find_library + find_path
if(NOT SDL3_FOUND)
    # 允许用户通过 -DSDL3_ROOT 指定 SDL3 根目录（如 /usr/local 或构建目录）
    if(NOT SDL3_ROOT)
        set(SDL3_ROOT "/usr/local" CACHE PATH "SDL3 installation root")
    endif()
    # 尝试在指定路径查找库（优先静态库）
    find_library(SDL3_LIB NAMES SDL3 libSDL3
        PATHS ${SDL3_ROOT}/lib ${SDL3_ROOT}/build_android_arm64
        NO_DEFAULT_PATH
    )
    find_path(SDL3_INCLUDE_DIR NAMES SDL3/SDL.h
        PATHS ${SDL3_ROOT}/include ${SDL3_ROOT}/include/SDL3
        NO_DEFAULT_PATH
    )
    # 如果上述没找到，再试系统默认路径（非交叉编译时）
    if(NOT SDL3_LIB OR NOT SDL3_INCLUDE_DIR)
        find_library(SDL3_LIB NAMES SDL3 libSDL3)
        find_path(SDL3_INCLUDE_DIR NAMES SDL3/SDL.h)
    endif()
    if(NOT SDL3_LIB OR NOT SDL3_INCLUDE_DIR)
        message(FATAL_ERROR "SDL3 not found. Please set SDL3_DIR (Config) or SDL3_ROOT (fallback).")
    endif()
endif()

if(NOT SDL3_ttf_FOUND)
    if(NOT SDL3_TTF_ROOT)
        set(SDL3_TTF_ROOT "/usr/local" CACHE PATH "SDL3_ttf installation root")
    endif()
    find_library(SDL3_ttf_LIB NAMES SDL3_ttf libSDL3_ttf
        PATHS ${SDL3_TTF_ROOT}/lib ${SDL3_TTF_ROOT}/build_android_arm64
        NO_DEFAULT_PATH
    )
    find_path(SDL3_ttf_INCLUDE_DIR NAMES SDL3_ttf/SDL_ttf.h
        PATHS ${SDL3_TTF_ROOT}/include ${SDL3_TTF_ROOT}/include/SDL3_ttf
        NO_DEFAULT_PATH
    )
    if(NOT SDL3_ttf_LIB OR NOT SDL3_ttf_INCLUDE_DIR)
        find_library(SDL3_ttf_LIB NAMES SDL3_ttf libSDL3_ttf)
        find_path(SDL3_ttf_INCLUDE_DIR NAMES SDL3_ttf/SDL_ttf.h)
    endif()
    if(NOT SDL3_ttf_LIB OR NOT SDL3_ttf_INCLUDE_DIR)
        message(FATAL_ERROR "SDL3_ttf not found. Please set SDL3_ttf_DIR (Config) or SDL3_TTF_ROOT (fallback).")
    endif()
endif()

# 可选：处理 macOS 下的 framework 或额外库（如 CoreFoundation）
if(APPLE)
    find_library(COREFOUNDATION_LIB CoreFoundation)
    set(EXTRA_LIBS ${COREFOUNDATION_LIB})
endif()

# 设置库输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Lib)

# 生成库
add_library(DuiLib ${ALL_SOURCES})

# 链接 SDL 相关库
if(SDL3_FOUND AND SDL3_ttf_FOUND)
    # 使用导入目标（自动处理 include 路径和依赖）
    target_link_libraries(DuiLib
        SDL3::SDL3
        SDL3_ttf::SDL3_ttf
        ${EXTRA_LIBS}
    )
else()
    # 回退：直接使用找到的库和头文件路径
    target_link_libraries(DuiLib
        ${SDL3_LIB}
        ${SDL3_ttf_LIB}
        ${EXTRA_LIBS}
    )
    # 手动添加头文件路径（仅在回退模式下需要）
    target_include_directories(DuiLib PRIVATE
        ${SDL3_INCLUDE_DIR}
        ${SDL3_ttf_INCLUDE_DIR}
    )
endif()

# 平台特定链接库（静态链接时也需要）
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(DuiLib pthread dl)
elseif(APPLE)
    target_link_libraries(DuiLib
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework CoreFoundation"
        "-framework CoreText"
        "-framework CoreGraphics"
    )
endif()

# 导出头文件搜索路径（供外部项目使用）
target_include_directories(DuiLib INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Control
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/Layout
    ${CMAKE_CURRENT_SOURCE_DIR}/Render
)

# 可选：安装规则
install(TARGETS DuiLib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY Core Control Utils Layout Render DESTINATION include/DuiLib FILES_MATCHING PATTERN "*.h")