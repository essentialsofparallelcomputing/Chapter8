cmake_minimum_required(VERSION 2.8)

project(GhostExchange)

# Require MPI for this project:
find_package(MPI REQUIRED)

add_executable(GhostExchange GhostExchange.cc timer.c timer.h malloc2D.c malloc2D.h)

target_include_directories(GhostExchange PRIVATE ${MPI_CXX_INCLUDE_PATH})
target_compile_options(GhostExchange PRIVATE ${MPI_CXX_COMPILE_FLAGS})
target_link_libraries(GhostExchange ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})

# Add a test:
enable_testing()
add_test(SimpleTest ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} ${MPIEXEC_PREFLAGS}
         ${CMAKE_CURRENT_BINARY_DIR}/GhostExchange ${MPIEXEC_POSTFLAGS})

# Cleanup
add_custom_target(distclean COMMAND rm -rf CMakeCache.txt CMakeFiles
                  Makefile cmake_install.cmake CTestTestfile.cmake Testing)
