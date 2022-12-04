#include "terrain/Chunk.hpp"

namespace terraingen {
  namespace terrain {
    Chunk::~Chunk() {
      if (vertex_data != nullptr) {
        delete[] vertex_data;
      }

      if (index_data != nullptr) {
        delete[] index_data;
      }
    }

    Chunk::Chunk(Chunk&& other) {
      vertex_data = other.vertex_data;
      index_data = other.index_data;

      other.vertex_data = nullptr;
      other.index_data = nullptr;
    }

    Chunk& Chunk::operator=(Chunk&& other) {
      if (vertex_data != nullptr) {
        delete[] vertex_data;
      }

      if (index_data != nullptr) {
        delete[] index_data;
      }

      vertex_data = other.vertex_data;
      index_data = other.index_data;

      other.vertex_data = nullptr;
      other.index_data = nullptr;

      return *this;
    }
  }
}