#ifndef LOD_NODE_H_
#define LOD_NODE_H_

#include <glm/glm.hpp>

namespace terraingen {
  namespace lod {
    struct lod_node {
      // bottom left
      lod_node* bl;

      // bottom right
      lod_node* br;

      // top left
      lod_node* tl;

      // top right
      lod_node* tr;

      // allocate new lod node
      static lod_node* lod_node_alloc();

      // free allocated lod node + children
      static void lod_node_free(lod_node* node);

      // helper functions

      /**
       * @brief fetches the size of a chunk, based on a sample point.
       * 
       * @param node - root node
       * @param tree_res - resolution of input tree.
       * @param sample_point - point on the tree we wish to sample. -> +, ^ + - from bottom left
       * @return int - size of specified chunk, or -1 if invalid
       */
      static size_t GetChunkSize(const lod_node* node, size_t tree_res, const glm::vec2& sample_point);
    };
  }
}

#endif // LOD_NODE_H_