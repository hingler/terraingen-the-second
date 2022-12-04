// todo: test vertex generation :3

#include <gtest/gtest.h>

#include "terrain/VertexGenerator.hpp"
#include "lod/lod_node.hpp"

using namespace terraingen;
using namespace terrain;
using namespace lod;
// sample terrain

struct DummySampler {
  float Get(int x, int y) { return 2.5f; }
};

TEST(VertexGeneratorTest, FeasibilityTest) {
  std::shared_ptr<DummySampler> heightmap = std::make_shared<DummySampler>();
  VertexGenerator gen(heightmap, 2.0f, (1.0 / 128.0), 64, 128, glm::vec3(0, 0, 0));
  lod_node* node = lod_node::lod_node_alloc();
  node->bl = lod_node::lod_node_alloc();
  node->br = lod_node::lod_node_alloc();
  node->tl = lod_node::lod_node_alloc();
  node->tr = lod_node::lod_node_alloc();
  
  Vertex test = gen.CreateVertex(60, 60, 2, node);

  ASSERT_NEAR(test.position.x, 120.0f, 0.001f);
  ASSERT_NEAR(test.position.y, 2.5f, 0.001f);
  ASSERT_NEAR(test.position.z, 120.0f, 0.001f);

  ASSERT_NEAR(test.normal.x, 0.0f, 0.001f);
  ASSERT_NEAR(test.normal.y, 1.0f, 0.001f);
  ASSERT_NEAR(test.normal.z, 0.0f, 0.001f);

  ASSERT_NEAR(test.texcoord.x, (60.0 / 128.0), 0.0001f);
  ASSERT_NEAR(test.texcoord.y, (60.0 / 128.0), 0.0001f);

  ASSERT_NEAR(test.tangent.x, 1.0f, 0.0001f);
  ASSERT_NEAR(test.tangent.y, 0.0f, 0.0001f);
  ASSERT_NEAR(test.tangent.z, 0.0f, 0.0001f);

  lod_node::lod_node_free(node);
}

TEST(VertexGeneratorTest, SimpleCornerCaseTest) {
  std::shared_ptr<DummySampler> heightmap = std::make_shared<DummySampler>();
  VertexGenerator gen(heightmap, 2.0f, (1.0 / 128.0), 64, 128, glm::vec3(0, 0, 0));
  lod_node* node = lod_node::lod_node_alloc();
  node->bl = lod_node::lod_node_alloc();
  node->br = lod_node::lod_node_alloc();
  node->tl = lod_node::lod_node_alloc();
  node->tr = lod_node::lod_node_alloc();
  
  Vertex test = gen.CreateVertex(64, 64, 1, node);

  ASSERT_NEAR(test.position.x, 128.0f, 0.001f);
  ASSERT_NEAR(test.position.y, 2.5f, 0.001f);
  ASSERT_NEAR(test.position.z, 128.0f, 0.001f);

  ASSERT_NEAR(test.normal.x, 0.0f, 0.001f);
  ASSERT_NEAR(test.normal.y, 1.0f, 0.001f);
  ASSERT_NEAR(test.normal.z, 0.0f, 0.001f);

  ASSERT_NEAR(test.texcoord.x, (64.0 / 128.0), 0.0001f);
  ASSERT_NEAR(test.texcoord.y, (64.0 / 128.0), 0.0001f);

  ASSERT_NEAR(test.tangent.x, 1.0f, 0.0001f);
  ASSERT_NEAR(test.tangent.y, 0.0f, 0.0001f);
  ASSERT_NEAR(test.tangent.z, 0.0f, 0.0001f);

  test = gen.CreateVertex(128, 128, 1, node);

  ASSERT_NEAR(test.position.x, 256.0f, 0.001f);
  ASSERT_NEAR(test.position.y, 2.5f, 0.001f);
  ASSERT_NEAR(test.position.z, 256.0f, 0.001f);

  ASSERT_NEAR(test.normal.x, 0.0f, 0.001f);
  ASSERT_NEAR(test.normal.y, 1.0f, 0.001f);
  ASSERT_NEAR(test.normal.z, 0.0f, 0.001f);

  ASSERT_NEAR(test.texcoord.x, (128.0 / 128.0), 0.0001f);
  ASSERT_NEAR(test.texcoord.y, (128.0 / 128.0), 0.0001f);

  ASSERT_NEAR(test.tangent.x, 1.0f, 0.0001f);
  ASSERT_NEAR(test.tangent.y, 0.0f, 0.0001f);
  ASSERT_NEAR(test.tangent.z, 0.0f, 0.0001f);

  lod_node::lod_node_free(node);
}

struct DummySample_EdgeCase {
  float Get(int x, int y) {
    return static_cast<float>(x % 2);
  }
};

TEST(VertexGeneratorTest, SimpleEdgeCaseTest) {
  std::shared_ptr<DummySample_EdgeCase> heightmap = std::make_shared<DummySample_EdgeCase>();
  VertexGenerator gen(heightmap, 2.0f, (1.0 / 128.0), 32, 128, glm::vec3(0, 0, 0));
  lod_node* node = lod_node::lod_node_alloc();
  node->bl = lod_node::lod_node_alloc();
  node->br = lod_node::lod_node_alloc();
  node->tl = lod_node::lod_node_alloc();
  node->tr = lod_node::lod_node_alloc();

  // subdivide bottom right
  lod_node* child = node->br;
  child->bl = lod_node::lod_node_alloc();
  child->br = lod_node::lod_node_alloc();
  child->tl = lod_node::lod_node_alloc();
  child->tr = lod_node::lod_node_alloc();

  // we're on a corner case here
  Vertex test = gen.CreateVertex(72, 64, 1, node);
  // left is 2
  // right is 2
  // up is 2
  // down is 1
  // should all sample 0
  ASSERT_NEAR(test.position.x, 144.0f, 0.001f);
  ASSERT_NEAR(test.position.y, 0.0f, 0.001f);
  ASSERT_NEAR(test.position.z, 128.0f, 0.001f);

  ASSERT_NEAR(test.normal.x, 0.0f, 0.001f);
  ASSERT_NEAR(test.normal.y, 1.0f, 0.001f);
  ASSERT_NEAR(test.normal.z, 0.0f, 0.001f);

  ASSERT_NEAR(test.texcoord.x, (72.0 / 128.0), 0.0001f);
  ASSERT_NEAR(test.texcoord.y, (64.0 / 128.0), 0.0001f);

  ASSERT_NEAR(test.tangent.x, 1.0f, 0.0001f);
  ASSERT_NEAR(test.tangent.y, 0.0f, 0.0001f);
  ASSERT_NEAR(test.tangent.z, 0.0f, 0.0001f);

  test = gen.CreateVertex(73, 64, 1, node);
  // this would naively sample with a height of 1...
  // but because we're interpolating along edge it should be consistent with lower detail level
  ASSERT_NEAR(test.position.x, 146.0f, 0.001f);
  ASSERT_NEAR(test.position.y, 0.0f, 0.001f);
  ASSERT_NEAR(test.position.z, 128.0f, 0.001f);

  ASSERT_NEAR(test.normal.x, 0.0f, 0.001f);
  ASSERT_NEAR(test.normal.y, 1.0f, 0.001f);
  ASSERT_NEAR(test.normal.z, 0.0f, 0.001f);

  ASSERT_NEAR(test.texcoord.x, (73.0 / 128.0), 0.0001f);
  ASSERT_NEAR(test.texcoord.y, (64.0 / 128.0), 0.0001f);

  ASSERT_NEAR(test.tangent.x, 1.0f, 0.0001f);
  ASSERT_NEAR(test.tangent.y, 0.0f, 0.0001f);
  ASSERT_NEAR(test.tangent.z, 0.0f, 0.0001f);
}