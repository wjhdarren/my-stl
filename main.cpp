#include <iostream>
#include <string>
#include <vector>
#include "my/vector.h"
// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
  using Vec = my::vector<std::string>;
  auto v = Vec(5, "my");
  v.show();

  auto v2 = Vec(v);
  v2.show();

  auto v3 = Vec{"1", "2", "3"};
  v3.show();
  std::vector vec  = {1, 2, 3, 4};

  v3 = {"4", "5", "6"};
  v3.show();
  // TIP See CLion help at <a href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>. Also, you can try interactive lessons for CLion by selecting 'Help | Learn IDE Features' from the main menu.
}