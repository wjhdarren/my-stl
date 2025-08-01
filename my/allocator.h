#pragma once
#include <cstddef>

template <class Pointer> struct allocation_result {
  Pointer ptr;
  std::size_t count;
};

namespace my {

template <class T> struct allocator {

  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;

  [[nodiscard]] constexpr pointer allocate(size_type n) {
    if (n == 0)
      return nullptr;
    return static_cast<pointer>(::operator new(n * sizeof(value_type)));
  }

  [[nodiscard]] constexpr allocation_result<pointer>
  allocate_at_least(size_t n);

  constexpr void deallocate(pointer p, size_t /* n */) {
    if (p == nullptr)
      return;
    ::operator delete(p);
  }
};
} // namespace my
