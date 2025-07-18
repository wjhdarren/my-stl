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

struct m_zero_then_variadic_args_t {
  explicit m_zero_then_variadic_args_t() = default;
};
struct m_one_then_variadic_args_t {
  explicit m_one_then_variadic_args_t() = default;
};

// EBO-enabled version: inherit from _T1 if it's empty and not final
template <typename _T1, typename _T2,
          bool = my::is_empty_v<_T1> && !my::is_final_v<_T1>>
class m_compressed_pair : private _T1 {
public:
  _T2 second;

  template <typename... Args2>
  constexpr explicit m_compressed_pair(
      m_zero_then_variadic_args_t,
      Args2 &&...args2) noexcept(std::is_nothrow_default_constructible_v<_T1> &&
                                 std::is_nothrow_constructible_v<_T2, Args2...>)
      : _T1(), second(std::forward<Args2>(args2)...) {}

  template <typename Arg1, typename... Args2>
  constexpr m_compressed_pair(
      m_one_then_variadic_args_t, Arg1 &&arg1,
      Args2 &&...args2) noexcept(std::is_nothrow_constructible_v<_T1, Arg1> &&
                                 std::is_nothrow_constructible_v<_T2, Args2...>)
      : _T1(std::forward<Arg1>(arg1)), second(std::forward<Args2>(args2)...) {}

  constexpr _T1 &get_first() noexcept { return *this; }
  constexpr const _T1 &get_first() const noexcept { return *this; }
  constexpr _T2 &get_second() noexcept { return second; }
  constexpr const _T2 &get_second() const noexcept { return second; }
};

// Non-EBO version: store both members directly
template <typename _T1, typename _T2> class m_compressed_pair<_T1, _T2, false> {
public:
  _T1 first;
  _T2 second;

  template <typename... Args2>
  constexpr explicit m_compressed_pair(
      m_zero_then_variadic_args_t,
      Args2 &&...args2) noexcept(std::is_nothrow_default_constructible_v<_T1> &&
                                 std::is_nothrow_constructible_v<_T2, Args2...>)
      : first(), second(std::forward<Args2>(args2)...) {}

  template <typename Arg1, typename... Args2>
  constexpr m_compressed_pair(
      m_one_then_variadic_args_t, Arg1 &&arg1,
      Args2 &&...args2) noexcept(std::is_nothrow_constructible_v<_T1, Arg1> &&
                                 std::is_nothrow_constructible_v<_T2, Args2...>)
      : first(std::forward<Arg1>(arg1)), second(std::forward<Args2>(args2)...) {
  }

  constexpr _T1 &get_first() noexcept { return first; }
  constexpr const _T1 &get_first() const noexcept { return first; }
  constexpr _T2 &get_second() noexcept { return second; }
  constexpr const _T2 &get_second() const noexcept { return second; }
};

template <typename D>
concept has_pointer_type = requires { typename D::pointer; };

template <class T, class Deleter = default_delete<T>> class unique_ptr {
public:
  using pointer =
      my::conditional_t<has_pointer_type<my::remove_reference_t<Deleter>>,
                        typename my::remove_reference_t<Deleter>::pointer, T *>;
  using element_type = T;
  using deleter_type = Deleter;

private:
  m_compressed_pair<deleter_type, pointer> m_pair;

public:
  // ctor
  unique_ptr(const unique_ptr &) = delete;

  // member functions

  unique_ptr &operator=(const unique_ptr &) = delete;

  // modifiers
  constexpr pointer release() noexcept {
    return std::exchange(m_pair.get_second(), nullptr);
  }
  constexpr pointer reset(pointer ptr = nullptr) noexcept {
    auto old_ptr = std::exchange(m_pair.get_second(), ptr);
    if (old_ptr) {
      m_pair.get_first()(old_ptr);
    }
  }
  void swap(unique_ptr &other) noexcept {
    if (*this != other) {
      std::swap(m_pair.get_first(), other.m_pair.get_first());
      std::swap(m_pair.get_second(), other.m_pair.get_second());
    }
  }
  // observers
  constexpr pointer get() const noexcept { return m_pair.get_second(); }
  constexpr deleter_type &get_deleter() noexcept { return m_pair.get_first(); }
  constexpr const deleter_type &get_deleter() const noexcept {
    return m_pair.get_first();
  }
  constexpr explicit operator bool() const noexcept {
    return (static_cast<bool>(m_pair.get_second()));
  }

  // pointer operator
  [[nodiscard]] constexpr typename my::add_lvalue_reference_t<element_type>
  operator*() const noexcept(noexcept(*std::declval<pointer>())) {
    return *m_pair.get_second();
  }
  [[nodiscard]] constexpr pointer operator->() const noexcept {
    return m_pair.get_second();
  }

  // dtor
  ~unique_ptr() {
    if (m_pair.get_second()) {
      m_pair.get_first()(m_pair.get_second());
    }
  }
};

template <class T, class Deleter> class unique_ptr<T[], Deleter>;
} // namespace my
