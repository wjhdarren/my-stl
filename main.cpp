#include "my/unordered_map.h"
#include "my/vector.h"
#include <algorithm>
#include <functional>
#include <ranges>
#include <string>
#include <vector>

using namespace std::string_literals;
namespace stdv = std::ranges::views;
namespace stdr = std::ranges;
using Map = my::unordered_map<std::string, std::string>;

void test_vector() {
  // create from view
  auto vec = my::vector<int>(std::from_range, stdv::iota(0, 10));
  vec.show();

  // create from view of string
  auto str_v = stdv::transform(stdv::iota(0, 10),
                                     [](int i) { return std::to_string(i); });
  auto vec2 = my::vector<std::string>(std::from_range, str_v);
  vec2.show();

  // create from view of unique_ptr (don't have copy ctor)
  auto unique_ptr_v = stdv::transform(
      stdv::iota(0, 10), [](int i) { return std::make_unique<int>(i); });

  auto vec3 = my::vector<std::unique_ptr<int>>(std::from_range, unique_ptr_v);
  vec3.show();

  // use with pipe operator & template deduction
  auto vec4 = vec | stdv::filter([](int i) { return i <= 5;}) | stdr::to<my::vector>();
  vec4.show();

}

void test_unique_ptr() {

}

int main() {
    test_vector();
    test_unqiue_ptr();
}
