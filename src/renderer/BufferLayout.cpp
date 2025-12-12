module;

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

export module moonstone:buffer_layout;

export namespace moonstone::renderer
{
struct buffer_element
{
	std::uint32_t type;
	std::uint32_t count;
	bool normalized;

	static auto get_size_of_type(unsigned int type) -> unsigned int
	{
		switch (type)
		{
		case GL_FLOAT:
		case GL_UNSIGNED_INT:
			return 4;
		case GL_UNSIGNED_BYTE:
			return 1;
		default:
			return 0;
		}
	}
} __attribute__((packed, aligned(16)));
class buffer_layout
{
	std::vector<buffer_element> m_elements;
	unsigned int m_stride;

public:
#define PUSH(TYPE, GL_TYPE, NORMALIZE)                                         \
	template <> void push<TYPE>(unsigned int count)                            \
	{                                                                          \
		this->m_elements.emplace_back(                                         \
			buffer_element{GL_TYPE, count, NORMALIZE});                        \
		this->m_stride += buffer_element::get_size_of_type(GL_TYPE) * count;   \
	}

	template <typename T> void push(std::uint32_t /*Count*/)
	{
		std::terminate();
	}

	PUSH(std::float_t, GL_FLOAT, false);
	PUSH(std::int32_t, GL_INT, false);
	PUSH(std::uint32_t, GL_UNSIGNED_INT, false);
	PUSH(std::byte, GL_UNSIGNED_BYTE, true);

	[[nodiscard]] const std::vector<buffer_element>& get_elements() const
	{
		return this->m_elements;
	}

	[[nodiscard]] std::uint32_t get_stride() const
	{
		return this->m_stride;
	}
};
} // namespace moonstone::renderer
