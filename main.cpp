#include "my/vector.h"
#include <print>
#include <string>
#include <vector>

using Vec = my::vector<std::string>;

int main() {

  auto vec = Vec{"1", "2", "3"};
  vec.reserve(100);
  vec.show();
  vec.insert(vec.begin() + 1, {"1", "2", "3"});
  vec.show();
}
