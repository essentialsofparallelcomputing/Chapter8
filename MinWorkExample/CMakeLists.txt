cmake_minimum_required(VERSION 2.8)

project(MinWorkExampleMPI)

# Require MPI for this project:
find_package(MPI REQUIRED)

add_executable(MinWorkExampleMPI MinWorkExampleMPI.c)

target_include_directories(MinWorkExampleMPI PRIVATE ${MPI_C_INCLUDE_PATH})
target_compile_options(MinWorkExampleMPI PRIVATE ${MPI_C_COMPILE_FLAGS})
target_link_libraries(MinWorkExampleMPI ${MPI_C_LIBRARIES} ${MPI_C_LINK_FLAGS})

# Add a test:
enable_testing()
add_test(SimpleTest ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} ${MPIEXEC_PREFLAGS}
         ${CMAKE_CURRENT_BINARY_DIR}/MinWorkExampleMPI ${MPIEXEC_POSTFLAGS})

# Cleanup
add_custom_target(distclean COMMAND rm -rf CMakeCache.txt CMakeFiles
                  Makefile cmake_install.cmake CTestTestfile.cmake Testing)
