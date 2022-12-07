#include "lod/lod_node.hpp"

namespace terraingen {
  namespace lod {
    lod_node* lod_node::lod_node_alloc() {
      return new lod_node();
    }

    void lod_node::lod_node_free(lod_node* node) {
      if (node == nullptr) { return; }
      lod_node_free(node->bl);
      lod_node_free(node->br);
      lod_node_free(node->tl);
      lod_node_free(node->tr);
      delete node;
    }

    size_t lod_node::GetChunkSize(const lod_node* node, size_t tree_res, const glm::vec2& sample_point) {
      glm::vec2 sub_sample_point = sample_point;

      if (node == nullptr) {
        return tree_res * 2;
      }

      const lod_node** node_ptr = reinterpret_cast<const lod_node**>(const_cast<lod_node*>(node));
      if (sample_point.y > tree_res) {
        node_ptr += 2;
        sub_sample_point.y -= tree_res;
      }

      if (sample_point.x > tree_res) {
        node_ptr += 1;
        sub_sample_point.x -= tree_res;
      }

      return GetChunkSize(*node_ptr, tree_res / 2, sub_sample_point);
    }
  }
}