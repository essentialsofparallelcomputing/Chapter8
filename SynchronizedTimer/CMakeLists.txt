cmake_minimum_required(VERSION 2.8)
project(SynchronizedTimer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") # using Clang
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstrict-aliasing -march=native -mtune=native")

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") # using GCC
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstrict-aliasing -march=native -mtune=native")

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel") # using Intel C
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi-alias -xHOST")

elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")

elseif (CMAKE_CXX_COMPILER_ID MATCHES "XL")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -qalias=ansi -qhot -qarch=pwr9 -qtune=pwr9")

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Cray")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -h restrict=a")

endif()

# Require MPI for this project:
find_package(MPI REQUIRED)

add_executable(SynchronizedTimer1 SynchronizedTimer1.c)
set_target_properties(SynchronizedTimer1 PROPERTIES INCLUDE_DIRECTORIES "${MPI_C_INCLUDE_PATH}")
set_target_properties(SynchronizedTimer1 PROPERTIES COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS}")
set_target_properties(SynchronizedTimer1 PROPERTIES LINK_FLAGS "${MPI_C_LIBRARIES} ${MPI_C_LINK_FLAGS}")

add_executable(SynchronizedTimer2 SynchronizedTimer2.c)
set_target_properties(SynchronizedTimer2 PROPERTIES INCLUDE_DIRECTORIES "${MPI_C_INCLUDE_PATH}")
set_target_properties(SynchronizedTimer2 PROPERTIES COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS}")
set_target_properties(SynchronizedTimer2 PROPERTIES LINK_FLAGS "${MPI_C_LIBRARIES} ${MPI_C_LINK_FLAGS}")

# Add a test:
enable_testing()
add_test(SimpleTest ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} ${MPIEXEC_PREFLAGS}
         ${CMAKE_CURRENT_BINARY_DIR}/MinWorkExampleMPI ${MPIEXEC_POSTFLAGS})

# Cleanup
add_custom_target(distclean COMMAND rm -rf CMakeCache.txt CMakeFiles
                  Makefile cmake_install.cmake CTestTestfile.cmake Testing)
