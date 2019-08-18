cmake_minimum_required(VERSION 2.8)
project(CartExchange)

# Require MPI for this project:
find_package(MPI REQUIRED)

add_executable(CartExchange CartExchange.cc timer.c timer.h malloc2D.c malloc2D.h)

target_include_directories(CartExchange PRIVATE ${MPI_CXX_INCLUDE_PATH})
target_compile_options(CartExchange PRIVATE ${MPI_CXX_COMPILE_FLAGS})
target_link_libraries(CartExchange ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})

# Add a test:
enable_testing()
add_test(SimpleTest ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} ${MPIEXEC_PREFLAGS}
         ${CMAKE_CURRENT_BINARY_DIR}/CartExchange ${MPIEXEC_POSTFLAGS})

# Cleanup
add_custom_target(distclean COMMAND rm -rf CMakeCache.txt CMakeFiles
                  Makefile cmake_install.cmake CTestTestfile.cmake Testing)
