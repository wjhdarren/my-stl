#pragma once
#include <memory>
#include <print>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <initializer_list>
#include <print>

namespace my {

template<typename T>
class vector {
public:
    using value_type             = T;
    using size_type              = size_t;
    using difference_type        = std::ptrdiff_t;
    using pointer                = T*;
    using const_pointer          = const T*;
    using reference              = T&;
    using const_reference        = const T&;
    using iterator               = T*;
    using const_iterator         = const T*;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    T* allocate(size_type n) {
        return static_cast<T*> (operator new(n*sizeof(value_type)));
    }

    void deallocate(T* p) {
        operator delete(p);
    }

    T* m_data;
    size_t m_size;
    size_t m_capacity;

public:
    // debug
    void show() {
        for (const auto& v : *this) {
            std::cout << v << " ";
        }
        std::cout << "\n";
        std::cout << "size: " << m_size << "\n";
        std::cout << "capacity: " << m_capacity << "\n";
    }

    //ctor
    constexpr vector() noexcept: m_data{}, m_size{}, m_capacity{} {}
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

    template<typename InputIt>
    vector(InputIt first, InputIt last) {
        size_type count = std::distance(first, last);
        m_data = allocate(count);
        try {
            std::uninitialized_copy(first, last, m_data);
            m_size = m_capacity = count;
        } catch (...) {
            deallocate(m_data);
            throw;
        }
    }

    // copy ctor
    vector(const vector& other) {
        size_type count = other.size();
        m_data = allocate(count);
        try {
            std::uninitialized_copy(other.cbegin(), other.cend(), m_data);
            m_size = m_capacity = count;
        } catch (...) {
            deallocate(m_data);
            throw;
        }
    }

    // move ctor
    vector(vector&& other) noexcept {
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
        try {
            std::uninitialized_copy(ilist.begin(), ilist.end(), m_data);
            m_size = m_capacity = count;
        } catch (...) {
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
    vector& operator=(const vector& other) {
        if (this != &other) {
            vector temp(other);
            swap(temp);
        }
        return *this;
    }

    vector& operator=(vector&& other) noexcept {
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

    vector& operator=(std::initializer_list<value_type> ilist) {
        vector temp(ilist);
        swap(temp);
        return *this;
    }

    // element access
    reference at(size_type pos) {
        if (pos >= size()) {
            throw std::out_of_range("my::vector::at: index out of range");
        }
        return *(begin()+pos);
    }
    const_reference at(size_type pos) const {
        if (pos >= size()) {
            throw std::out_of_range("my::vector::at: index out of range");
        }
        return *(begin()+pos);
    }
    reference operator[](size_type pos) {
        return *(begin()+pos);
    }
    const_reference operator[](size_type pos) const {
        return *(begin()+pos);
    }
    reference front() {
        return *begin();
    }
    const_reference front() const {
        return *begin();
    }
    reference back() {
        return *(end() - 1);
    }
    const_reference back() const {
        return *(end() - 1);
    }
    pointer data() {
        return m_data;
    }
    const_pointer data() const {
        return m_data;
    }

    // iterators
    iterator begin() {
        return m_data;
    }

    const_iterator cbegin() const {
        return m_data;
    }

    iterator end() {
        return m_data + m_size;
    }

    const_iterator cend() const {
        return m_data + m_size;
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(begin());
    }

    // capacity
    bool empty() const noexcept {
        return (size() == 0);
    }
    size_type size() const noexcept {
        return m_size;
    }
    size_type max_size() const noexcept {
        return std::numeric_limits<difference_type>::max();
    }
    void reserve(size_type new_cap) {
        if (new_cap > max_size())
            throw std::length_error("my::vector::reserve: can't reserve space greater than max_size()!");
        if (capacity() >= new_cap)
            return;
        T* new_data = allocate(new_cap);
        std::uninitialized_copy(begin(), end(), new_data);
        std::destroy(begin(), end());
        deallocate(m_data);
        m_data = new_data;
        m_capacity = new_cap;
    }

    size_type capacity() const noexcept {
        return m_capacity;
    };

    void shrink_to_fit() {
        if (capacity() == size())
            return;
        m_capacity = size();
    }

    // modifiers
    void clear() {
        std::destroy(begin(), end());
        m_size = 0;
    }
    void insert();
    void insert_range();
    void emplace();
    void erase();
    void push_back(const_reference value);
    void push_back(value_type&& vale);
    void emplace_back();
    void append_range();
    void pop_back();
    void resize();
    void swap(vector& other) noexcept {
        if (this != &other) {
            std::swap(m_data, other.m_data);
            std::swap(m_size, other.m_size);
            std::swap(m_capacity, other.m_capacity);
        }
    };
};


} // namespace my