cmake_minimum_required(VERSION 2.8)
project(SendRecv)

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

add_executable(SendRecv1 SendRecv1.c)
set_target_properties(SendRecv1 PROPERTIES INCLUDE_DIRECTORIES "${MPI_C_INCLUDE_PATH}")
set_target_properties(SendRecv1 PROPERTIES COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS}")
set_target_properties(SendRecv1 PROPERTIES LINK_FLAGS "${MPI_C_LIBRARIES} ${MPI_C_LINK_FLAGS}")

add_executable(SendRecv2 SendRecv2.c)
set_target_properties(SendRecv2 PROPERTIES INCLUDE_DIRECTORIES "${MPI_C_INCLUDE_PATH}")
set_target_properties(SendRecv2 PROPERTIES COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS}")
set_target_properties(SendRecv2 PROPERTIES LINK_FLAGS "${MPI_C_LIBRARIES} ${MPI_C_LINK_FLAGS}")

add_executable(SendRecv3 SendRecv3.c)
set_target_properties(SendRecv3 PROPERTIES INCLUDE_DIRECTORIES "${MPI_C_INCLUDE_PATH}")
set_target_properties(SendRecv3 PROPERTIES COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS}")
set_target_properties(SendRecv3 PROPERTIES LINK_FLAGS "${MPI_C_LIBRARIES} ${MPI_C_LINK_FLAGS}")

add_executable(SendRecv4 SendRecv4.c)
set_target_properties(SendRecv4 PROPERTIES INCLUDE_DIRECTORIES "${MPI_C_INCLUDE_PATH}")
set_target_properties(SendRecv4 PROPERTIES COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS}")
set_target_properties(SendRecv4 PROPERTIES LINK_FLAGS "${MPI_C_LIBRARIES} ${MPI_C_LINK_FLAGS}")

add_executable(SendRecv5 SendRecv5.c)
set_target_properties(SendRecv5 PROPERTIES INCLUDE_DIRECTORIES "${MPI_C_INCLUDE_PATH}")
set_target_properties(SendRecv5 PROPERTIES COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS}")
set_target_properties(SendRecv5 PROPERTIES LINK_FLAGS "${MPI_C_LIBRARIES} ${MPI_C_LINK_FLAGS}")

add_executable(SendRecv6 SendRecv6.c)
set_target_properties(SendRecv6 PROPERTIES INCLUDE_DIRECTORIES "${MPI_C_INCLUDE_PATH}")
set_target_properties(SendRecv6 PROPERTIES COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS}")
set_target_properties(SendRecv6 PROPERTIES LINK_FLAGS "${MPI_C_LIBRARIES} ${MPI_C_LINK_FLAGS}")

# Add a test:
enable_testing()
add_test(SimpleTest ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} ${MPIEXEC_PREFLAGS}
         ${CMAKE_CURRENT_BINARY_DIR}/SendRecv2 ${MPIEXEC_POSTFLAGS})

# Cleanup
add_custom_target(distclean COMMAND rm -rf CMakeCache.txt CMakeFiles
                  Makefile cmake_install.cmake CTestTestfile.cmake Testing)
