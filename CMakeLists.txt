cmake_minimum_required(VERSION 3.17)
project(XBind)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_HOST_SYSTEM_NAME STREQUAL Darwin)
    list(APPEND CMAKE_PREFIX_PATH /usr/local/opt/llvm/lib/cmake/clang)
endif ()

find_package(Clang REQUIRED CONFIG)

include_directories(${CLANG_INCLUDE_DIRS} include)

add_definitions(${CLANG_DEFINITIONS} -fvisibility=hidden)

set(LLVM_LINK_COMPONENTS Support)

add_executable(xbind src/main.cpp src/XBindVisitor.cpp src/XBindDecl.cpp src/XBindType.cpp)

target_link_libraries(xbind PRIVATE clangAST clangBasic clangFrontend clangSerialization clangTooling)
