module;

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <glm/glm.hpp>
#include <print>

export module moonstone:vertex_element;

import :buffer_layout;

export namespace moonstone::renderer
{
/// Interface
struct vertex_element
{
	glm::vec3 m_normal;
	glm::vec3 m_uv;
	glm::vec2 m_position;

	vertex_element(glm::vec2 m_position, glm::vec2 m_uv,
				   std::uint32_t m_tex_layer)
		: /* m_normal(m_normal), */ m_position(m_position),
		  m_uv(m_uv.x, m_uv.y, static_cast<std::float_t>(m_tex_layer))
	{
	}

	vertex_element() = default;

	~vertex_element() = default;
	static void register_layout(buffer_layout& layout)
	{
		// m_normal     3 floats
		layout.push<std::float_t>(3);
		// m_uv         3 floats
		layout.push<std::float_t>(3);
		// m_position   2 floats
		layout.push<std::float_t>(2);
		// total        8 floats
		// In total there's 32 bytes
		// so stride will be 32 bytes
		std::println("Stride: {}", layout.get_stride());
	}

	vertex_element(const vertex_element&) = default;
	vertex_element(vertex_element&&) = default;
	vertex_element& operator=(const vertex_element&) = default;
	vertex_element& operator=(vertex_element&&) = default;
} __attribute__((aligned(32), packed));

} // namespace moonstone::renderer
