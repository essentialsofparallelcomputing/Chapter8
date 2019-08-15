cmake_minimum_required(VERSION 2.8)
project (stream_triad)

set (CMAKE_C_STANDARD 99)

# Set OpenMP flags for a few compilers
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O3")
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang") # using Clang
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstrict-aliasing -march=native -mtune=native")

elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU") # using GCC
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstrict-aliasing -march=native -mtune=native")

elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "Intel") # using Intel C
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi-alias")

elseif (CMAKE_C_COMPILER_ID MATCHES "MSVC")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

elseif (CMAKE_C_COMPILER_ID MATCHES "XL")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -qalias=ansi -qhot -qarch=pwr9 -qtune=pwr9")

elseif (CMAKE_C_COMPILER_ID MATCHES "Cray")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -h restrict=a")

endif()

find_package(MPI REQUIRED)

# Adds build target of stream_triad with source code files
add_executable(stream_triad stream_triad.c timer.c timer.h)
target_include_directories(stream_triad PRIVATE ${MPI_C_INCLUDE_PATH})
target_compile_options(stream_triad PRIVATE ${MPI_C_COMPILE_FLAGS})
target_link_libraries(stream_triad ${MPI_C_LIBRARIES} ${MPI_C_LINK_FLAGS})

# Cleanup
add_custom_target(distclean COMMAND rm -rf CMakeCache.txt CMakeFiles
                  Makefile cmake_install.cmake stream_triad.dSYM ipo_out.optrpt)
