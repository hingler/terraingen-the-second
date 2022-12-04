#ifndef CHUNK_IDENTIFIER_H_
#define CHUNK_IDENTIFIER_H_

#include <functional>

namespace terraingen {
  namespace terrain {
    struct ChunkIdentifier {
      size_t x;
      size_t y;
      size_t size;

      bool operator==(const ChunkIdentifier& rhs) {
        return (rhs.x == x && rhs.y == y && rhs.size == size);
      }
    };

    bool operator==(const ChunkIdentifier& lhs, const ChunkIdentifier& rhs) {
      return lhs == rhs;
    }
  }
}

namespace std {
  template<>
  struct hash<terraingen::terrain::ChunkIdentifier> {
    size_t operator()(const terraingen::terrain::ChunkIdentifier& identifier) {
      // this is fine for now i think
      return ((identifier.x << 24) | identifier.y) * identifier.size;
    }

    // need equality check :3
  };
}

#endif // CHUNK_IDENTIFIER_H_