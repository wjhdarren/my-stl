#include "../my/vector.h"
#include <gtest/gtest.h>
#include <vector>

using namespace my;

// Constructor Tests
TEST(VectorTest, VectorConstructorTest) {
    // Default constructor
    vector<std::string> v1;
    EXPECT_TRUE(v1.empty());
    EXPECT_EQ(v1.size(), 0);
    EXPECT_EQ(v1.capacity(), 0);

    // Count constructor with default value
    vector<std::string> v2(5);
    EXPECT_EQ(v2.size(), 5);
    EXPECT_EQ(v2.capacity(), 5);
    for (const auto& val : v2) {
        EXPECT_EQ(val, "");
    }

    // Count constructor with specific value
    vector<std::string> v3(3, "hello");
    EXPECT_EQ(v3.size(), 3);
    for (const auto& val : v3) {
        EXPECT_EQ(val, "hello");
    }

    // Initializer list constructor
    vector<std::string> v4{"first", "second", "third", "fourth", "fifth"};
    EXPECT_EQ(v4.size(), 5);
    EXPECT_EQ(v4[0], "first");
    EXPECT_EQ(v4[1], "second");
    EXPECT_EQ(v4[4], "fifth");

    // Iterator range constructor
    std::vector<std::string> std_vec{"alpha", "beta", "gamma"};
    vector<std::string> v5(std_vec.begin(), std_vec.end());
    EXPECT_EQ(v5.size(), 3);
    EXPECT_EQ(v5[0], "alpha");
    EXPECT_EQ(v5[1], "beta");
    EXPECT_EQ(v5[2], "gamma");
}

TEST(VectorTest, VectorCopyOperatorTest) {
    // Copy constructor
    vector<std::string> original{"apple", "banana", "cherry", "date", "elderberry"};
    vector<std::string> copied(original);
    EXPECT_EQ(copied.size(), original.size());
    EXPECT_EQ(copied, original);
    
    // Modify original to ensure deep copy
    original[0] = "modified";
    EXPECT_NE(copied[0], original[0]);
    EXPECT_EQ(copied[0], "apple");

    // Copy assignment operator
    vector<std::string> assigned;
    assigned = original;
    EXPECT_EQ(assigned.size(), original.size());
    EXPECT_EQ(assigned, original);

    // Self-assignment
    assigned = assigned;
    EXPECT_EQ(assigned.size(), original.size());
    EXPECT_EQ(assigned, original);
}

TEST(VectorTest, VectorMoveOperatorTest) {
    // Move constructor
    vector<std::string> original{"cat", "dog", "bird", "fish", "hamster"};
    size_t original_size = original.size();
    vector<std::string> moved(std::move(original));
    
    EXPECT_EQ(moved.size(), original_size);
    EXPECT_EQ(original.size(), 0);
    EXPECT_TRUE(original.empty());
    EXPECT_EQ(moved[0], "cat");
    EXPECT_EQ(moved[4], "hamster");

    // Move assignment operator
    vector<std::string> source{"red", "green", "blue"};
    vector<std::string> target;
    target = std::move(source);
    
    EXPECT_EQ(target.size(), 3);
    EXPECT_EQ(source.size(), 0);
    EXPECT_EQ(target[0], "red");
    EXPECT_EQ(target[2], "blue");
}

// Element Access Tests
TEST(VectorTest, ElementAccessTest) {
    vector<int> v{10, 20, 30, 40, 50};
    
    // operator[]
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[4], 50);
    v[1] = 200;
    EXPECT_EQ(v[1], 200);

    // at() with bounds checking
    EXPECT_EQ(v.at(0), 10);
    EXPECT_EQ(v.at(4), 50);
    EXPECT_THROW(v.at(5), std::out_of_range);

    // front() and back()
    EXPECT_EQ(v.front(), 10);
    EXPECT_EQ(v.back(), 50);
    v.front() = 100;
    v.back() = 500;
    EXPECT_EQ(v.front(), 100);
    EXPECT_EQ(v.back(), 500);

    // data()
    EXPECT_EQ(*v.data(), v[0]);
    EXPECT_NE(v.data(), nullptr);
}

// Iterator Tests
TEST(VectorTest, IteratorTest) {
    vector<int> v{1, 2, 3, 4, 5};
    
    // Forward iteration
    int expected = 1;
    for (auto it = v.begin(); it != v.end(); ++it, ++expected) {
        EXPECT_EQ(*it, expected);
    }

    // Range-based for loop
    expected = 1;
    for (const auto& val : v) {
        EXPECT_EQ(val, expected++);
    }

    // Reverse iteration
    expected = 5;
    for (auto it = v.rbegin(); it != v.rend(); ++it, --expected) {
        EXPECT_EQ(*it, expected);
    }

    // Const iterators
    const vector<int>& cv = v;
    expected = 1;
    for (auto it = cv.cbegin(); it != cv.cend(); ++it, ++expected) {
        EXPECT_EQ(*it, expected);
    }
}

// Capacity Tests
TEST(VectorTest, CapacityTest) {
    vector<std::string> v;
    
    // Empty vector
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);

    // Reserve capacity
    v.reserve(10);
    EXPECT_EQ(v.capacity(), 10);
    EXPECT_EQ(v.size(), 0);
    EXPECT_TRUE(v.empty());

    // Add elements
    v.push_back("first");
    v.push_back("second");
    EXPECT_FALSE(v.empty());
    EXPECT_EQ(v.size(), 2);
    EXPECT_GE(v.capacity(), 2);

    // Shrink to fit
    v.shrink_to_fit();
    EXPECT_EQ(v.capacity(), v.size());

    // Clear
    v.clear();
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);
}

// Modifier Tests
TEST(VectorTest, PushBackEmplaceBackTest) {
    vector<std::string> v;
    
    // push_back
    v.push_back("hello");
    v.push_back("world");
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v[0], "hello");
    EXPECT_EQ(v[1], "world");

    // emplace_back
    v.emplace_back("test");
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[2], "test");

    // pop_back
    v.pop_back();
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v.back(), "world");
}

TEST(VectorTest, InsertEraseTest) {
    vector<int> v{1, 2, 3, 4, 5};
    
    // Insert single element
    auto it = v.insert(v.begin() + 2, 99);
    EXPECT_EQ(v.size(), 6);
    EXPECT_EQ(*it, 99);
    EXPECT_EQ(v[2], 99);
    
    // Insert multiple elements
    v.insert(v.begin(), 3, 77);
    EXPECT_EQ(v.size(), 9);
    EXPECT_EQ(v[0], 77);
    EXPECT_EQ(v[1], 77);
    EXPECT_EQ(v[2], 77);

    // Insert from initializer list
    v.insert(v.end(), {88, 89});
    EXPECT_EQ(v.size(), 11);
    EXPECT_EQ(v[9], 88);
    EXPECT_EQ(v[10], 89);

    // Erase single element
    it = v.erase(v.begin());
    EXPECT_EQ(v.size(), 10);
    EXPECT_EQ(*it, 77);

    // Erase range
    it = v.erase(v.begin(), v.begin() + 2);
    EXPECT_EQ(v.size(), 8);
}

TEST(VectorTest, ResizeTest) {
    vector<std::string> v{"one", "two", "three"};
    
    // Resize to larger size with default value
    v.resize(5);
    EXPECT_EQ(v.size(), 5);
    EXPECT_EQ(v[3], "");
    EXPECT_EQ(v[4], "");

    // Resize to larger size with specific value
    v.resize(7, "new");
    EXPECT_EQ(v.size(), 7);
    EXPECT_EQ(v[5], "new");
    EXPECT_EQ(v[6], "new");

    // Resize to smaller size
    v.resize(3);
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[2], "three");
}

TEST(VectorTest, SwapTest) {
    vector<int> v1{1, 2, 3};
    vector<int> v2{4, 5, 6, 7, 8};
    
    v1.swap(v2);
    
    EXPECT_EQ(v1.size(), 5);
    EXPECT_EQ(v2.size(), 3);
    EXPECT_EQ(v1[0], 4);
    EXPECT_EQ(v2[0], 1);
}

// Comparison Tests
TEST(VectorTest, ComparisonTest) {
    vector<int> v1{1, 2, 3};
    vector<int> v2{1, 2, 3};
    vector<int> v3{1, 2, 4};
    vector<int> v4{1, 2};

    // Equality
    EXPECT_TRUE(v1 == v2);
    EXPECT_FALSE(v1 == v3);
    EXPECT_FALSE(v1 == v4);

    // Three-way comparison
    EXPECT_TRUE((v1 <=> v2) == std::strong_ordering::equal);
    EXPECT_TRUE((v1 <=> v3) == std::strong_ordering::less);
    EXPECT_TRUE((v1 <=> v4) == std::strong_ordering::greater);
}

// Assignment operator tests
TEST(VectorTest, AssignmentOperatorTest) {
    vector<std::string> v;
    
    // Initializer list assignment
    v = {"morning", "afternoon", "evening"};
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], "morning");
    EXPECT_EQ(v[2], "evening");

    // Copy assignment
    vector<std::string> other{"winter", "spring"};
    v = other;
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v[0], "winter");
    EXPECT_EQ(v[1], "spring");
}

// Exception safety tests
TEST(VectorTest, ExceptionSafetyTest) {
    vector<int> v;
    
    // at() bounds checking
    EXPECT_THROW(v.at(0), std::out_of_range);
    
    v.push_back(1);
    EXPECT_NO_THROW(v.at(0));
    EXPECT_THROW(v.at(1), std::out_of_range);
} 