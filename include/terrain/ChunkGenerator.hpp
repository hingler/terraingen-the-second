#ifndef CHUNK_GENERATOR_H_
#define CHUNK_GENERATOR_H_

#include "terrain/Vertex.hpp"
#include "util/LRUCache.hpp"
#include "traits/height_map.hpp"
#include "lod/lod_node.hpp"

#include "terrain/Chunk.hpp"
#include "terrain/ChunkIdentifier.hpp"

#include <memory>

namespace terraingen {
  namespace terrain {
    // initialize with
    // - height map
    // - size (PoT + 1)
    // - chunk size (PoT + 1)

    // take in a LoD tree and generate geometry for it
    // - for edges, read relevant border cell, match its scale (and its tangents)

    // for vertex data alignment when sending to GPU
    template <typename HeightMap>
    class ChunkGenerator {
      static_assert(sizeof(Vertex) == 44);
      static_assert(traits::height_map<HeightMap>::value);
    public:
      ChunkGenerator(
        std::shared_ptr<HeightMap> height,
        float horizontal_scale,
        double texcoord_scale,
        const glm::vec3& terrain_offset) 
        : chunk_data_(256),
          height_(height),
          horizontal_scale_(horizontal_scale),
          texcoord_scale_(texcoord_scale),
          terrain_offset_(terrain_offset)
      {
        // store all of this
      }

      void UpdateChunks(const lod::lod_node* nodes, size_t tree_res) {
        index_offset = 0;
        chunk_data_.Reserve(GetChunkCount_recurse(nodes) + 1);
        chunk_count_ = UpdateChunks_recurse(0, 0, tree_res, tree_res, nodes);
      }
    private:
      size_t UpdateChunks_recurse(
        size_t offset_x,
        size_t offset_y,
        size_t chunk_res,
        size_t tree_res,
        const lod::lod_node* node
      ) {
        // if children are null, draw
        if (node->tl == nullptr) {
          ChunkIdentifier identifier { offset_x, offset_y, chunk_res };
          // see if we have this one stored

          std::shared_ptr<Chunk> chunk = Chunk::chunk_create(height_, offset_x, offset_y, index_offset, tree_res, chunk_res, terrain_offset_, node);
          chunk_data_.Put(identifier, chunk);

          // difficulty: res is 1024, terrain is 1025
          // let's just stick with that for now (our splat should be sample-able one unit larger
          // but we'll pretend the chunk itself is a round pot because the math works better
          index_offset += chunk->vertex_count;
          return 1;
        } else {
          assert(node->tr != nullptr);
          assert(node->bl != nullptr);
          assert(node->br != nullptr);
          size_t half_res = chunk_res >> 1;
          size_t center_x = offset_x + half_res;
          size_t center_y = offset_y + half_res;

          size_t chunk_count = 0;
          chunk_count += UpdateChunks_recurse(offset_x,            offset_y,            half_res, tree_res, node->bl);
          chunk_count += UpdateChunks_recurse(offset_x + half_res, offset_y,            half_res, tree_res, node->br);
          chunk_count += UpdateChunks_recurse(offset_x,            offset_y + half_res, half_res, tree_res, node->tl);
          chunk_count += UpdateChunks_recurse(offset_x + half_res, offset_y + half_res, half_res, tree_res, node->tr);
          return chunk_count;
        }
      }

      int GetChunkCount_recurse(const lod::lod_node* node) {
        if (node == nullptr) {
          return 0;
        }

        // current chunk is valid
        int chunk_count = 1;
        chunk_count += GetChunkCount_recurse(node->tl);
        chunk_count += GetChunkCount_recurse(node->tr);
        chunk_count += GetChunkCount_recurse(node->bl);
        chunk_count += GetChunkCount_recurse(node->br);
        return chunk_count;
      }

      util::LRUCache<ChunkIdentifier, std::shared_ptr<Chunk>> chunk_data_;
      // our chunks will be at the front of the 

      std::shared_ptr<HeightMap> height_;
      float horizontal_scale_;
      double texcoord_scale_;
      glm::vec3 terrain_offset_;

      size_t chunk_count_;

      unsigned int index_offset;
    };  
  }
}

#endif // CHUNK_GENERATOR_H_