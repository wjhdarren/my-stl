#pragma once

#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <utility>

namespace my {
namespace detail {

template <class T> struct base_node;
template <class T> struct list_node;

// base_node class for sentinel node
template <class T> struct base_node {
  using base_pointer = base_node<T> *;
  using node_pointer = list_node<T> *;
  base_pointer prev;
  base_pointer next;

  constexpr base_node() noexcept = default;

  base_pointer self() { return static_cast<base_pointer>(&*this); }

  void unlink() { prev = next = self(); }

  node_pointer as_node() { return static_cast<node_pointer>(&*this); }
};

template <class T> struct list_node : public base_node<T> {
  using base_pointer = base_node<T> *;
  using node_pointer = list_node<T> *;

  T data;

  constexpr list_node() noexcept = default;
  constexpr list_node(const T &v) : data{v} {}
  constexpr list_node(T &&v) : data{std::move(v)} {}
  ~list_node() { data.~T(); }

  node_pointer self() { return static_cast<node_pointer>(&*this); }
  base_pointer as_base() { return static_cast<base_pointer>(&*this); }
};

template <class T> class list_iterator {
public:
  using value_type = T;
  using pointer = T *;
  using reference = T &;
  using iterator = list_iterator<T>;
  using iterator_concept = std::bidirectional_iterator_tag;
  using base_pointer = base_node<T> *;
  using node_pointer = list_node<T> *;

private:
  base_pointer m_node;

public:
  constexpr list_iterator() noexcept = default;
  constexpr list_iterator(base_pointer p) : m_node{p} {}
  constexpr list_iterator(node_pointer p) : m_node{p->as_base()} {}
  constexpr list_iterator(const iterator &other) = default;

  ~list_iterator() = default;

  reference operator*() const { return m_node->as_node()->data; }
  pointer operator->() const { return &(operator*()); }

  iterator &operator++() {
    m_node = m_node->next;
    return *this;
  }

  iterator operator++(int) {
    iterator temp = *this;
    ++(*this);
    return temp;
  }

  iterator &operator--() {
    m_node = m_node->prev;
    return *this;
  }

  iterator operator--(int) {
    iterator temp = *this;
    --(*this);
    return temp;
  }

  bool operator==(const iterator &other) const {
    return m_node == other.m_node;
  }

  bool operator!=(const iterator &other) const {
    return m_node != other.m_node;
  }
};

template <class T> class list_const_iterator {
public:
  using value_type = T;
  using const_pointer = const T *;
  using const_reference = const T &;
  using iterator = list_iterator<T>;
  using const_iterator = list_const_iterator<T>;
  using iterator_concept = std::bidirectional_iterator_tag;
  using base_pointer = base_node<T> *;
  using node_pointer = list_node<T> *;

private:
  base_pointer m_node;

public:
  constexpr list_const_iterator() noexcept = default;
  constexpr list_const_iterator(base_pointer p) : m_node{p} {}
  constexpr list_const_iterator(node_pointer p) : m_node{p->as_base()} {}
  constexpr list_const_iterator(const iterator &other) = default;
  constexpr list_const_iterator(const const_iterator &other) = default;

  ~list_const_iterator() = default;

  const_reference operator*() const { return m_node->as_node()->data; }
  const_pointer operator->() const { return &(operator*()); }

  const_iterator &operator++() {
    m_node = m_node->next;
    return *this;
  }

  const_iterator operator++(int) {
    const_iterator temp = *this;
    ++(*this);
    return temp;
  }

  const_iterator &operator--() {
    m_node = m_node->prev;
    return *this;
  }

  const_iterator operator--(int) {
    const_iterator temp = *this;
    --(*this);
    return temp;
  }

  bool operator==(const const_iterator &other) const {
    return m_node == other.m_node;
  }

  bool operator!=(const const_iterator &other) const {
    return m_node != other.m_node;
  }
};
} // namespace detail

template <class T> class list {
public:
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = T &;
  using const_reference = const T &;

  using pointer = T *;
  using base_pointer = detail::base_node<T> *;
  using node_pointer = detail::list_node<T> *;

  using const_pointer = const T *;
  using const_base_pointer = const detail::base_node<T> *;
  using const_node_pointer = const detail::list_node<T> *;
  using iterator = detail::list_iterator<T>;
  using const_iterator = detail::list_const_iterator<T>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
  base_pointer m_sentinel;
  size_type m_size;

  template <class... Args> node_pointer create_node(Args &&...args) {

    /* create a list_node of type T with provided args */

    auto *raw = new detail::list_node<value_type>;

    try {
      new (std::addressof(raw->data)) T(std::forward<Args>(args)...);
      raw->prev = nullptr;
      raw->next = nullptr;
      return raw;
    } catch (...) {
      delete raw;
      throw;
    }
  }

  void destroy_node(node_pointer p) { delete p; }

  base_pointer create_sentinel() {
    auto *raw = new detail::base_node<value_type>;

    raw->unlink(); // point to itself

    return raw;
  }

  void destroy_sentinel() {
    if (m_sentinel) {
      delete m_sentinel;
    }
  }

public:
  // ctor
  list() : m_sentinel{create_sentinel()}, m_size{0} {}

  explicit list(size_type count) : list(count, T()) {}

  explicit list(size_type count, const_reference value)
      : m_sentinel{create_sentinel()}, m_size{0} {
    try {
      auto curr = m_sentinel;
      for (size_type i = 0; i < count; ++i) {
        auto next = create_node(value);
        next->prev = curr;
        curr->next = next;
        curr = next;
        ++m_size;
      }
      curr->next = m_sentinel;
      m_sentinel->prev = curr;
    } catch (...) {
      clear();
      destroy_sentinel();
      throw;
    }
  }

  template <class InputIt>
  list(InputIt first, InputIt last) : m_sentinel{create_sentinel()}, m_size{0} {
    try {
      auto curr = m_sentinel;
      for (auto it = first; it != last; ++it) {
        auto next = create_node(*it);
        next->prev = curr;
        curr->next = next;
        curr = next;
        ++m_size;
      }
      curr->next = m_sentinel;
      m_sentinel->prev = curr;
    } catch (...) {
      clear();
      destroy_sentinel();
      throw;
    }
  }

  // copy ctor
  list(const list &other) : list(other.cbegin(), other.cend()) {}

  // move ctor
  list(list &&other) : m_sentinel(other.m_sentinel), m_size{other.m_size} {
    other.m_sentinel = nullptr;
    other.m_size = 0;
  }

  // iterators
  iterator begin() noexcept { return m_sentinel->next; }

  const_iterator begin() const noexcept { return m_sentinel->next; }

  const_iterator cbegin() const noexcept { return m_sentinel->next; }

  iterator end() noexcept { return m_sentinel; }

  const_iterator end() const noexcept { return m_sentinel; }

  const_iterator cend() const noexcept { return m_sentinel; }

  // capacity
  [[nodiscard]] bool empty() const { return m_size == 0; }

  [[nodiscard]] size_type size() const { return m_size; }

  [[nodiscard]] size_type max_size() const {
    return std::numeric_limits<difference_type>::max();
  }

  // destructor
  ~list() {
    clear();
    destroy_sentinel();
  }

  // modifiers
  void clear() {
    if (m_sentinel) {
      auto curr = m_sentinel->next;
      while (curr != m_sentinel) {
        auto next = curr->next;
        destroy_node(curr->as_node());
        curr = next;
      }
      m_sentinel->unlink();
      m_size = 0;
    }
  }
};
} // namespace my
