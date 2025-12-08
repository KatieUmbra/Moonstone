module;

#include "Assert.hpp"
#include <algorithm>
#include <cstdint>
#include <flat_map>
#include <glad/glad.h>
#include <initializer_list>
#include <print>
#include <stdexcept>
#include <vector>

export module moonstone:vertex_buffer;

import :vertex_element;

export namespace moonstone::renderer
{
class vertex_buffer
{
	std::uint32_t m_renderer_id{};
	std::uint32_t m_vertex_count{0};
	std::flat_map<std::uint32_t, vertex_element> m_vertices;

public:
	vertex_buffer()
	{
		GL_CALL(glGenBuffers(1, &this->m_renderer_id));
		this->bind();
		const void* data =
			reinterpret_cast<const void*>(this->m_vertices.values().data());
		GL_CALL(glBufferData(GL_ARRAY_BUFFER,
							 this->m_vertices.size() * sizeof(vertex_element),
							 data, GL_STATIC_DRAW));
	};
	~vertex_buffer()
	{
		GL_CALL(glDeleteBuffers(1, &this->m_renderer_id));
	};
	std::uint32_t insert(vertex_element element)
	{
		this->m_vertices.insert({this->m_vertex_count, element});
		return this->m_vertex_count++;
	}
	void update(std::uint32_t element_id, vertex_element element)
	{
		try
		{
			vertex_element& element_ref = this->m_vertices.at(element_id);
			element_ref = element;
		}
		catch (std::out_of_range e)
		{
			std::println("Oops! called index: {}", element_id);
		}
		this->bind();
		long old_size;
		glGetBufferParameteri64v(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &old_size);
		auto size = this->m_vertices.size() * sizeof(vertex_element);
		if (size > old_size)
		{
			GL_CALL(glBufferData(
				GL_ARRAY_BUFFER, size,
				reinterpret_cast<const void*>(this->m_vertices.values().data()),
				GL_DYNAMIC_DRAW));
		}
		else
		{
			GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, size,
									reinterpret_cast<const void*>(
										this->m_vertices.values().data())));
		}
	}
	void log_values()
	{
		std::println("-----------");
		std::ranges::for_each(
			static_cast<std::vector<vertex_element>>(this->m_vertices.values()),
			[](vertex_element& value) {
				std::println("Pos: ({}, {}), UV: ({}, "
							 "{}), Layer: {}",
							 value.m_position.x, value.m_position.y,
							 value.m_uv.x, value.m_uv.y, value.m_tex_layer);
			});
	}
	auto bind() const -> void
	{
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, this->m_renderer_id));
	};
	static auto unbind() -> void
	{
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, NULL));
	};
	vertex_buffer(const vertex_buffer&) = delete;
	vertex_buffer(vertex_buffer&&) = delete;
	vertex_buffer& operator=(const vertex_buffer&) = delete;
	vertex_buffer& operator=(vertex_buffer&&) = delete;
};
} // namespace moonstone::renderer
