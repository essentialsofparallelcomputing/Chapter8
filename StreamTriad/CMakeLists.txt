cmake_minimum_required(VERSION 2.8)
project (StreamTriad)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set (CMAKE_C_STANDARD 99)

# Set OpenMP flags for a few compilers
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O3")

if (NOT CMAKE_VECTOR_VERBOSE)
    set(CMAKE_VECTOR_VERBOSE true)
endif (NOT CMAKE_VECTOR_VERBOSE)

find_package(Vector REQUIRED)
if (CMAKE_VECTOR_VERBOSE)
    set(VECTOR_C_FLAGS "${VECTOR_C_FLAGS} ${VECTOR_C_VERBOSE}")
    set(VECTOR_NOVEC_C_FLAGS "${VECTOR_NOVEC_C_FLAGS} ${VECTOR_C_VERBOSE}")
endif (CMAKE_VECTOR_VERBOSE)

find_package(MPI REQUIRED)

# Adds build target of stream_triad with source code files
add_executable(StreamTriad StreamTriad.c timer.c timer.h)
set_target_properties(StreamTriad PROPERTIES INCLUDE_DIRECTORIES "${MPI_C_INCLUDE_PATH}")
set_target_properties(StreamTriad PROPERTIES COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS}")
set_target_properties(StreamTriad PROPERTIES COMPILE_FLAGS "${VECTOR_NOVEC_C_FLAGS}")
set_target_properties(StreamTriad PROPERTIES LINK_FLAGS "${MPI_C_LINK_FLAGS}")
target_link_libraries(StreamTriad PRIVATE "${MPI_C_LIBRARIES}")

add_executable(StreamTriadVec StreamTriadVec.c timer.c timer.h)
set_target_properties(StreamTriadVec PROPERTIES INCLUDE_DIRECTORIES "${MPI_C_INCLUDE_PATH}")
set_target_properties(StreamTriadVec PROPERTIES COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS}")
set_target_properties(StreamTriadVec PROPERTIES COMPILE_FLAGS "${VECTOR_C_FLAGS}")
set_target_properties(StreamTriadVec PROPERTIES LINK_FLAGS "${MPI_C_LINK_FLAGS}")
target_link_libraries(StreamTriadVec PRIVATE "${MPI_C_LIBRARIES}")

# Cleanup
add_custom_target(distclean COMMAND rm -rf CMakeCache.txt CMakeFiles
                  Makefile cmake_install.cmake StreamTriadNoVec.dSYM StreamTriadVectorized.dSYM ipo_out.optrpt)
