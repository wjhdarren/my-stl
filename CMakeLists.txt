cmake_minimum_required(VERSION 3.31)
project(my_stl)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(my)

add_executable(dbg_my_stl main.cpp)

target_link_libraries(dbg_my_stl lib_my_stl)



# enable_testing()

# # Add Google Test
# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   GIT_REPOSITORY https://github.com/google/googletest.git
#   GIT_TAG        v1.15.2
# )
# # For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)

# # Test executable
# add_executable(
#   vector_tests
#   test/vector/vector_test.cpp
# )

# # Link test executable with gtest
# target_link_libraries(
#   vector_tests
#   gtest_main
# )

# # Include directories for tests
# target_include_directories(vector_tests PRIVATE ${CMAKE_SOURCE_DIR})

# # Register tests with CTest
# include(GoogleTest)
# gtest_discover_tests(vector_tests)
