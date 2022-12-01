#ifndef LOD_NODE_H_
#define LOD_NODE_H_

namespace terraingen {
  namespace lod {
    struct lod_node {
      // top left
      lod_node* tl;

      // top right
      lod_node* tr;

      // bottom left
      lod_node* bl;

      // bottom right
      lod_node* br;

      // allocate new lod node
      static lod_node* lod_node_alloc();

      // free allocated lod node + children
      static void lod_node_free(lod_node* node);
    };
  }
}

#endif // LOD_NODE_H_