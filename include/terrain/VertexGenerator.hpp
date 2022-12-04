#ifndef VERTEX_GENERATOR_H_
#define VERTEX_GENERATOR_H_

#include "terrain/Vertex.hpp"
#include "traits/height_map.hpp"

#include "lod/lod_node.hpp"

#include <algorithm>
#include <memory>

#include <iostream>

#include <glm/glm.hpp>

namespace terraingen {
  namespace terrain {
    namespace impl {
      
    }

    template <typename HeightMap>
    class VertexGenerator {
      static_assert(traits::height_map<HeightMap>::value);
    public:
      /**
       * @brief Construct a new Vertex Generator object
       * 
       * @param height - height map
       * @param horizontal_scale - ratio of world space : height space (ie how far apart samples are, as vertices)
       * @param texcoord_scale - ratio of world space : texcoord space (ie how far apart samples are, in tex space)
       */
      VertexGenerator(
        std::shared_ptr<HeightMap> height,
        float horizontal_scale,
        double texcoord_scale,
        size_t chunk_resolution,
        size_t tree_resolution,
        const glm::vec3& terrain_offset)
        : height_map(height),
          scale(horizontal_scale),
          scale_tex(texcoord_scale),
          offset(terrain_offset),
          chunk_res(chunk_resolution),
          tree_res(tree_resolution) {}

      Vertex CreateVertex(
        size_t offset_x,
        size_t offset_y,
        size_t step,
        const lod::lod_node* tree
      ) {
        size_t chunk_size = chunk_res * step;
        if (offset_x % chunk_size == 0 || offset_y % chunk_size == 0) {
          // have to poll chunk axis
          glm::vec2 axis_sample(offset_x, offset_y);
          // sampling on an axis fucks up corner case
          // just sample on diagonal and the logic should still apply
          axis_sample -= glm::vec2(0.5);

          size_t target_size = lod::lod_node::GetChunkSize(tree, tree_res, axis_sample);
          size_t effective_step = target_size / chunk_res;

          axis_sample += glm::vec2(1.0);
          target_size = lod::lod_node::GetChunkSize(tree, tree_res, axis_sample);
          effective_step = std::max(effective_step, target_size / chunk_res);

          if (offset_x % effective_step != 0 || offset_y % effective_step != 0) {
            // interpolate between nearest reference points
            return CreateVertex_edge(offset_x, offset_y, effective_step);
          } else {
            // sample along both axes
            return CreateVertex_corner(offset_x, offset_y, tree);
          }
        }

        return CreateVertex_internal(offset_x, offset_y, step);
      }

    private:
      /**
       * @brief Create a Vertex object for the specified location
       * 
       * @param offset_x - x offset for sample
       * @param offset_y - y offset for sample
       * @param step - number of vertices to step when generating normals, tangents, etc...
       * @return Vertex - resultant vertex object.
       */
      Vertex CreateVertex_internal(
        size_t offset_x,
        size_t offset_y,
        size_t step
      ) {
        auto position = GetPosition(offset_x, offset_y);
        auto coord_l = GetPosition(offset_x - step, offset_y);
        auto coord_r = GetPosition(offset_x + step, offset_y);
        auto coord_d = GetPosition(offset_x, offset_y - step);
        auto coord_u = GetPosition(offset_x, offset_y + step);
        
        auto tangent = glm::normalize(coord_r - coord_l);
        auto bitangent = glm::normalize(coord_u - coord_d);
        auto normal = glm::cross(bitangent, tangent);

        auto texcoord = glm::vec2(offset_x * scale_tex, offset_y * scale_tex);

        return { position, normal, texcoord, tangent };
      }

      Vertex CreateVertex_edge(
        size_t offset_x,
        size_t offset_y,
        size_t effective_step
      ) {
        
        
        // we're not on a corner...
        // so the edge axis is the coord st (offset % step) == 0
        glm::ivec2 sample_point(offset_x, offset_y);
        int axis = (offset_x % effective_step == 0 ? 1 : 0);

        float mix = static_cast<float>(sample_point[axis] % effective_step) / effective_step;
        sample_point[axis] -= sample_point[axis] % effective_step;

        // lots of seemingly wasted samples for corners and edges :(
        Vertex vert_floor = CreateVertex_internal(sample_point.x, sample_point.y, effective_step);

        sample_point[axis] += effective_step;
        Vertex vert_ceil = CreateVertex_internal(sample_point.x, sample_point.y, effective_step);

        Vertex res;
        res.position = vert_floor.position * (1.0f - mix) + vert_ceil.position * mix;
        res.normal = vert_floor.normal * (1.0f - mix) + vert_ceil.normal * mix;
        res.texcoord = vert_floor.texcoord * (1.0f - mix) + vert_ceil.texcoord * mix;
        res.tangent = vert_floor.tangent * (1.0f - mix) + vert_ceil.tangent * mix;

        return res;
      }

      Vertex CreateVertex_corner(
        size_t offset_x,
        size_t offset_y,
        const lod::lod_node* node
      ) {
        // find our nearest control points via GetChunkSize
        glm::vec2 test_point(offset_x, offset_y);
        size_t step_bl = lod::lod_node::GetChunkSize(node, tree_res, test_point + glm::vec2(-0.5, -0.5)) / chunk_res;
        size_t step_br = lod::lod_node::GetChunkSize(node, tree_res, test_point + glm::vec2(0.5, -0.5)) / chunk_res;
        size_t step_tl = lod::lod_node::GetChunkSize(node, tree_res, test_point + glm::vec2(-0.5, 0.5)) / chunk_res;
        size_t step_tr = lod::lod_node::GetChunkSize(node, tree_res, test_point + glm::vec2(0.5, 0.5)) / chunk_res;
        
        size_t left_step = std::max(step_bl, step_tl);
        size_t right_step = std::max(step_br, step_tr);
        size_t top_step = std::max(step_tl, step_tr);
        size_t bot_step = std::max(step_bl, step_br);

        // not tri area whatever
        float tri_tl = left_step * top_step;
        float tri_tr = right_step * top_step;
        float tri_bl = left_step * bot_step;
        float tri_br = right_step * bot_step;

        float quad_area = tri_tl + tri_tr + tri_bl + tri_br;
        std::cout << "quad area: " << quad_area << std::endl;

        float left_bias = (tri_tl + tri_bl) / (2 * quad_area);
        float right_bias = (tri_tr + tri_br) / (2 * quad_area);
        float top_bias = (tri_tl + tri_tr) / (2 * quad_area);
        float bot_bias = (tri_bl + tri_br) / (2 * quad_area);

        // fetch our actual edge vertices, using steps calculated above
        // blend normals, tangents according to above calculati
        Vertex left_vert = CreateVertex_internal(offset_x - left_step, offset_y, left_step);
        Vertex right_vert = CreateVertex_internal(offset_x + right_step, offset_y, right_step);
        Vertex top_vert = CreateVertex_internal(offset_x, offset_y + top_step, top_step);
        Vertex bot_vert = CreateVertex_internal(offset_x, offset_y - bot_step, bot_step);

        Vertex res;
        // position + texcoord we can set trivially
        res.position = GetPosition(offset_x, offset_y);
        res.texcoord = glm::vec2(offset_x * scale_tex, offset_y * scale_tex);

        res.normal = left_vert.normal * left_bias
          + right_vert.normal * right_bias
          + top_vert.normal * top_bias
          + bot_vert.normal * bot_bias;

        res.normal = glm::normalize(res.normal);

        res.tangent = left_vert.tangent * left_bias
          + right_vert.tangent * right_bias
          + top_vert.tangent * top_bias
          + bot_vert.tangent * bot_bias;

        res.tangent = glm::normalize(res.tangent);
        return res;
      }

      std::shared_ptr<HeightMap> height_map;
      float scale;
      float scale_tex;
      glm::vec3 offset;

      size_t chunk_res;
      size_t tree_res;

      glm::vec3 GetPosition(size_t offset_x, size_t offset_y) {
        float sample = height_map->Get(offset_x, offset_y);
        glm::vec3 position(offset_x * scale, sample, offset_y * scale);
        position -= offset;
        return position;
      }

      // for corner cases: float sampling might be a necessity

      // compare 
      size_t GetStepSizeAtPoint(size_t tree_size, size_t chunk_size, size_t step_default, const lod::lod_node* tree, const glm::vec2& point) {
        size_t res = step_default;
        size_t point_chunk_size = lod::lod_node::GetChunkSize(tree, tree_size, point);
        while (point_chunk_size > chunk_size) {
          // point's chunk is larger
          // increase step size
          res <<= 1;
          point_chunk_size >>= 1;
        }

        while (point_chunk_size < chunk_size) {
          // point's chunk is smaller
          // decrease step size
          res >>= 1;
          point_chunk_size <<= 1;
        }

        return res;
      }
    };
  }
}

#endif // VERTEX_GENERATOR_H_