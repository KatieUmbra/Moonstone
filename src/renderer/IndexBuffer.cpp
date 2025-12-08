module;

#include "Assert.hpp"
#include "glad/glad.h"

export module moonstone:index_buffer;

export namespace moonstone::renderer
{
class index_buffer
{
	unsigned int m_renderer_id{};
	unsigned int m_count;

public:
	index_buffer(const unsigned int* data, unsigned int count) : m_count{count}
	{
		GL_CALL(glGenBuffers(1, &this->m_renderer_id));
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_renderer_id));
		GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
							 this->m_count * sizeof(unsigned int), data,
							 GL_STATIC_DRAW));
	}

	~index_buffer()
	{
		GL_CALL(glDeleteBuffers(1, &this->m_renderer_id));
	}

	void bind() const
	{
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_renderer_id));
	}

	void unbind() const
	{
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
	inline auto get_count() -> unsigned int
	{
		return this->m_count;
	}
};
} // namespace moonstone::renderer
