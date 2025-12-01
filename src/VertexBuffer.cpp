module;

#include "Assert.hpp"
#include "glad/glad.h"

export module moonstone:vertex_buffer;

export namespace moonstone
{
class vertex_buffer
{
	unsigned int m_renderer_id{};

public:
	vertex_buffer(const void* data, unsigned int size)
	{
		GL_CALL(glGenBuffers(1, &this->m_renderer_id));
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, this->m_renderer_id));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
	};
	~vertex_buffer()
	{
		GL_CALL(glDeleteBuffers(1, &this->m_renderer_id));
	};

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
} // namespace moonstone
