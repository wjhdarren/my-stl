add_library(lib_my_stl INTERFACE)
target_include_directories(lib_my_stl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Add all header files to the interface library for better IDE support
target_sources(lib_my_stl INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/vector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/memory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/type_traits.h
    ${CMAKE_CURRENT_SOURCE_DIR}/allocator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/list.h
)
