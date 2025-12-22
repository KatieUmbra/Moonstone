module;

#include "Try.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <glm/glm.hpp>
#include <ranges>
#include <stdexcept>

export module moonstone:quad;

import :vertex_element;
import :vertex_buffer;
import :index_buffer;
import :error;
import :sync_buffer_connection;
import :sync_buffer;

export namespace moonstone::engine
{
using quad_handler = renderer::buffer_connection<
	renderer::synchronized_buffer<renderer::vertex_element, 4UL>,
	renderer::vertex_element, 4UL>;
class quad
{
	glm::vec2 m_size;
	glm::vec2 m_anchor;
	glm::vec2 m_position;
	std::uint32_t m_texture;
	std::array<glm::vec2, 4> m_quad_vertices{};
	std::array<std::size_t, 6> m_quad_indices{};
	static constexpr std::array<glm::vec2, 4> m_quad_uvs{
		glm::vec2{0.0F, 1.0F}, glm::vec2{0.0F, 0.0F}, glm::vec2{1.0F, 0.0F},
		glm::vec2{1.0F, 1.0F}};
	std::array<std::uint32_t, 4> m_buffer_vertices{0U, 0U, 0U, 0U};
	renderer::index_buffer& m_ibo;
	quad_handler m_vbo_connection;

	void update_quad_vertices()
	{
		const glm::vec2 i_anchor = glm::vec2(1.0F, 1.0F) - this->m_anchor;
		this->m_quad_vertices[0] = {-this->m_size.x * m_anchor.x,
									this->m_size.y * i_anchor.y};
		this->m_quad_vertices[1] = {-this->m_size.x * m_anchor.x,
									-this->m_size.y * m_anchor.y};
		this->m_quad_vertices[2] = {this->m_size.x * i_anchor.x,
									-this->m_size.y * m_anchor.y};
		this->m_quad_vertices[3] = {this->m_size.x * i_anchor.x,
									this->m_size.y * i_anchor.y};
		std::ranges::for_each(this->m_quad_vertices,
							  [this](auto& it) { it += this->m_position; });
	}

	error::result<> create()
	{
		std::array<renderer::vertex_element, 4UL> temp_buffer{};
		this->update_quad_vertices();
		// Generates the vertex buffer vertices
		for (auto it : std::views::zip(temp_buffer, this->m_quad_vertices,
									   quad::m_quad_uvs))
		{
			auto& [buffer_vertex, quad_vertex, quad_uv] = it;
			buffer_vertex = {quad_vertex, quad_uv, 0};
		}
		this->m_vbo_connection.update(temp_buffer);
		auto buffer_size = this->m_ibo.get_highest();
		auto current_index = (buffer_size < 1) ? 0 : buffer_size + 1;
		const std::array<std::size_t, 6> indices{
			current_index,	   current_index + 1, current_index + 2,
			current_index + 2, current_index + 3, current_index};
		this->m_quad_indices = Try(this->m_ibo.insert<6UL>(indices));
		return {};
	}

public:
	quad(glm::vec2 position, glm::vec2 size, glm::vec2 anchor,
		 std::uint32_t texture, quad_handler&& vbo_handler,
		 renderer::index_buffer& ibo)
		: m_position(position), m_size(size),
		  m_anchor(glm::clamp(anchor, 0.0F, 1.0F)), m_texture(texture),
		  m_vbo_connection(std::move(vbo_handler)), m_ibo(ibo)
	{
		auto err = this->create();
		if (!err.has_value())
		{
			throw std::runtime_error(err.error().format());
		}
	}

	~quad() = default;

	void set_position(glm::vec2 position)
	{
		this->m_position = position;
		this->update();
	}

	void update()
	{
		this->update_quad_vertices();
		std::array<renderer::vertex_element, 4UL> temp_buffer{};
		for (auto it : std::views::zip(temp_buffer, this->m_quad_vertices,
									   quad::m_quad_uvs))
		{
			auto& [buffer_vertex, quad_vertex, quad_uv] = it;
			buffer_vertex = {quad_vertex, quad_uv, 0};
		}
		this->m_vbo_connection.update(temp_buffer);
	}
	quad(const quad&) = default;
	quad(quad&&) = default;
	quad& operator=(const quad&) = delete;
	quad& operator=(quad&&) = delete;
};
} // namespace moonstone::engine
