#ifndef VERTEX_H_
#define VERTEX_H_

#include <glm/glm.hpp>

namespace terraingen {
  namespace terrain {
    struct Vertex {
      glm::vec3 position;
      glm::vec3 normal;
      glm::vec2 texcoord;
      glm::vec4 tangent;
    } __attribute__((aligned(4)));
  }
}

#endif // VERTEX_H_