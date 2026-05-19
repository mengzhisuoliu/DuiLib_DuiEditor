cmake_minimum_required(VERSION 3.10)
project(DuiLib VERSION 1.0.0 LANGUAGES CXX)

# 需要 C++11 或更高
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/SDL_ttf/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/SDL_gfx
)

# 查找系统库（假设库文件位于 /usr/local/lib）
find_library(SDL3_LIB NAMES SDL3 libSDL3 PATHS /usr/local/lib /usr/lib)
find_library(SDL3_image_LIB NAMES SDL3_image SDL_image PATHS /usr/local/lib /usr/lib)
find_library(SDL3_ttf_LIB NAMES SDL3_ttf SDL_ttf PATHS /usr/local/lib /usr/lib)
find_library(SDL3_gfx_LIB NAMES SDL3_gfx SDL_gfx PATHS /usr/local/lib /usr/lib)

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
target_link_libraries(DuiLib
    ${SDL3_LIB}
    ${SDL3_image_LIB}
    ${SDL3_ttf_LIB}
    ${SDL3_gfx_LIB}
    ${EXTRA_LIBS}
)

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
