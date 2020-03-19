cmake_minimum_required(VERSION 2.8)
project (StreamTriad)

set (CMAKE_C_STANDARD 99)

# Set OpenMP flags for a few compilers
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O3")
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang") # using Clang
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstrict-aliasing -march=native -mtune=native")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Rpass-analysis=loop-vectorize")
   set(CMAKE_NOVEC_FLAGS "${CMAKE_NOVEC_FLAGS} -fnovectorize")
   set(CMAKE_VEC_FLAGS "${CMAKE_VEC_FLAGS} -fvectorize")

elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU") # using GCC
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstrict-aliasing -march=native -mtune=native")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopt-info-loop-optimized")
   set(CMAKE_NOVEC_FLAGS "${CMAKE_NOVEC_FLAGS} -fno-tree-vectorize")
   set(CMAKE_VEC_FLAGS "${CMAKE_VEC_FLAGS} -ftree-vectorize")
   if ("${CMAKE_C_COMPILER_VERSION}" VERSION_GREATER "7.4.0")
      set(CMAKE_VEC_FLAGS "${CMAKE_VEC_FLAGS} -mprefer-vector-width=512")
   endif ("${CMAKE_C_COMPILER_VERSION}" VERSION_GREATER "7.4.0")

elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "Intel") # using Intel C
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi-alias")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -qopt-report-phase=openmp,loop,vec")
   set(CMAKE_NOVEC_FLAGS "${CMAKE_NOVEC_FLAGS} -no-vec")
   set(CMAKE_VEC_FLAGS "${CMAKE_VEC_FLAGS} -xHOST")
   if ("${CMAKE_C_COMPILER_VERSION}" VERSION_GREATER "17.0.4")
     set(CMAKE_VEC_FLAGS "${CMAKE_VEC_FLAGS} -qopt-zmm-usage=high")
   endif ("${CMAKE_C_COMPILER_VERSION}" VERSION_GREATER "17.0.4")

elseif (CMAKE_C_COMPILER_ID MATCHES "MSVC")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Qvec-report:2")
   set(CMAKE_NOVEC_FLAGS "${CMAKE_NOVEC_FLAGS}")
   set(CMAKE_VEC_FLAGS "${CMAKE_VEC_FLAGS}")

elseif (CMAKE_C_COMPILER_ID MATCHES "XL")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -qalias=ansi -qhot -qarch=pwr9 -qtune=pwr9")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -qreport")
   set(CMAKE_NOVEC_FLAGS "${CMAKE_NOVEC_FLAGS} -qsimd=noauto")
   set(CMAKE_VEC_FLAGS "${CMAKE_VEC_FLAGS} -qsimd=auto")

elseif (CMAKE_C_COMPILER_ID MATCHES "Cray")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -h restrict=a")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -h msgs -h negmsgs -h list=a")
   set(CMAKE_NOVEC_FLAGS "${CMAKE_NOVEC_FLAGS} -h vector=0")
   set(CMAKE_VEC_FLAGS "${CMAKE_VEC_FLAGS} -h vector=3")

endif()

find_package(MPI REQUIRED)

# Adds build target of stream_triad with source code files
add_executable(StreamTriad StreamTriad.c timer.c timer.h)
set_target_properties(StreamTriad PROPERTIES INCLUDE_DIRECTORIES "${MPI_C_INCLUDE_PATH}")
set_target_properties(StreamTriad PROPERTIES COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS}")
set_target_properties(StreamTriad PROPERTIES COMPILE_FLAGS "${CMAKE_NOVEC_FLAGS}")
set_target_properties(StreamTriad PROPERTIES LINK_FLAGS "${MPI_C_LINK_FLAGS}")
target_link_libraries(StreamTriad PRIVATE "${MPI_C_LIBRARIES}")

add_executable(StreamTriadVec StreamTriadVec.c timer.c timer.h)
set_target_properties(StreamTriadVec PROPERTIES INCLUDE_DIRECTORIES "${MPI_C_INCLUDE_PATH}")
set_target_properties(StreamTriadVec PROPERTIES COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS}")
set_target_properties(StreamTriadVec PROPERTIES COMPILE_FLAGS "${CMAKE_VEC_FLAGS}")
set_target_properties(StreamTriadVec PROPERTIES LINK_FLAGS "${MPI_C_LINK_FLAGS}")
target_link_libraries(StreamTriadVec PRIVATE "${MPI_C_LIBRARIES}")

# Cleanup
add_custom_target(distclean COMMAND rm -rf CMakeCache.txt CMakeFiles
                  Makefile cmake_install.cmake StreamTriadNoVec.dSYM StreamTriadVectorized.dSYM ipo_out.optrpt)
