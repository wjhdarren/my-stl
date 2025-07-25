project(
  'my-stl',
  'cpp',
  version : '0.1',
  meson_version : '>= 1.3.0',
  default_options : ['warning_level=3', 'cpp_std=c++23'],
)

my_stl_lib = shared_library(
  'mystl',
  ['my/vector.h']
)

test_dep = dependency('gtest')

# exe = executable(
#   'main',
#   'main.cpp',
#   install : true,
#   dependencies : [],
# )

vector_test = executable(
  'vectortest',
  sources : ['test/vector_test.cpp', 'test/test.cpp'],
  dependencies : test_dep,
  link_with : my_stl_lib
)

