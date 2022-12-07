#ifndef CHUNK_IDENTIFIER_H_
#define CHUNK_IDENTIFIER_H_

#include <functional>

namespace terraingen {
  namespace terrain {
    struct ChunkIdentifier {
      long x;
      long y;
      size_t size;

      bool operator==(const ChunkIdentifier& rhs) const {
        return (rhs.x == x && rhs.y == y && rhs.size == size);
      }
    };
  }
}

namespace std {
  template<>
  struct hash<terraingen::terrain::ChunkIdentifier> {
    size_t operator()(const terraingen::terrain::ChunkIdentifier& identifier) const {
      // this is fine for now i think
      return ((identifier.x << 24) | identifier.y) * identifier.size;
    }

    // need equality check :3
  };
}

#endif // CHUNK_IDENTIFIER_H_