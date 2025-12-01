module;

#include "Assert.hpp"
#include "glad/glad.h"
#include "glfwpp/event.h"
#include "glm/vec4.hpp"

export module moonstone:renderer;

import :vertex_array;
import :window;
import :index_buffer;
import :shader;

export namespace moonstone
{

class renderer
{
	window& m_window;

public:
	renderer(window& wd) : m_window{wd}
	{
	}
	void draw(const vertex_array& vao, index_buffer& ib, const shader& shader)
	{
		shader.bind();
		vao.bind();
		ib.bind();
		GL_CALL(glDrawElements(GL_TRIANGLES, ib.get_count(), GL_UNSIGNED_INT, nullptr));
	}
	void clear() const
	{
		GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
	}
	void clear(float r, float g, float b, float a) const
	{
		this->clear();
		GL_CALL(glClearColor(r, g, b, a));
	}
	void clear(glm::vec4 color) const
	{

		this->clear();
		GL_CALL(glClearColor(color.x, color.y, color.z, color.w));
	}
	void update_buffers()
	{

		glfw::pollEvents();
		this->m_window.get_glfw_window().swapBuffers();
	}
};
} // namespace moonstone
