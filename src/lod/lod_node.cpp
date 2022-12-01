#include "lod/lod_node.hpp"

namespace terraingen {
  namespace lod {
    lod_node* lod_node_alloc() {
      return new lod_node();
    }

    void lod_node_free(lod_node* node) {
      if (node == nullptr) { return; }
      lod_node_free(node->tl);
      lod_node_free(node->tr);
      lod_node_free(node->bl);
      lod_node_free(node->br);
    }
  }
}