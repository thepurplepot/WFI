#include "object.hh"
#include <gtest/gtest.h>
#include <iostream>

TEST(object, test_string_hash_key) {
    auto hello1 = object::String("Hello World");
    auto hello2 = object::String("Hello World");
    auto diff1 = object::String("My name is johnny");
    auto diff2 = object::String("My name is johnny");

    ASSERT_EQ(hello1.hash_key(), hello2.hash_key());
    ASSERT_EQ(diff1.hash_key(), diff2.hash_key());
    ASSERT_NE(hello1.hash_key(), diff1.hash_key());
}
