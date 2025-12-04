module;

#include "Assert.hpp"
#include "glad/glad.h"
#include <cstdint>
#include <flat_map>
#include <initializer_list>
#include <print>
#include <stdexcept>

export module moonstone:vertex_buffer;

import :vertex_element;

export namespace moonstone::renderer
{
class vertex_buffer
{
	std::uint32_t m_renderer_id{};
	std::uint32_t m_vertex_count{0};
	std::flat_map<std::uint32_t, vertex_element> m_vertices;
	void update_data()
	{
		// I know it's bad to cast this value to a const void* but that's just how OpenGL is.
		// I don't really have a choice but to do this
		// Anyways m_vertices is a flat_map so the values are an std::vector of vertex_element
		// so casting it as a void pointer with its own size should be *fine*
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, this->m_vertices.size() * sizeof(vertex_element),
							 reinterpret_cast<const void*>(&this->m_vertices.values()), GL_STATIC_DRAW));
	}

public:
	vertex_buffer(std::initializer_list<vertex_element> elements) : m_vertices{}
	{
		GL_CALL(glGenBuffers(1, &this->m_renderer_id));
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, this->m_renderer_id));
		update_data();
	};
	~vertex_buffer()
	{
		GL_CALL(glDeleteBuffers(1, &this->m_renderer_id));
	};
	std::uint32_t insert(vertex_element element)
	{
		this->m_vertices.emplace(this->m_vertex_count, element);
		return this->m_vertex_count++;
	}
	void update(std::uint32_t element_id, vertex_element element)
	{
		try
		{
			auto& element_ref = this->m_vertices.at(element_id);
			element_ref.m_normal = element.m_normal;
			element_ref.m_uv = element.m_uv;
			element_ref.m_tex_layer = element.m_tex_layer;
			element_ref.m_position = element.m_position;
		}
		catch (std::out_of_range e)
		{
			std::println("Oops! called index: {}", element_id);
		}
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
