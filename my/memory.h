#pragma once
#include "type_traits.h"
#include <concepts>
#include <utility>

namespace my {

template <class T> struct default_delete {
  // Primary template specializations
  using pointer = T *;
  constexpr default_delete() noexcept = default;
  template <class U>
  constexpr default_delete(const default_delete<U> &d) noexcept {}
  constexpr void operator()(T *ptr) const {
    static_assert(0 < sizeof(T), "can't delete an incomplete type");
    delete ptr;
  }
};

template <class T> struct default_delete<T[]> {
  // Array specializations
  constexpr default_delete() noexcept = default;
  template <class U>
  constexpr default_delete(const default_delete<U[]> &d) noexcept {}
  template <class U> constexpr void operator()(U *ptr) const { delete[] ptr; }
};

// m_compressed_pair

struct m_zero_then_variadic_args_t {};
struct m_one_then_variadic_args_t {};

// EBO-enabled version: inherit from _T1 if it's empty and not final
template <typename _T1, typename _T2,
          bool = my::is_empty_v<_T1> && !my::is_final_v<_T1>>
class compressed_pair : private _T1 {
public:
  _T2 second;

  template <typename... Args2>
  constexpr explicit compressed_pair(
      m_zero_then_variadic_args_t,
      Args2 &&...args2) noexcept(std::is_nothrow_default_constructible_v<_T1> &&
                                 std::is_nothrow_constructible_v<_T2, Args2...>)
      : _T1(), second(std::forward<Args2>(args2)...) {}

  template <typename Arg1, typename... Args2>
  constexpr compressed_pair(
      m_one_then_variadic_args_t, Arg1 &&arg1,
      Args2 &&...args2) noexcept(std::is_nothrow_constructible_v<_T1, Arg1> &&
                                 std::is_nothrow_constructible_v<_T2, Args2...>)
      : _T1(std::forward<Arg1>(arg1)), second(std::forward<Args2>(args2)...) {}

  constexpr _T1 &first() noexcept { return *this; }
  constexpr const _T1 &first() const noexcept { return *this; }
};

// Non-EBO version: store both members directly
template <typename _T1, typename _T2> class compressed_pair<_T1, _T2, false> {
public:
  _T1 first;
  _T2 second;

  template <typename... Args2>
  constexpr explicit compressed_pair(
      m_zero_then_variadic_args_t,
      Args2 &&...args2) noexcept(std::is_nothrow_default_constructible_v<_T1> &&
                                 std::is_nothrow_constructible_v<_T2, Args2...>)
      : first(), second(std::forward<Args2>(args2)...) {}

  template <typename Arg1, typename... Args2>
  constexpr compressed_pair(
      m_one_then_variadic_args_t, Arg1 &&arg1,
      Args2 &&...args2) noexcept(std::is_nothrow_constructible_v<_T1, Arg1> &&
                                 std::is_nothrow_constructible_v<_T2, Args2...>)
      : first(std::forward<Arg1>(arg1)), second(std::forward<Args2>(args2)...) {
  }

  constexpr _T1 &first_elem() noexcept { return first; }
  constexpr const _T1 &first_elem() const noexcept { return first; }
};

template <typename D>
concept has_pointer_type = requires { typename D::pointer; };

} // namespace my
