// terrain generator test
// run tests with terrain gen, wander around a bit and see what we get

#include <gtest/gtest.h>

#include "TerrainGenerator.hpp"

using namespace terraingen;

struct DummySampler {
  float Get(float x, float y) { return sin(0.5 * x + 0.4 * y); }
};

TEST(TerrainGeneratorTest, DumbTest) {
  std::shared_ptr<DummySampler> sampler = std::make_shared<DummySampler>();
  TerrainGenerator generator(
    sampler,
    4.0f,
    (1.0 / 2048.0),
    glm::vec3(0.0),
    2048,
    64,
    4.0
  );

  // issue: lod runs in terrain space
  generator.UpdateChunkData(glm::vec3(128.0, 0.0, 128.0));

  size_t chunk_count = generator.GetChunkCount();
  EXPECT_GT(chunk_count, 1);
  EXPECT_LT(chunk_count, 512);
  EXPECT_EQ(generator.GetVertexBufferSize(), chunk_count * 65 * 65 * sizeof(terrain::Vertex));
  EXPECT_EQ(generator.GetIndexBufferSize(), chunk_count * 64 * 64 * 6 * sizeof(unsigned int));
}

TEST(TerrainGeneratorTest, WanderingTest) {
  std::shared_ptr<DummySampler> sampler = std::make_shared<DummySampler>();
  TerrainGenerator generator(
    sampler,
    4.0f,
    (1.0 / 2048.0),
    glm::vec3(0.0),
    2048,
    64,
    4.0
  );

  for (float theta = 0.0f; theta < M_PI * 2; theta += 0.1f) {
    
  }
}