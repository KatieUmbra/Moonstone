module;

#include "Try.hpp"
#include "glad/glad.h"
#include <cstdint>
#include <exception>
#include <print>
#include <stdexcept>

export module moonstone:vertex_array;

import :vertex_buffer;
import :buffer_layout;
import :error;
import :call;

export namespace moonstone::renderer
{
class vertex_array
{
	std::uint32_t m_renderer_id{};

public:
	vertex_array()
	{
		auto res = gl().call(glGenVertexArrays, 1, &this->m_renderer_id);
		if (!res.has_value())
		{
			std::terminate();
			throw std::runtime_error(res.error().format().c_str());
		}
	}
	~vertex_array()
#ifdef _DEBUG
	{
		auto res = gl().call(glDeleteVertexArrays, 1, &this->m_renderer_id);
		if (!res.has_value())
		{
			std::println(stderr, "{}", res.error().format());
			std::terminate();
		}
	}
#else
	{
		gl().call(glDeteglDeleteVertexArrays, 1, &this->m_renderer_id);
	}
#endif
	template <typename T, std::size_t N>
	[[nodiscard]] error::result<> add_buffer(const vertex_buffer<T, N>& vb,
											 const buffer_layout& bl) const
	{
		Try(this->bind());
		Try(vb.bind());
		const auto& elements = bl.get_elements();
		std::uintptr_t offset = 0;
#pragma unroll 4
		for (std::uint32_t i = 0; i < elements.size(); i++)
		{
			const auto& element = elements[i];
			const auto stride = bl.get_stride();
			const auto size = element.count;
			Try(gl().call(glEnableVertexAttribArray, i));
			Try(gl().call(glVertexAttribPointer, i, size, element.type,
						  (element.normalized ? GL_TRUE : GL_FALSE), stride,
						  reinterpret_cast<const void*>(offset)));
			offset += static_cast<std::uintptr_t>(element.count) *
					  buffer_element::get_size_of_type(element.type);
		}
		return {};
	}
	[[nodiscard]] error::result<> bind() const
	{
		Try(gl().call(glBindVertexArray, this->m_renderer_id));
		return {};
	}
	static error::result<> unbind()
	{
		Try(gl().call(glBindVertexArray, 0));
		return {};
	}
	vertex_array(const vertex_array&) = delete;
	vertex_array(vertex_array&&) = delete;
	vertex_array& operator=(const vertex_array&) = delete;
	vertex_array& operator=(vertex_array&&) = delete;
};
} // namespace moonstone::renderer
