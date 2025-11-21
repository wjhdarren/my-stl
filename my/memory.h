#pragma once
#include <concepts>
#include <type_traits>
#include <print>
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

// EBO-enabled version: inherit from T1 if it's empty and not final
template <typename T1, typename T2,
          bool = std::is_empty_v<T1> && !std::is_final_v<T1>>
class m_compressed_pair : private T1 {
public:
  T2 second;

  template <typename... Args2>
  constexpr explicit m_compressed_pair(
      m_zero_then_variadic_args_t,
      Args2 &&...args2) noexcept(std::is_nothrow_default_constructible_v<T1> &&
                                 std::is_nothrow_constructible_v<T2, Args2...>)
      : T1(), second(std::forward<Args2>(args2)...) {}

  template <typename Arg1, typename... Args2>
  constexpr m_compressed_pair(
      m_one_then_variadic_args_t, Arg1 &&arg1,
      Args2 &&...args2) noexcept(std::is_nothrow_constructible_v<T1, Arg1> &&
                                 std::is_nothrow_constructible_v<T2, Args2...>)
      : T1(std::forward<Arg1>(arg1)), second(std::forward<Args2>(args2)...) {}

  constexpr T1 &get_first() noexcept { return *this; }
  constexpr const T1 &get_first() const noexcept { return *this; }
  constexpr T2 &get_second() noexcept { return second; }
  constexpr const T2 &get_second() const noexcept { return second; }
};

// Non-EBO version: store both members directly
template <typename T1, typename T2> class m_compressed_pair<T1, T2, false> {
public:
  T1 first;
  T2 second;

  template <typename... Args2>
  constexpr explicit m_compressed_pair(
      m_zero_then_variadic_args_t,
      Args2 &&...args2) noexcept(std::is_nothrow_default_constructible_v<T1> &&
                                 std::is_nothrow_constructible_v<T2, Args2...>)
      : first(), second(std::forward<Args2>(args2)...) {}

  template <typename Arg1, typename... Args2>
  constexpr m_compressed_pair(
      m_one_then_variadic_args_t, Arg1 &&arg1,
      Args2 &&...args2) noexcept(std::is_nothrow_constructible_v<T1, Arg1> &&
                                 std::is_nothrow_constructible_v<T2, Args2...>)
      : first(std::forward<Arg1>(arg1)), second(std::forward<Args2>(args2)...) {
  }

  constexpr T1 &get_first() noexcept { return first; }
  constexpr const T1 &get_first() const noexcept { return first; }
  constexpr T2 &get_second() noexcept { return second; }
  constexpr const T2 &get_second() const noexcept { return second; }
};

// concepts to be used in unique_ptr
template <typename D>
concept has_pointer_type = requires { typename D::pointer; };

template <typename D>
concept unique_ptr_enable_default_t =
    !std::is_pointer_v<D> && std::default_initializable<D>;

// unique pointer
template <class T, class Deleter = default_delete<T>> class unique_ptr {
public:
  using pointer =
      std::conditional_t<has_pointer_type<std::remove_reference_t<Deleter>>,
                         typename std::remove_reference_t<Deleter>::pointer,
                         T *>;
  using element_type = T;
  using deleter_type = Deleter;

private:
  m_compressed_pair<deleter_type, pointer> m_pair;

public:
  void show() {
    std::println("pointer={}, size={}", m_pair.get_second(), sizeof(m_pair));
  }
  // ctor
  constexpr unique_ptr() noexcept
    requires unique_ptr_enable_default_t<deleter_type>
      : m_pair(m_zero_then_variadic_args_t{}) {}

  constexpr unique_ptr(std::nullptr_t) noexcept
    requires unique_ptr_enable_default_t<deleter_type>
      : unique_ptr() {}

  unique_ptr(const unique_ptr &) = delete;

  // member functions

  unique_ptr &operator=(const unique_ptr &) = delete;

  // modifiers
  constexpr pointer release() noexcept {
    return std::exchange(m_pair.get_second(), nullptr);
  }
  constexpr void reset(pointer ptr = nullptr) noexcept {
    if (auto old_ptr = std::exchange(m_pair.get_second(), ptr)) {
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
  [[nodiscard]] constexpr typename std::add_lvalue_reference_t<element_type>
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
