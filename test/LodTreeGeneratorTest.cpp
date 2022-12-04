// todo: write test :3
#include <gtest/gtest.h>

#include "lod/LodTreeGenerator.hpp"
#include "util/HashList.hpp"

using namespace terraingen;
using namespace lod;

class HeightMapTest {
public:
  float Get(int x, int y) { return 2.0f; }
};

void lod_node_recurse_verify(lod_node* node) {
  if (node->tl != nullptr) {
    ASSERT_NE(node->tr, nullptr);
    ASSERT_NE(node->bl, nullptr);
    ASSERT_NE(node->br, nullptr);
    lod_node_recurse_verify(node->tl);
    lod_node_recurse_verify(node->tr);
    lod_node_recurse_verify(node->bl);
    lod_node_recurse_verify(node->br);
  } else {
    ASSERT_EQ(node->tr, nullptr);
    ASSERT_EQ(node->bl, nullptr);
    ASSERT_EQ(node->br, nullptr);
  }
}

TEST(LodTreeGeneratorTest, EnsureConsistency) {
  std::shared_ptr<HeightMapTest> test = std::make_shared<HeightMapTest>();
  LodTreeGenerator<HeightMapTest> generator(test, 256, 16);
  generator.cascade_factor = 16.0f;
  glm::vec3 local_point(40.0, 40.0, 2.5);

  lod_node* node = generator.CreateLodTree(local_point);
  lod_node_recurse_verify(node);
  lod_node::lod_node_free(node);
}

TEST(LodTreeGeneratorTest, VerifyMinChunkSizeAtLocalPoint) {
  std::shared_ptr<HeightMapTest> test = std::make_shared<HeightMapTest>();
  LodTreeGenerator<HeightMapTest> generator(test, 256, 16);
  generator.cascade_factor = 32.0f;
  glm::vec3 local_point(40.0, 40.0, 2.5);

  lod_node* node = generator.CreateLodTree(local_point);

  int chunk_size_local = lod_node::GetChunkSize(node, 256, glm::vec2(40.0, 40.0));
  ASSERT_EQ(chunk_size_local, 16);

  lod_node::lod_node_free(node);
}