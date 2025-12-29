module;

#include "Try.hpp"
#include <cstdint>
#include <exception>
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
template <typename T, std::size_t N>
class vertex_buffer
{
	std::uint32_t m_renderer_id{};
	synchronized_buffer<vertex_element, 4> m_buffer;
	error::result<> create()
	{
		Try(gl().call(glGenBuffers, 1, &this->m_renderer_id));
		Try(this->bind());
		auto [data, size, lock] = this->m_buffer.read();
		Try(gl().call(
			glBufferData, GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW));
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
	buffer_connection<synchronized_buffer<T, N>, T, N> connect()
	{
		auto connection = this->m_buffer.connect();
		return connection;
	}
	void replace(std::size_t key, const std::array<T, N>& data)
	{
		try
		{
			this->m_buffer.update(key, data);
		}
		catch (const std::out_of_range& e)
		{
			std::println("Oops! called key: {}", key);
		}
	}
	error::result<> update()
	{
		Try(this->bind());
		std::int64_t old_size = 0;
		Try(gl().call(glGetBufferParameteri64v,
					  GL_ARRAY_BUFFER,
					  GL_BUFFER_SIZE,
					  &old_size));
		auto [data, size, lock] = this->m_buffer.read();
		if (size > static_cast<std::uint64_t>(old_size))
		{
			Try(gl().call(
				glBufferData, GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW));
		}
		else
		{
			Try(gl().call(glBufferSubData, GL_ARRAY_BUFFER, 0, size, data));
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
