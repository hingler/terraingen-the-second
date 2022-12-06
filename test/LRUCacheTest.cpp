#include <gtest/gtest.h>

#include "util/LRUCache.hpp"

using namespace terraingen;
using namespace util;

TEST(LRUCacheTest, SimpleStorageRecall) {
  LRUCache<int, int> cache(4);
  int output_var;
  CachePutResult result = cache.Put(1, 1, &output_var);
  ASSERT_EQ(result, CachePutResult::SUCCESS);
  result = cache.Put(2, 2, &output_var);
  ASSERT_EQ(result, CachePutResult::SUCCESS);
  result = cache.Put(3, 3, &output_var);
  ASSERT_EQ(result, CachePutResult::SUCCESS);
  result = cache.Put(4, 4, &output_var);
  ASSERT_EQ(result, CachePutResult::SUCCESS);

  result = cache.Put(5, 5, &output_var);
  ASSERT_EQ(result, CachePutResult::REMOVE_LAST);
  ASSERT_EQ(output_var, 1);
}

TEST(LRUCacheTest, RefreshElement) {
  LRUCache<int, int> cache(4);
  int output_var;
  CachePutResult result = cache.Put(1, 1, &output_var);
  ASSERT_EQ(result, CachePutResult::SUCCESS);
  result = cache.Put(2, 2, &output_var);
  ASSERT_EQ(result, CachePutResult::SUCCESS);
  result = cache.Put(3, 3, &output_var);
  ASSERT_EQ(result, CachePutResult::SUCCESS);
  result = cache.Put(4, 4, &output_var);
  ASSERT_EQ(result, CachePutResult::SUCCESS);

  result = cache.Put(1, 16, &output_var);
  ASSERT_EQ(result, CachePutResult::OVERWRITE);
  ASSERT_EQ(output_var, 1);

  result = cache.Put(6, 6, &output_var);
  ASSERT_EQ(result, CachePutResult::REMOVE_LAST);
  ASSERT_EQ(output_var, 2);

  bool fetch_success = cache.Fetch(1, &output_var);
  ASSERT_TRUE(fetch_success);
  ASSERT_EQ(output_var, 16);
}

TEST(LRUCacheTest, VerifyUnboundedIterator) {
  LRUCache<int, int> cache(64);

  for (int i = 0; i < 256; i++) {
    cache.Put(i, i);
  }

  auto itr = cache.begin();
  int i = 255;
  while (itr != cache.end()) {
    ASSERT_EQ(*itr, i--);
    ++itr;
  }

  std::cout << (itr != cache.end()) << std::endl;

  ASSERT_TRUE(itr == cache.end());

  i = 255;
  for (auto itr = cache.begin_bounded(32); itr != cache.end(); ++itr) {
    ASSERT_EQ(*itr, i--);
  }

  ASSERT_EQ(i, 223);
}