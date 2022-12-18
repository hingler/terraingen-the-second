#ifndef TERRAIN_GENERATOR_H_
#define TERRAIN_GENERATOR_H_

#include "traits/height_map.hpp"

#include "terrain/ChunkGenerator.hpp"
#include "lod/LodTreeGenerator.hpp"

#include <cstddef>
#include <memory>

// if this is the way we go, then this should be the only public thing
namespace terraingen {
  template <typename HeightMap>
  class TerrainGenerator {
  static_assert(traits::height_map<HeightMap>::value);
  public:
    TerrainGenerator(
      std::shared_ptr<HeightMap> heightmap,
      float horizontal_scale,
      double texcoord_scale,
      const glm::vec3& terrain_offset,
      size_t terrain_res,
      size_t chunk_res,
      double cascade_factor
    ) : heightmap_(heightmap),
        chunk_gen_(
          heightmap_,
          horizontal_scale,
          texcoord_scale,
          terrain_offset,
          chunk_res
        ),
        tree_gen_(
          heightmap_,
          terrain_res,
          chunk_res
        ),
        terrain_res_(terrain_res) {
          tree_gen_.cascade_factor = cascade_factor;
        }
    
    void UpdateChunkData(const glm::vec3& local_position) {
      // update lod tree
      lod::lod_node* tree = tree_gen_.CreateLodTree(local_position);
      chunk_gen_.UpdateChunks(tree, terrain_res_);
      lod::lod_node::lod_node_free(tree);
    }

    size_t GetChunkCount() {
      return chunk_gen_.GetChunkCount();
    }

    size_t GetVertexBufferSize() {
      return chunk_gen_.GetVertexBufferSize();
    }

    size_t GetIndexBufferSize() {
      return chunk_gen_.GetIndexBufferSize();
    }

    void WriteVertexBuffer(void* dst, size_t n) {
      return chunk_gen_.WriteVertexBuffer(dst, n);
    }

    size_t WriteVertexBufferSeparate(glm::vec3* positions, glm::vec3* normals, glm::vec2* texcoords, glm::vec4* tangents, const size_t vertices) {
      return chunk_gen_.WriteVertexBufferSeparate(positions, normals, texcoords, tangents, vertices);
    }

    size_t WriteIndexBuffer(void* dst, size_t n) {
      return chunk_gen_.WriteIndexBuffer(dst, n);
    }

  private:
    std::shared_ptr<HeightMap> heightmap_;
    terrain::ChunkGenerator<HeightMap> chunk_gen_;
    lod::LodTreeGenerator<HeightMap> tree_gen_;
    size_t terrain_res_;
  };
}

#endif // TERRAIN_GENERATOR_H_