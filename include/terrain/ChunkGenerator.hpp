#ifndef CHUNK_GENERATOR_H_
#define CHUNK_GENERATOR_H_

#include "terrain/Vertex.hpp"
#include "util/LRUCache.hpp"
#include "traits/height_map.hpp"
#include "lod/lod_node.hpp"
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
      ChunkGenerator(std::shared_ptr<HeightMap> height, float horizontal_scale, double texcoord_scale, const glm::vec3& terrain_offset) {
        // store all of this
      }

      void UpdateChunks(const lod::lod_node* nodes) {
        // build chunks (create_chunk)
        // update our lru cache based on which chunks we encounter
        // at the end of it: string them together
      }
    };  
  }
}

#endif // CHUNK_GENERATOR_H_