module;

#include "Try.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/ext/vector_float4.hpp>
#include <stdexcept>

export module moonstone:renderer;

import :window;
import :shader;
import :vertex_array;
import :index_buffer;
import :error;
import :call;

export namespace moonstone::renderer
{

class renderer
{
	window& m_window;

public:
	explicit renderer(window& wd) : m_window{wd}
	{
#ifdef _DEBUG
		auto err = gl().call(glEnable, GL_DEBUG_OUTPUT);
		if (!err.has_value())
		{
			throw std::runtime_error(err.error().format());
		}
#endif
	}
	static error::result<> draw(const vertex_array& vao, index_buffer& ib,
								const shader& shader)
	{
		Try(shader.bind());
		Try(vao.bind());
		Try(ib.bind());
		Try(gl().call(glDrawElements, GL_TRIANGLES, ib.get_size(),
					  GL_UNSIGNED_INT, nullptr));
		return {};
	}
	static error::result<> clear()
	{
		Try(gl().call(glClear, GL_COLOR_BUFFER_BIT));
		return {};
	}
	static error::result<> clear(glm::vec4 color)
	{
		Try(renderer::clear());
		Try(gl().call(glClearColor, color.r, color.g, color.b, color.a));
		return {};
	}
	void update_buffers()
	{
		glfwPollEvents();
		glfwSwapBuffers(this->m_window.get_glfw_window());
	}
};
} // namespace moonstone::renderer
