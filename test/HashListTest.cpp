#include <gtest/gtest.h>

#include "util/HashList.hpp"
#include "util/LRUCache.hpp"
#include "terrain/Vertex.hpp"
#include "terrain/VertexGenerator.hpp"

#include <iostream>

using namespace terraingen;
using namespace util;

TEST(HashListTest, SimpleCreateInsert) {
  HashList<int> list;
  list.PushFront(1);
  list.PushFront(2);

  ASSERT_TRUE(list.Contains(1));
  ASSERT_TRUE(list.Contains(2));
  ASSERT_FALSE(list.Contains(3));
}

TEST(HashListTest, PushBackInit) {
  HashList<int> list;
  list.PushBack(1);
  list.PushFront(2);

  ASSERT_TRUE(list.Contains(1));
  ASSERT_TRUE(list.Contains(2));
  ASSERT_FALSE(list.Contains(3));

  int output;
  ASSERT_TRUE(list.PopBack(&output));
  ASSERT_EQ(output, 1);
  ASSERT_TRUE(list.PopBack(&output));
  ASSERT_EQ(output, 2);
}

TEST(HashListTest, RemoveInPlace) {
  HashList<unsigned int> list;
  for (unsigned int i = 0; i < 256; i++) {
    list.PushBack(i);
  }

  for (unsigned int i = 0; i < 256; i++) {
    unsigned int t = i;
    t = ((t & 0x0F) << 4) | ((t & 0xF0) >> 4);
    t = ((t & 0x33) << 2) | ((t & 0xCC) >> 2);
    t = ((t & 0x55) << 1) | ((t & 0xAA) >> 1);

    ASSERT_TRUE(list.Contains(t));
    ASSERT_TRUE(list.RemoveKey(t));
    ASSERT_FALSE(list.Contains(t));
  }
}

TEST(HashListTest, InsertDuplicate) {
  HashList<int> list;
  for (int i = 0; i < 64; i++) {
    list.PushBack(i);
  }

  int output;
  for (int i = 64; i < 48; i++) {
    // we inserted in increasing order
    list.PushFront(i);

    // ensure reinserted value no longer on back
    list.PopBack(&output);
    ASSERT_NE(output, i);

    // ensure reinserted value is now on front
    list.PopFront(&output);
    ASSERT_EQ(output, i);
  }
}

TEST(HashListTest, PushLastElementToFront) {
  HashList<int> list;
  for (int i = 0; i < 64; i++) {
    list.PushBack(i);
  }

  int output;
  list.PushFront(63);
  list.PopBack(&output);
  ASSERT_EQ(output, 62);
}

TEST(HashListTest, PushFirstElementOntoLast) {
  HashList<int> list;
  for (int i = 0; i < 64; i++) {
    list.PushBack(i);
  }

  int output;
  list.PushBack(0);
  list.PopFront(&output);
  ASSERT_EQ(output, 1);
}

TEST(HashListTest, VerifyIterator) {
  HashList<int> test;
  for (int i = 0; i < 512; i++) {
    test.PushBack(i);
  }

  int cur = 0;
  for (auto itr = test.begin(); itr != test.end(); itr++) {
    ASSERT_EQ(*itr, cur++);
  }

  ASSERT_EQ(cur, 512);

  auto itr = test.begin();
  for (int i = 0; i < 511; i++) {
    ++itr;
  }

  cur = 511;
  while (itr != test.end()) {
    ASSERT_EQ(*itr, cur--);
    --itr;
  }

  ASSERT_EQ(cur, -1);
}

TEST(HashListTest, StressTest) {
  HashList<int> list;

  // O3
  // (2 << 26) - 1 -> something like 65 million push/pop pairs
  // about 4 seconds (with assertions)
  // 30 mil modifications per second (sufficient for this)
  for (int size = 1; size < (1 << 25); size <<= 1) {
    for (int i = 0; i < size; i++) {
      list.PushFront(i);
    }

    int output;
    for (int i = size - 1; i >= 0; i--) {
      ASSERT_TRUE(list.PopFront(&output));
      ASSERT_EQ(output, i);
    }
  }
}