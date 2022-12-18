#ifndef LOD_TREE_GENERATOR_H_
#define LOD_TREE_GENERATOR_H_

#define CASCADE_MUL_FACTOR 4

#include "lod/lod_node.hpp"

#include <glm/glm.hpp>

#include "traits/height_map.hpp"

#include <memory>

namespace terraingen {
  namespace lod {
    /**
     * @brief Generates an LOD quadtree for a terrain object at an arbitrary location
     */
    template <typename HeightMap>
    class LodTreeGenerator {
      static_assert(traits::height_map<HeightMap>::value);
    public:
      LodTreeGenerator(std::shared_ptr<HeightMap> height_map, int size, int chunk_res)
       : height_map_(height_map),
         size_(size),
         chunk_res_(chunk_res)
      {
        // PoT + 1
        assert(((size) & (size - 1)) == 0);
        assert(((chunk_res) & (chunk_res - 1)) == 0);
        assert(chunk_res_ > 1);
        assert(size_ > 1);
      }



      lod_node* CreateLodTree(const glm::vec3& local_position);

      // distance cap for min subdivision level
      // other cascades are handled internally
      double cascade_factor;

      // max number of chunk subdivisions to perform
    private:
      const std::shared_ptr<HeightMap> height_map_;
      const int size_;
      const int chunk_res_;

      void CreateLodTree_recurse(int x, int y, int node_size, double cascade_threshold, const glm::vec3& local_position, lod_node* root);
    };

    template <typename HeightMap>
    lod_node* LodTreeGenerator<HeightMap>::CreateLodTree(const glm::vec3& local_position) {
      auto* node = lod_node::lod_node_alloc();
      int size = size_;
      double cascade_real = cascade_factor / CASCADE_MUL_FACTOR;
      while (size > chunk_res_) {
        cascade_real *= CASCADE_MUL_FACTOR;
        size >>= 1;
      }

      CreateLodTree_recurse(
        0,
        0,
        size_,
        cascade_real,
        local_position,
        node
      );

      return node;
    }

    template <typename HeightMap>
    void LodTreeGenerator<HeightMap>::CreateLodTree_recurse(
      int x, 
      int y,
      int node_size,
      double cascade_threshold,
      const glm::vec3& local_position,
      lod_node* root) 
    {
      // no longer descend
      if (node_size <= chunk_res_) {
        return;
      }


      float dist_to_chunk;
      float x_f = static_cast<float>(x);
      float y_f = static_cast<float>(y);

      // side note: we need to map z to height :(
      if (local_position.x < x || local_position.x > x + node_size || local_position.z < y || local_position.z > y + node_size) {
        glm::vec3 closest_point(glm::clamp(local_position.x, x_f, x_f + node_size), local_position.y, glm::clamp(local_position.z, y_f, y_f + node_size));
        dist_to_chunk = glm::length(closest_point - local_position);
      } else {
        // ignore z
        dist_to_chunk = 0.0;
      }

      if (dist_to_chunk > cascade_threshold) {
        return;
      }

      root->bl = lod_node::lod_node_alloc();
      root->br = lod_node::lod_node_alloc();
      root->tl = lod_node::lod_node_alloc();
      root->tr = lod_node::lod_node_alloc();

      double new_cascade_threshold = cascade_threshold / CASCADE_MUL_FACTOR;
      int new_node_size = node_size / 2;

      CreateLodTree_recurse(x,                 y,                 new_node_size, new_cascade_threshold, local_position, root->bl);
      CreateLodTree_recurse(x + new_node_size, y,                 new_node_size, new_cascade_threshold, local_position, root->br);
      CreateLodTree_recurse(x,                 y + new_node_size, new_node_size, new_cascade_threshold, local_position, root->tl);
      CreateLodTree_recurse(x + new_node_size, y + new_node_size, new_node_size, new_cascade_threshold, local_position, root->tr);
    }
  }
}

#endif // LOD_TREE_GENERATOR_H_