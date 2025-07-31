# Test directory CMakeLists.txt
# This provides a more organized structure for test building

# Find GTest (already found in parent, but good practice to declare here too)
find_package(GTest REQUIRED)

# Test executable
add_executable(vectortest
    vector_test.cpp
    test.cpp
)

target_link_libraries(vectortest
    lib_my_stl
    GTest::gtest
)

# Add tests to CTest
enable_testing()
add_test(NAME VectorTests COMMAND vectortest)
