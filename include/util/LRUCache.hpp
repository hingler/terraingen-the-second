#ifndef LRU_CACHE_H_
#define LRU_CACHE_H_

#include <list>
#include <unordered_map>

#include "util/HashList.hpp"
#include "terrain/ChunkGenerator.hpp"

namespace terraingen {
  namespace util {

    enum CachePutResult {
      // last element in cache was removed
      REMOVE_LAST,

      // wrote over inserted element
      OVERWRITE,

      // succeeded!
      SUCCESS
    };

    // insertion and removal is O(1)
    // random access is O(n)
    // an iterator would go far faster...
    // but we'd need it custom :(

    /**
     * @brief LRU cache impl
     * 
     * @tparam KeyType - type for key
     * @tparam ValueType - type for value
     */
    template <typename KeyType, typename ValueType>
    class LRUCache {
    public:
      LRUCache(int capacity) : capacity_(capacity) {}
      bool Fetch(const KeyType& key, ValueType* output) {
        if (key_cache.Contains(key)) {
          key_cache.PushFront(key);
          *output = value_cache.at(key);
          return true;
        }

        return false;
      }

      bool Has(const KeyType& key) {
        return (key_cache.Contains(key));
      }

      /**
       * @brief ensure cache has capacity for specified items
       * 
       * @param new_capacity 
       */
      void Reserve(int new_capacity) {
        if (capacity_ < new_capacity) {
          capacity_ = new_capacity;
        }
      }

      int Capacity() {
        return capacity_;
      }

      // put, ignore result
      void Put(const KeyType& key, const ValueType& value) {
        int _;
        CachePutResult __ = Put(key, value, &_);
      }

      // output receives booted out value
      CachePutResult Put(const KeyType& key, const ValueType& value, ValueType* output) {
        CachePutResult res = SUCCESS;
        key_cache.PushFront(key);
        auto itr = value_cache.find(key);
        if (itr != value_cache.end()) {
          *output = itr->second;
          res = OVERWRITE;
        } else if (key_cache.Size() > capacity_) {
          KeyType key_last; 

          // might want to clean this up later
          // whatever :3
          bool key_available = key_cache.PopBack(&key_last);
          assert(key_available);
          assert(value_cache.find(key_last) != value_cache.end());
          *output = value_cache.at(key_last);
          res = REMOVE_LAST;
        }

        value_cache.insert_or_assign(key, value);
        return res;
      }

    private:
      HashList<KeyType> key_cache;
      std::unordered_map<KeyType, ValueType> value_cache;
      int capacity_;
    };     
  }
}

#endif // LRU_CACHE_H_