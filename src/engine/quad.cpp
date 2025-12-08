module;

#include <algorithm>
#include <array>
#include <cstddef>
#include <glm/common.hpp>
#include <glm/ext/matrix_float2x2.hpp>
#include <glm/ext/vector_float2.hpp>
#include <print>
#include <ranges>
#include <tuple>

export module moonstone:quad;

import :vertex_element;
import :vertex_buffer;

export namespace moonstone::engine
{
class quad
{
	glm::vec2 m_size;
	glm::vec2 m_anchor;
	glm::vec2 m_position;
	std::uint32_t m_texture;
	std::array<glm::vec2, 4> m_quad_vertices{};
	static constexpr std::array<glm::vec2, 4> m_quad_uvs{
		glm::vec2{0.0F, 1.0F}, glm::vec2{0.0F, 0.0F}, glm::vec2{1.0F, 0.0F},
		glm::vec2{1.0F, 1.0F}};
	std::array<std::uint32_t, 4> m_buffer_vertices{0U, 0U, 0U, 0U};
	renderer::vertex_buffer& m_vbo;

	void update_quad_vertices()
	{
		glm::vec2 i_anchor = glm::vec2(1.0F, 1.0F) - this->m_anchor;
		this->m_quad_vertices[0] = {-this->m_size.x * m_anchor.x,
									this->m_size.y * i_anchor.y};
		this->m_quad_vertices[1] = {-this->m_size.x * m_anchor.x,
									-this->m_size.y * m_anchor.y};
		this->m_quad_vertices[2] = {this->m_size.x * i_anchor.x,
									-this->m_size.y * m_anchor.y};
		this->m_quad_vertices[3] = {this->m_size.x * i_anchor.x,
									this->m_size.y * i_anchor.y};
		std::ranges::for_each(this->m_quad_vertices, [this](auto& it) {
			// std::println("Vertex update!");
			it += this->m_position;
		});
	}

public:
	quad(glm::vec2 position, glm::vec2 size, glm::vec2 anchor,
		 std::uint32_t texture, renderer::vertex_buffer& vbo)
		: m_position(position), m_size(size),
		  m_anchor(glm::clamp(anchor, 0.0F, 1.0F)), m_texture(texture),
		  m_vbo(vbo)
	{
		this->update_quad_vertices();
		// Generates the vertex buffer vertices
		for (auto it : std::views::zip(this->m_buffer_vertices,
									   this->m_quad_vertices, quad::m_quad_uvs))
		{
			auto& [buffer_vertex, quad_vertex, quad_uv] = it;
			buffer_vertex = this->m_vbo.insert({quad_vertex, quad_uv, 0});
		}
	}

	void set_position(glm::vec2 position)
	{
		this->m_position = position;
		this->update();
	}

	void update()
	{
		this->update_quad_vertices();
		std::ranges::for_each(
			this->m_quad_vertices, [this, idx = 0](auto& it) mutable {
				this->m_vbo.update(this->m_buffer_vertices.at(idx),
								   {it, quad::m_quad_uvs.at(idx), 0});
				++idx;
			});
	}
	quad(const quad&) = default;
	quad(quad&&) = default;
	quad& operator=(const quad&) = delete;
	quad& operator=(quad&&) = delete;
};
} // namespace moonstone::engine
