#include <gtest/gtest.h>

#include <cmath>
#include <iostream>

#include "terrain/ChunkGenerator.hpp"

struct DumbSampler {
  float Get(double x, double y) {
    return sin(0.125 * x);
  }
};

using namespace terraingen;
using namespace terrain;

using namespace lod;

TEST(ChunkGeneratorTest, SimpleChunkGen) {
  std::shared_ptr<DumbSampler> sampler = std::make_shared<DumbSampler>();
  ChunkGenerator<DumbSampler> generator(sampler, 1.0, (1.0 / 128.0), glm::vec3(0), 32);

  lod::lod_node* node = lod_node::lod_node_alloc();
  node->tl = lod_node::lod_node_alloc();
  node->tr = lod_node::lod_node_alloc();
  node->bl = lod_node::lod_node_alloc();
  node->br = lod_node::lod_node_alloc();

  generator.UpdateChunks(node, 128);

  ASSERT_EQ(generator.GetChunkCount(), 4);

  ASSERT_EQ(generator.GetVertexBufferSize(), 33 * 33 * 4 * sizeof(Vertex));
  ASSERT_EQ(generator.GetIndexBufferSize(), 32 * 32 * 4 * 6 * sizeof(unsigned int));

  lod_node::lod_node_free(node);

  Vertex* vertex_buffer = new Vertex[33 * 33 * 4];
  unsigned int* index_buffer = new unsigned int[32 * 32 * 4 * 6];

  ASSERT_EQ(generator.WriteVertexBuffer(vertex_buffer, 33 * 33 * 4 * 44), 33 * 33 * 4 * 44);
  ASSERT_EQ(generator.WriteIndexBuffer(index_buffer, 32 * 32 * 6 * 4 * 4), 32 * 32 * 6 * 4 * 4);

  for (int i = 0; i < 33 * 3 * 4; i++) {
    Vertex& vert = vertex_buffer[i];
    float height = sampler->Get(vert.position.x, vert.position.z);
    EXPECT_NEAR(vert.position.y, height, 0.0001);

    EXPECT_NEAR(vert.texcoord.x, vert.position.x * (1.0 / 128.0), 0.0001);

    EXPECT_NEAR(vert.texcoord.y, vert.position.z * (1.0 / 128.0), 0.0001);
    EXPECT_GT(vert.normal.y, 0.1);
  }

  for (int i = 0; i < 32 * 32 * 6 * 4; i++) {
    unsigned int& index = index_buffer[i];
    EXPECT_LT(index, 33 * 33 * 4);
  }

  delete[] vertex_buffer;
  delete[] index_buffer;
}

TEST(ChunkGeneratorTest, MultipleLod) {
  std::shared_ptr<DumbSampler> sampler = std::make_shared<DumbSampler>();
  ChunkGenerator<DumbSampler> generator(sampler, 1.0, (1.0 / 128.0), glm::vec3(0), 32);

  lod::lod_node* node = lod_node::lod_node_alloc();
  node->tl = lod_node::lod_node_alloc();
  node->tr = lod_node::lod_node_alloc();
  node->bl = lod_node::lod_node_alloc();
  node->br = lod_node::lod_node_alloc();
  auto* child_node = node->br;

  child_node->tl = lod_node::lod_node_alloc();
  child_node->tr = lod_node::lod_node_alloc();
  child_node->bl = lod_node::lod_node_alloc();
  child_node->br = lod_node::lod_node_alloc();

  
  generator.UpdateChunks(node, 128);

  ASSERT_EQ(generator.GetChunkCount(), 7);

  ASSERT_EQ(generator.GetVertexBufferSize(), 33 * 33 * 7 * sizeof(Vertex));
  ASSERT_EQ(generator.GetIndexBufferSize(), 32 * 32 * 7 * 6 * sizeof(unsigned int));

  lod_node::lod_node_free(node);

  Vertex* vertex_buffer = new Vertex[33 * 33 * 7];
  unsigned int* index_buffer = new unsigned int[32 * 32 * 7 * 6];

  ASSERT_EQ(generator.WriteVertexBuffer(vertex_buffer, 33 * 33 * 7 * sizeof(Vertex)), 33 * 33 * 7 * 44);
  ASSERT_EQ(generator.WriteIndexBuffer(index_buffer, 32 * 32 * 6 * 7 * sizeof(unsigned int)), 32 * 32 * 6 * 7 * 4);
  for (int i = 0; i < 33 * 33 * 7; i++) {
    Vertex& vert = vertex_buffer[i];
    float height = sampler->Get(vert.position.x, vert.position.z);
    // heights are probably off on edges
    // could test later!!
    EXPECT_NEAR(vert.position.y, height, 0.01);

    EXPECT_NEAR(vert.texcoord.x, vert.position.x * (1.0 / 128.0), 0.005);

    EXPECT_NEAR(vert.texcoord.y, vert.position.z * (1.0 / 128.0), 0.005);

    EXPECT_GT(vert.normal.y, 0.1);
  }

  for (int i = 0; i < 32 * 32 * 6 * 7; i++) {
    unsigned int& index = index_buffer[i];
    EXPECT_LT(index, 33 * 33 * 7);
  }

  delete[] vertex_buffer;
  delete[] index_buffer;
}