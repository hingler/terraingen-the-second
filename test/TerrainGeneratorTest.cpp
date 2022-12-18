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
    256.0
  );

  // x62
  for (float theta = 0.0f; theta < M_PI * 2; theta += 0.1f) {
    // x5
    for (float radius = 63.0; radius < 1024.0; radius *= 2) {
      glm::vec3 point(cos(theta) * 960.0 + radius, 0.5, sin(theta) * radius + 1024.0);
      generator.UpdateChunkData(point);

      size_t chunk_count = generator.GetChunkCount();
      EXPECT_GT(chunk_count, 1);
      EXPECT_LT(chunk_count, 512);
      EXPECT_EQ(generator.GetVertexBufferSize(), chunk_count * 65 * 65 * sizeof(terrain::Vertex));
      EXPECT_EQ(generator.GetIndexBufferSize(), chunk_count * 64 * 64 * 6 * sizeof(unsigned int));
    }
  }

  // x310 chunk generation
  // 0.45sec -> < 2ms for chunk gen? (hundreds of chunks -> 4096 * 200 -> 800k vertices + index data)
  // - we can save on index gen by caching results - the indices literally do not ever change

  // this might be fine actually - could regenerate terrain on each frame (or delay, come up with alt solution)
}

TEST(TerrainGeneratorTest, RespectOffsetInTreeGen) {
  std::shared_ptr<DummySampler> sampler = std::make_shared<DummySampler>();
  TerrainGenerator generator(
    sampler,
    4.0f,
    (1.0 / 2048.0),
    glm::vec3(-1024.0, 0.0, -1024.0),
    2048,
    64,
    256.0
  );

  generator.UpdateChunkData(glm::vec3(0));
  size_t chunks = generator.GetChunkCount();

  generator.UpdateChunkData(glm::vec3(1024, 0, 1024));
  size_t chunks_border = generator.GetChunkCount();

  // symmetrical about center
  generator.UpdateChunkData(glm::vec3(-1024, 0, -1024));
  size_t chunks_opposite_border = generator.GetChunkCount();

  ASSERT_EQ(chunks_border, chunks_opposite_border);
  ASSERT_GT(chunks, chunks_border);
}