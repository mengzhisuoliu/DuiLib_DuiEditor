cmake_minimum_required(VERSION 3.10)
project("Main")

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

#message(FATAL_ERROR "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
#file(REAL_PATH "${CMAKE_SOURCE_DIR}/../../../../../../3rd/SDL/include" REAL_INC_PATH)
#message(STATUS "Resolved include path: ${REAL_INC_PATH}")

# 添加预处理器宏：使用 SDL 后端
add_definitions(-DDUILIB_SDL)

# 头文件路径（根据实际存放位置调整）
include_directories(
    # DuiLib头文件目录
    ${CMAKE_SOURCE_DIR}/../../../../../../DuiLib
    # SDL3 头文件
    ${CMAKE_SOURCE_DIR}/../../../../../../3rd/SDL/include
    ${CMAKE_SOURCE_DIR}/../../../../../../3rd/SDL_ttf/include
)

# 导入预编译的库
add_library(DuiLib STATIC IMPORTED)
set_target_properties(DuiLib PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libDuiLib.so)

add_library(SDL3 STATIC IMPORTED)
set_target_properties(SDL3 PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libSDL3.so)

add_library(SDL3_ttf STATIC IMPORTED)
set_target_properties(SDL3_ttf PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libSDL3_ttf.so)

# 构建你自己的 native 库（共享库）
add_library(Main SHARED
    main.cpp
    App.cpp
    MainFrame.cpp
)

# 链接库
target_link_libraries(Main
    DuiLib
    SDL3
    SDL3_ttf
    android
    log
    GLESv1_CM
    GLESv2
    OpenSLES
)