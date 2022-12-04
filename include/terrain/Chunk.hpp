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
    // chunks shouldn't take responsibility for their own construction (it doesn't make sense)
  
    // wrapper for chunk data, managed by chunk gen
    struct Chunk {
      Vertex* vertex_data;
      unsigned int* index_data;

      template <typename HeightMap>
      static Chunk chunk_create(
        const VertexGenerator<HeightMap>& vert_generator,
        size_t offset_x,
        size_t offset_y,
        size_t step,
        size_t tree_res,
        size_t chunk_res,
        const glm::vec3& terrain_offset,
        const lod::lod_node* lod) 
      {
        Vertex* vert_data = new Vertex[chunk_res * chunk_res];
        unsigned int* ind_data = new unsigned int[(chunk_res - 1) * (chunk_res - 1) * 6];
        for (int y = 0; y <= chunk_res; y++) {
          for (int x = 0; x <= chunk_res; x++) {
            vert_data[x * chunk_res + y] = vert_generator->CreateVertex(x + offset_x, y + offset_y, chunk_res, step);
          }
        }

        size_t index_offset = 0;
        for (int y = 0; y < chunk_res; y++) {
          for (int x = 0; x < chunk_res; x++) {
            size_t index_init = (y * chunk_res + x);
            ind_data[index_offset++] = index_init;
            ind_data[index_offset++] = index_init + 1;
            ind_data[index_offset++] = index_init + chunk_res;
            ind_data[index_offset++] = index_init + 1;
            ind_data[index_offset++] = index_init + chunk_res + 1;
            ind_data[index_offset++] = index_init + chunk_res;
          }
        }

        Chunk res;
        res.vertex_data = vert_data;
        res.index_data = ind_data;
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