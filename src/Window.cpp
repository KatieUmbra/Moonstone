module;

#define GLFW_INCLUDE_NONE
#include "Try.hpp"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <exception>
#include <glad/glad.h>
#include <print>

export module moonstone:window;

import :error;
import :call;

export namespace moonstone
{
struct window_properties
{
	std::uint16_t width{}, height{};
	const char* title{};
	bool vsync{};
	bool fullscreen{};
} __attribute__((aligned(128))) __attribute__((packed));

class window
{
	GLFWwindow* m_window;

	static error::result<> create()
	{
		Try(renderer::gl().call(
			glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		Try(renderer::gl().call(glEnable, GL_BLEND));
		return {};
	}

public:
	explicit window(const window_properties& props) :
		m_window{glfwCreateWindow(props.width, props.height, props.title,
								  nullptr, nullptr)}
	{
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwMakeContextCurrent(this->m_window);
		if (props.vsync)
			glfwSwapInterval(1);
		if (gladLoadGLLoader(
				reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0)
		{
			std::println(stderr, "Failed to initialize GLAD");
			std::terminate();
		}
		auto err = window::create();
		if (!err.has_value())
		{
			std::println(stderr, "{}", err.error().format());
			std::terminate();
		}
	}
	~window() = default;
	[[nodiscard]] bool loop() const
	{
		return glfwWindowShouldClose(this->m_window) == 0;
	};
	void set_icon(const std::vector<GLFWimage>& imgs)
	{
		glfwSetWindowIcon(this->m_window, 1, imgs.data());
	};
	auto get_glfw_window() -> GLFWwindow*
	{
		return this->m_window;
	}
	window(const window&) = delete;
	window(window&&) = delete;
	window& operator=(const window&) = delete;
	window& operator=(window&&) = delete;
};
} // namespace moonstone
