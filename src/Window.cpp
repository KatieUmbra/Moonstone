module;

#define GLFW_INCLUDE_NONE
#include "Try.hpp"
#include <cstdint>
#include <exception>
#include <glad/glad.h>
#include <glfwpp/window.h>
#include <print>

export module moonstone:window;

import :error;
import :call;

export namespace moonstone
{
struct window_properties
{
	glfw::WindowHints hints;
	std::uint16_t width{}, height{};
	const char* title{};
	bool vsync{};
	bool fullscreen{};
} __attribute__((aligned(128))) __attribute__((packed));

class window
{
	glfw::Window m_window;

	static error::result<> create()
	{
		Try(renderer::gl().call(glBlendFunc, GL_SRC_ALPHA,
								GL_ONE_MINUS_SRC_ALPHA));
		Try(renderer::gl().call(glEnable, GL_BLEND));
		return {};
	}

public:
	explicit window(const window_properties& props)
		: m_window{glfw::Window{props.width, props.height, props.title}}
	{

		props.hints.apply();
		glfw::makeContextCurrent(this->m_window);
		if (props.vsync)
			glfw::swapInterval(1);
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
		return !this->m_window.shouldClose();
	};
	void set_icon(const std::vector<glfw::Image>& imgs)
	{
		this->m_window.setIcon(imgs);
	};
	auto get_glfw_window() -> glfw::Window&
	{
		return this->m_window;
	}
	window(const window&) = delete;
	window(window&&) = delete;
	window& operator=(const window&) = delete;
	window& operator=(window&&) = delete;
};
} // namespace moonstone
