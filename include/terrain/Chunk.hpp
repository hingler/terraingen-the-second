#ifndef CHUNK_H_
#define CHUNK_H_

#include "terrain/VertexGenerator.hpp"
#include "terrain/Vertex.hpp"

#include "lod/lod_node.hpp"

#include <memory>
#include <vector>

// build a single chunk at a time
namespace terraingen {
  namespace terrain {    
    struct Chunk {
      Vertex* vertex_data;
      unsigned int* index_data;

      size_t vertex_count;
      size_t index_count;

      /**
       * @brief Creates a new chunk.
       * 
       * @tparam HeightMap - format for height map
       * @param vert_generator - vertex generator.
       * @param offset_x - x offset
       * @param offset_y - y offset
       * @param offset_index - integer offset for indices.
       * @param step - step size for offset.
       * @param chunk_res - number of quads along each axis
       * @param lod - root of our LOD tree.
       * @return std::shared_ptr<Chunk> - pointer to populated chunk.
       */
      template <typename HeightMap>
      static std::shared_ptr<Chunk> chunk_create(
        VertexGenerator<HeightMap>& vert_generator,
        long offset_x,
        long offset_y,
        unsigned int offset_index,
        size_t step,
        size_t chunk_res,
        const lod::lod_node* lod) 
      {
        Vertex* vert_data = new Vertex[(chunk_res + 1) * (chunk_res + 1)];
        for (int y = 0; y <= chunk_res; y++) {
          for (int x = 0; x <= chunk_res; x++) {
            vert_data[x * (chunk_res + 1) + y] = vert_generator.CreateVertex(x * step + offset_x, y * step + offset_y, step, lod);
          }
        }

        auto res = std::shared_ptr<Chunk>(new Chunk());
        res->vertex_data = vert_data;
        res->vertex_count = (chunk_res + 1) * (chunk_res + 1);
        res->index_count = 0;

        return res;
      }

      // dtor
      ~Chunk();

      // disable copy
      Chunk(const Chunk& other) = delete;
      Chunk& operator=(const Chunk& other) = delete;

      // move
      Chunk(Chunk&& other);
      Chunk& operator=(Chunk&& other);
    private:
      Chunk() {};
    };
  }
}

#endif // CHUNK_H_