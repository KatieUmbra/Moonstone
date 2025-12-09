module;

#include "Assert.hpp"
#include "glad/glad.h"
#include <cstddef>
#include <cstdint>

export module moonstone:vertex_array;

import :vertex_buffer;
import :buffer_layout;

export namespace moonstone::renderer
{
class vertex_array
{
	std::uint32_t m_renderer_id{};

public:
	vertex_array()
	{
		GL_CALL(glGenVertexArrays(1, &this->m_renderer_id));
	}
	~vertex_array()
	{
		GL_CALL(glDeleteVertexArrays(1, &this->m_renderer_id));
	}
	void add_buffer(const vertex_buffer& vb, const buffer_layout& bl) const
	{
		this->bind();
		vb.bind();
		const auto& elements = bl.get_elements();
		std::uintptr_t offset = 0;
#pragma unroll 4
		for (std::uint32_t i = 0; i < elements.size(); i++)
		{
			const auto& element = elements[i];
			const auto stride = bl.get_stride();
			const auto size = element.count;
			GL_CALL(glEnableVertexAttribArray(i));
			GL_CALL(glVertexAttribPointer(
						i, size, element.type,
						(element.normalized ? GL_TRUE : GL_FALSE), stride,
						reinterpret_cast<const void*>(offset));)
			offset += static_cast<std::uintptr_t>(element.count) *
					  buffer_element::get_size_of_type(element.type);
		}
	}
	void bind() const
	{
		GL_CALL(glBindVertexArray(this->m_renderer_id));
	}
	static void unbind()
	{
		GL_CALL(glBindVertexArray(0));
	}
	vertex_array(const vertex_array&) = delete;
	vertex_array(vertex_array&&) = delete;
	vertex_array& operator=(const vertex_array&) = delete;
	vertex_array& operator=(vertex_array&&) = delete;
};
} // namespace moonstone::renderer
