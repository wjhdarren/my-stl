cmake_minimum_required(VERSION 3.31)
project(my_stl)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_CLANG_TIDY clang-tidy)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(my)

# Main debug executable (equivalent to commented out main in meson)
add_executable(dbg_my_stl main.cpp)
target_link_libraries(dbg_my_stl lib_my_stl)

# Find GTest dependency
find_package(GTest REQUIRED)

# Test executable (equivalent to vectortest in meson)
add_executable(vectortest 
    test/vector_test.cpp 
    test/test.cpp
)

target_link_libraries(vectortest 
    lib_my_stl 
    GTest::gtest
)

# Enable testing
enable_testing()
add_test(NAME VectorTests COMMAND vectortest)



