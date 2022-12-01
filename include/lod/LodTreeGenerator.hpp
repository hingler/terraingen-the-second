#ifndef LOD_TREE_GENERATOR_H_
#define LOD_TREE_GENERATOR_H_

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
        assert(((size - 1) & (size - 2)) == 0);
        assert(((chunk_res - 1) & (chunk_res - 2)) == 0);
      }



      const lod_node* CreateLodTree(glm::vec3 local_position) {
        // don't use heightmap yet :/
        

      }

      // distance cap for min subdivision level
      // other cascades are handled internally
      float cascade_factor;

      // max number of chunk subdivisions to perform
    private:
      const std::shared_ptr<HeightMap> height_map_;
      const int size_;
      const int chunk_res_;

      void CreateLodTree_recurse(int x, int y, )
    };
  }
}

#endif // LOD_TREE_GENERATOR_H_

// i dont like having to pass to this based on traits
// might be good to move genericimage here or something :(
// but idk
// i'd like to keep this separate, outside of actually sampling the points and generating the geometry
// i think a wrapper class would be a fair approach for this
// considering that 'it's not necessarily the same flow'