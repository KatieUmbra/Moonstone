module;

#include "Try.hpp"
#include <cstdint>
#include <flat_map>
#include <glad/glad.h>
#include <print>
#include <stdexcept>

export module moonstone:vertex_buffer;

import :vertex_element;
import :call;
import :error;
import :sync_buffer;

export namespace moonstone::renderer
{
class vertex_buffer
{
	std::uint32_t m_renderer_id{};
	std::uint32_t m_vertex_count{0};
	synchronized_buffer<vertex_element, 4> m_buffer;
	std::flat_map<std::uint32_t, vertex_element> m_vertices;
	error::result<> create()
	{
		Try(gl().call(glGenBuffers, 1, &this->m_renderer_id));
		Try(this->bind());
		Try(gl().call(glBufferData, GL_ARRAY_BUFFER,
					  this->m_vertices.size() * sizeof(vertex_element),
					  this->m_vertices.values().data(), GL_DYNAMIC_DRAW));
		return {};
	}

public:
	vertex_buffer()
	{
		auto res = this->create();
		if (!res.has_value())
		{
			throw std::runtime_error(res.error().format().c_str());
		}
	};
	~vertex_buffer()
#ifdef _DEBUG
	{
		auto res = gl().call(glDeleteBuffers, 1, &this->m_renderer_id);
		if (!res.has_value())
		{
			std::println(stderr, "{}", res.error().format());
			std::terminate();
		}
	}
#else
	{
		gl().call(glDeleteBuffers, 1, &this->m_renderer_id);
	}
#endif
	error::result<std::uint32_t> insert(vertex_element element)
	{
		this->m_vertices.insert({this->m_vertex_count, element});
		Try(this->update());
		return this->m_vertex_count++;
	}
	void replace(std::uint32_t element_id, vertex_element element)
	{
		try
		{
			vertex_element& element_ref = this->m_vertices.at(element_id);
			element_ref = element;
		}
		catch (const std::out_of_range& e)
		{
			std::println("Oops! called index: {}", element_id);
		}
	}
	error::result<> update()
	{
		Try(this->bind());
		std::int64_t old_size = 0;
		Try(gl().call(glGetBufferParameteri64v, GL_ARRAY_BUFFER, GL_BUFFER_SIZE,
					  &old_size));
		auto size = this->m_vertices.size() * sizeof(vertex_element);
		if (size > static_cast<std::uint64_t>(old_size))
		{
			Try(gl().call(glBufferData, GL_ARRAY_BUFFER, size,
						  this->m_vertices.values().data(), GL_DYNAMIC_DRAW));
		}
		else
		{
			Try(gl().call(glBufferSubData, GL_ARRAY_BUFFER, 0, size,
						  this->m_vertices.values().data()));
		}
		return {};
	}
	[[nodiscard]] error::result<> bind() const
	{
		Try(gl().call(glBindBuffer, GL_ARRAY_BUFFER, this->m_renderer_id));
		return {};
	}
	static error::result<> unbind()
	{
		Try(gl().call(glBindBuffer, GL_ARRAY_BUFFER, 0));
		return {};
	};
	vertex_buffer(const vertex_buffer&) = delete;
	vertex_buffer(vertex_buffer&&) = delete;
	vertex_buffer& operator=(const vertex_buffer&) = delete;
	vertex_buffer& operator=(vertex_buffer&&) = delete;
};
} // namespace moonstone::renderer
