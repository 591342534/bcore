﻿# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("bcore-project")

#指定c++17
set(CMAKE_CXX_STANDARD 17)

IF (CMAKE_BUILD_TYPE MATCHES "Debug")
    SET(LIB_DIR "lib/x64-debug")
    add_definitions(-D_DEBUG)
ELSEIF(CMAKE_BUILD_TYPE MATCHES "Release")
    SET(LIB_DIR "lib/x64-release")
ELSE()
    SET(CMAKE_BUILD_TYPE "")
ENDIF()

link_directories(
    ${LIB_DIR}
)
SET(LIB_SUFFIX ".lib")
SET(G_TEST_LIB ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_DIR}/gtest${LIB_SUFFIX})
SET(BCORE_LIB ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_DIR}/bcore${LIB_SUFFIX})
SET(BCORE_ASIO_LIB ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_DIR}/bcore_asio${LIB_SUFFIX})

SET(ROOT_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)
message(G_TEST_LIB\ ${G_TEST_LIB})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    F:/github_code/googletest-release-1.10.0/googletest/include
    F:/github_code/asio-1.16.1/include
)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_DIR})
add_definitions(-DBTEST)

# 包含子项目。
add_subdirectory ("test_project")
add_subdirectory ("network")
add_subdirectory ("src/bcore")
add_subdirectory ("test")
add_subdirectory ("src/basio")

