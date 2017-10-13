cmake_minimum_required(VERSION 3.0)
project(glee-project)

# flags
set(CMAKE_C_FLAGS "-std=c++11 -Wall -pthread")
add_definitions(-D_FILE_OFFSET_BITS=64)

# setup build directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# dependency libraries
set(Boost_USE_STATIC_LIBS        OFF)
set(Boost_USE_MULTITHREADED      ON)
find_package(Boost 1.65.1 COMPONENTS
        system filesystem thread program_options REQUIRED
) 
if(Boost_FOUND)
        include_directories(${Boost_INCLUDE_DIRS})
endif()
set(SQLITE_LIBRARIES "sqlite3")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})

# setup header files
include_directories(include)

# setup sub directory
add_subdirectory(src)
