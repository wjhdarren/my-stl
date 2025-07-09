#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace my {

template <typename T> class vector {
public:
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using iterator = T *;
  using const_iterator = const T *;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
  static inline constexpr float REALLOCATION_FACTOR = 2;

  pointer allocate(size_type n) {
    return static_cast<pointer>(operator new(n * sizeof(value_type)));
  }

  void deallocate(pointer p) { operator delete(p); }

  pointer m_data;
  size_type m_size;
  size_type m_capacity;

public:
  // for debug
  void show() {
    for (const auto &v : *this) {
      std::cout << v << " ";
    }
    std::cout << "\n";
    std::cout << "size: " << m_size << "\n";
    std::cout << "capacity: " << m_capacity << "\n";
  }

  // ctor
  constexpr vector() noexcept : m_data{}, m_size{}, m_capacity{} {}
  explicit vector(size_type count) : vector(count, T{}) {}
  vector(size_type count, const_reference value) {
    m_data = allocate(count);
    try {
      std::uninitialized_fill_n(begin(), count, value);
      m_size = m_capacity = count;
    } catch (...) {
      deallocate(m_data);
      throw;
    }
  }

  template <typename InputIt> vector(InputIt first, InputIt last) {
    size_type count = std::distance(first, last);
    m_data = allocate(count);
    auto constructed_end = m_data;
    try {
      constructed_end = std::uninitialized_copy(first, last, m_data);
      m_size = m_capacity = count;
    } catch (...) {
      std::destroy(m_data, constructed_end);
      deallocate(m_data);
      throw;
    }
  }

  // copy ctor
  vector(const vector &other) {
    size_type count = other.size();
    m_data = allocate(count);
    auto constructed_end = m_data;
    try {
      constructed_end =
          std::uninitialized_copy(other.cbegin(), other.cend(), m_data);
      m_size = m_capacity = count;
    } catch (...) {
      std::destroy(m_data, constructed_end);
      deallocate(m_data);
      throw;
    }
  }

  // move ctor
  vector(vector &&other) noexcept {
    m_data = other.m_data;
    m_size = other.m_size;
    m_capacity = other.m_capacity;
    other.m_data = nullptr;
    other.m_size = other.m_capacity = 0;
  }

  // initializer list
  vector(std::initializer_list<value_type> ilist) {
    size_type count = ilist.size();
    m_data = allocate(count);
    m_size = m_capacity = count;
    auto constructed_end = m_data;
    try {
      constructed_end =
          std::uninitialized_copy(ilist.begin(), ilist.end(), m_data);
      m_size = m_capacity = count;
    } catch (...) {
      std::destroy(m_data, constructed_end);
      deallocate(m_data);
      throw;
    }
  }

  // dtor
  ~vector() {
    std::destroy(begin(), end());
    deallocate(m_data);
  }

  // copy assignment operator
  vector &operator=(const vector &other) {
    if (this != &other) {
      vector temp(other);
      swap(temp);
    }
    return *this;
  }

  vector &operator=(vector &&other) noexcept {
    if (this != &other) {
      std::destroy(begin(), end());
      deallocate(m_data);
      m_data = other.m_data;
      m_size = other.m_size;
      m_capacity = other.m_capacity;
      other.m_data = nullptr;
      other.m_size = other.m_capacity = 0;
    }
    return *this;
  }

  vector &operator=(std::initializer_list<value_type> ilist) {
    vector temp(ilist);
    swap(temp);
    return *this;
  }

  // element access
  reference at(size_type pos) {
    if (pos >= size()) {
      throw std::out_of_range("my::vector::at: index out of range");
    }
    return *(begin() + pos);
  }
  const_reference at(size_type pos) const {
    if (pos >= size()) {
      throw std::out_of_range("my::vector::at: index out of range");
    }
    return *(begin() + pos);
  }
  reference operator[](size_type pos) { return *(begin() + pos); }
  const_reference operator[](size_type pos) const { return *(begin() + pos); }
  reference front() { return *begin(); }
  const_reference front() const { return *begin(); }
  reference back() { return *(end() - 1); }
  const_reference back() const { return *(end() - 1); }
  pointer data() { return m_data; }
  const_pointer data() const { return m_data; }

  // iterators
  iterator begin() { return m_data; }

  const_iterator begin() const { return m_data; }

  const_iterator cbegin() const noexcept { return m_data; }

  iterator end() { return m_data + m_size; }

  const_iterator end() const { return m_data + m_size; }

  const_iterator cend() const noexcept { return m_data + m_size; }

  reverse_iterator rbegin() { return reverse_iterator(end()); }

  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(end());
  }

  reverse_iterator rend() { return reverse_iterator(begin()); }

  const_reverse_iterator crend() const {
    return const_reverse_iterator(begin());
  }

  // capacity
  [[nodiscard]] bool empty() const noexcept { return (size() == 0); }
  [[nodiscard]] size_type size() const noexcept { return m_size; }
  [[nodiscard]] size_type max_size() const noexcept {
    return std::numeric_limits<difference_type>::max();
  }
  void reserve(size_type new_cap) {
    if (new_cap > max_size())
      throw std::length_error(
          "my::vector::reserve: can't reserve space greater than max_size()!");
    if (capacity() >= new_cap)
      return;
    pointer new_data = allocate(new_cap);
    std::uninitialized_move(begin(), end(), new_data);
    std::destroy(begin(), end());
    deallocate(m_data);
    m_data = new_data;
    m_capacity = new_cap;
  }

  [[nodiscard]] size_type capacity() const noexcept { return m_capacity; };

  void shrink_to_fit() {
    if (capacity() == size())
      return;

    if (size() == 0) {
      std::destroy(begin(), end());
      deallocate(m_data);
      m_data = nullptr;
      m_capacity = 0;
      return;
    }

    pointer new_data = allocate(size());
    try {
      std::uninitialized_move(begin(), end(), new_data);
      std::destroy(begin(), end());
      deallocate(m_data);
      m_data = new_data;
      m_capacity = size();
    } catch (...) {
      deallocate(new_data);
      throw;
    }
  }

  // modifiers
  void clear() {
    std::destroy(begin(), end());
    m_size = 0;
  }

  // insert
  iterator insert(const_iterator pos, const_reference value) {
    emplace(pos, value);
  }

  iterator insert(const_iterator pos, T &&value) {
    emplace(pos, std::move(value));
  }

  iterator insert(const_iterator pos, size_type count, const T &value) {
    assert((pos >= begin()) && (pos <= end()));

    if (count == 0) {
      return begin() + std::distance(cbegin(), pos);
    }

    auto idx = static_cast<size_type>(std::distance(cbegin(), pos));
    auto old_size = size();

    if (count + old_size > capacity()) {
      // Case 1: re-allocate new spaces
      auto new_cap =
          std::max(count + old_size,
                   static_cast<size_type>(REALLOCATION_FACTOR * capacity()));
      pointer new_data = allocate(new_cap);
      std::uninitialized_move(begin(), begin() + idx, new_data);
      std::uninitialized_move(begin() + idx, begin() + old_size,
                              new_data + idx + count);
      std::uninitialized_fill_n(new_data + idx, count, value);

      std::destroy(begin(), end());
      deallocate(m_data);

      m_data = new_data;
      m_capacity = new_cap;
    } else {
      auto old_end = end();
      value_type value_copy =
          value; // Fix self-reference: make a copy of value before reallocation
      if (count + idx <= old_size) {
        // Case 2: number of inserted elements are "small"
        std::uninitialized_move(end() - count, end(), end());
        std::move_backward(begin() + idx, end() - count, end());
        std::fill_n(begin() + idx, count, value_copy);
      } else {
        // Case 3: number of inserted elements are "large"
        std::uninitialized_move(begin() + idx, end(), begin() + idx + count);
        std::fill_n(begin() + idx, old_size - idx, value_copy);
        std::uninitialized_fill_n(end(), count - (old_size - idx), value_copy);
      }
    }
    m_size = old_size + count;
    return begin() + idx;
  }

  template <typename InputIt>
  iterator insert(const_iterator pos, InputIt first, InputIt last) {
    // ub if either first or last are iterators into *this
    assert((pos >= begin()) && (pos <= end()));

    if (first == last) {
      return begin() + std::distance(cbegin(), pos);
    }
    auto idx = static_cast<size_type>(std::distance(cbegin(), pos));
    auto old_size = size();
    auto count = static_cast<size_type>(std::distance(first, last));

    if (count + old_size > capacity()) {
      // Case 1: re-allocate new spaces
      auto new_cap =
          std::max(count + old_size,
                   static_cast<size_type>(REALLOCATION_FACTOR * capacity()));
      pointer new_data = allocate(new_cap);
      std::uninitialized_move(begin(), begin() + idx, new_data);
      std::uninitialized_move(begin() + idx, begin() + old_size,
                              new_data + idx + count);
      std::uninitialized_copy(first, last, new_data + idx);

      std::destroy(begin(), end());
      deallocate(m_data);

      m_data = new_data;
      m_capacity = new_cap;

    } else {
      if (count + idx <= old_size) {
        // Case 2: number of inserted elements are "small"
        std::uninitialized_move(end() - count, end(), end());
        std::move_backward(begin() + idx, end() - count, end());
        std::copy(first, last, begin() + idx);
      } else {
        // Case 3: number of inserted elements are "large"
        std::uninitialized_move(begin() + idx, end(), begin() + idx + count);
        std::copy(first, first + old_size - idx, begin() + idx);
        std::uninitialized_copy(first + old_size - idx, last, end());
      }
    }
    m_size = old_size + count;
    return begin() + idx;
  }

  iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
    return insert(pos, ilist.begin(), ilist.end());
  }

  // emplace
  template <class... Args>
  constexpr iterator emplace(const_iterator pos, Args &&...args) {
    assert((pos >= begin()) && (pos <= end()));
    auto idx = static_cast<size_type>(std::distance(cbegin(), pos));
    if (capacity() == size()) {
      reserve(capacity() == 0
                  ? 1
                  : static_cast<size_type>(REALLOCATION_FACTOR * capacity()));
    }

    if (idx < size()) {
      new (end()) T(std::move(back()));
      std::move_backward(begin() + idx, end() - 1, end());
      at(idx) = T(std::forward<Args>(args)...);
    } else {
      new (end()) T(std::forward<Args>(args)...);
    }

    ++m_size;
    return begin() + idx;
  }

  // erase
  constexpr iterator erase(const_iterator pos) {

    assert((pos >= begin()) && (pos < end()));

    auto idx = static_cast<size_type>(pos - begin());

    std::move(pos + 1, end(), pos);

    --m_size;

    std::destroy_at(end());
    return begin() + idx;
  }

  constexpr iterator erase(const_iterator first, const_iterator last) {
    assert(first <= last);
    assert(first >= begin() && last <= end());

    if (first == last)
      return begin() + (first - begin());

    auto num_to_erase = static_cast<size_type>(last - first);
    auto idx = static_cast<size_type>(first - begin());

    std::move(last, end(), first);

    m_size -= num_to_erase;

    std::destroy(end(), end() + num_to_erase);

    return begin() + idx;
  }

  void push_back(const_reference value) { emplace_back(value); }

  void push_back(value_type &&value) { emplace_back(std::move(value)); }

  template <class... Args> reference emplace_back(Args &&...args) {
    if (capacity() >= size() + 1) {
      new (end()) T(std::forward<Args>(args)...);
      ++m_size;
      return back();
    } else {
      // Reallocation needed - construct temporary first to handle
      // self-references
      T temp(std::forward<Args>(args)...);

      size_type new_cap =
          (capacity() == 0)
              ? 1
              : static_cast<size_type>(REALLOCATION_FACTOR * capacity());
      reserve(new_cap);

      new (end()) T(std::move(temp));
      ++m_size;
      return back();
    }
  }

  void pop_back() {
    if (empty())
      return;
    back().~T();
    --m_size;
  }

  void resize(size_type count) {
    if (count == size())
      return;
    if (count < size()) {
      std::destroy(begin() + count, end());
    } else {
      reserve(count);
      std::uninitialized_fill_n(end(), count - size(), T{});
    }
    m_size = count;
  }

  void resize(size_type count, const T &value) {
    if (count == size())
      return;

    if (count < size()) {
      std::destroy(begin() + count, end());
    } else {
      reserve(count);
      std::uninitialized_fill_n(end(), count - size(), value);
    }
    m_size = count;
  }

  void swap(vector &other) noexcept {
    if (this != &other) {
      std::swap(m_data, other.m_data);
      std::swap(m_size, other.m_size);
      std::swap(m_capacity, other.m_capacity);
    }
  }

  // Range-related methods
  // template< container-compatible-range<T> R >
  // constexpr iterator insert_range( const_iterator pos, R&& rg );
};

// Non-member functions
template <typename T>
constexpr auto operator<=>(const vector<T> &lhs, const vector<T> &rhs) {
  return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(),
                                                rhs.begin(), rhs.end());
}

} // namespace my
