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
    template <typename HeightMap>
    class ChunkGenerator {
      static_assert(sizeof(Vertex) == 44);
      static_assert(traits::height_map<HeightMap>::value);
    public:
      ChunkGenerator(
        std::shared_ptr<HeightMap> height,
        float horizontal_scale,
        double texcoord_scale,
        const glm::vec3& terrain_offset,
        size_t chunk_resolution) 
        : chunk_data_(256),
          height_(height),
          horizontal_scale_(horizontal_scale),
          texcoord_scale_(texcoord_scale),
          terrain_offset_(terrain_offset),
          chunk_res_(chunk_resolution)
      {
        // store all of this
      }

      void UpdateChunks(const lod::lod_node* node, size_t tree_res) {
        index_offset = 0;
        chunk_data_.Reserve(GetChunkCount_recurse(node) + 1);
        VertexGenerator<HeightMap> gen(
          height_,
          horizontal_scale_,
          texcoord_scale_,
          chunk_res_,
          tree_res,
          terrain_offset_
        );
        chunk_count_ = UpdateChunks_recurse(0, 0, tree_res, tree_res, node, node, gen);
      }

      // return number of chunks
      size_t GetChunkCount() {
        return chunk_count_;
      }

      // return vertex buffer size in bytes
      size_t GetVertexBufferSize() {
        return chunk_count_ * (chunk_res_ + 1) * (chunk_res_ + 1) * sizeof(Vertex);
      }

      // return index buffer size in bytes
      size_t GetIndexBufferSize() {
        return chunk_count_ * chunk_res_ * chunk_res_ * 6 * sizeof(unsigned int);
      }

      /**
       * @brief Writes vertex buffer to destination
       * 
       * @param dst - destination buffer
       * @param n - number of bytes in destination buffer
       * @return size_t - number of bytes written
       */
      size_t WriteVertexBuffer(void* dst, size_t n) {
        unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
        if (chunk_count_ <= 0) {
          return 0;
        }

        size_t chunk_size_bytes = (chunk_res_ + 1) * (chunk_res_ + 1) * sizeof(Vertex);
        size_t bytes_written = 0;
        for (auto itr = chunk_data_.begin_bounded(chunk_count_); itr != chunk_data_.end(); itr++) {
          if (n < chunk_size_bytes) {
            break;
          }

          memcpy(ptr, (*itr)->vertex_data, chunk_size_bytes);
          n -= chunk_size_bytes;
          bytes_written += chunk_size_bytes;
          ptr += chunk_size_bytes;
        }

        return bytes_written;
      }

      size_t WriteIndexBuffer(void* dst, size_t n) {
        unsigned int* ptr = reinterpret_cast<unsigned int*>(dst);
        if (chunk_count_ <= 0) {
          return 0;
        }

        size_t quad_size_bytes = 6 * sizeof(unsigned int);
        size_t bytes_written = 0;

        const size_t chunk_verts = (chunk_res_ + 1);

        size_t index_offset = 0;
        size_t index_step = chunk_verts * chunk_verts;
        for (int i = 0; i < chunk_count_; i++) {
          for (int y = 0; y < chunk_res_; y++) {
            for (int x = 0; x < chunk_res_; x++) {                        // ccw tris, bl -> tr
              *(ptr++) = index_offset + (y       * chunk_verts) + x;      // bl
              *(ptr++) = index_offset + (y       * chunk_verts) + x + 1;  // br
              *(ptr++) = index_offset + ((y + 1) * chunk_verts) + x + 1;  // tr
              *(ptr++) = index_offset + ((y + 1) * chunk_verts) + x + 1;  // tr
              *(ptr++) = index_offset + ((y + 1) * chunk_verts) + x;      // tl
              *(ptr++) = index_offset + (y       * chunk_verts) + x;      // bl

              bytes_written += quad_size_bytes;
              if (bytes_written > (n - quad_size_bytes)) {
                return bytes_written;
              }
            }
          }

          index_offset += index_step;
        }

        

        return bytes_written;
      }

    private:
      // builds chunks from an LOD tree recursively.
      size_t UpdateChunks_recurse(
        long offset_x,
        long offset_y,
        size_t chunk_size,
        size_t tree_res,
        const lod::lod_node* node,
        const lod::lod_node* tree,
        VertexGenerator<HeightMap>& vert_gen
      ) {
        // if children are null, draw
        if (node->tl == nullptr) {
          ChunkIdentifier identifier { offset_x, offset_y, chunk_size };
          std::shared_ptr<Chunk> chunk;
          if (!chunk_data_.Has(identifier)) {
            chunk = Chunk::chunk_create(vert_gen, offset_x, offset_y, index_offset, chunk_size / chunk_res_, chunk_res_, tree);
            chunk_data_.Put(identifier, chunk);
          } else {
            chunk_data_.Fetch(identifier, &chunk);
          }

          index_offset += chunk->vertex_count;
          return 1;
        } else {
          assert(node->tr != nullptr);
          assert(node->bl != nullptr);
          assert(node->br != nullptr);
          long half_size = chunk_size >> 1;

          size_t chunk_count = 0;
          chunk_count += UpdateChunks_recurse(offset_x,             offset_y,             half_size, tree_res, node->tl, tree, vert_gen);
          chunk_count += UpdateChunks_recurse(offset_x + half_size, offset_y,             half_size, tree_res, node->tr, tree, vert_gen);
          chunk_count += UpdateChunks_recurse(offset_x,             offset_y + half_size, half_size, tree_res, node->bl, tree, vert_gen);
          chunk_count += UpdateChunks_recurse(offset_x + half_size, offset_y + half_size, half_size, tree_res, node->br, tree, vert_gen);
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
      size_t chunk_res_;

      size_t chunk_count_;

      unsigned int index_offset;
    };  
  }
}

#endif // CHUNK_GENERATOR_H_