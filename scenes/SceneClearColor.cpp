module;

#define GLFW_INCLUDE_NONE
#include <Try.hpp>
#include <array>
#include <glad/glad.h>
#include <imgui.h>
#include <string_view>

export module scenes:clear_color;

import moonstone;

export namespace moonstone::scenes
{
class clear_color : public scene
{
	std::array<float, 4> m_clear_color;

public:
	clear_color() : m_clear_color{1.0F, 0.8F, 0.8F, 1.0F} {};
	explicit clear_color(std::array<float, 4> m_clear_color)
		: m_clear_color(m_clear_color)
	{
	}
	~clear_color() override = default;

	error::result<> on_update(float delta_time) override
	{
		return {};
	};
	error::result<> on_render() override
	{
		Try(renderer::gl().call(glClearColor, m_clear_color[0],
								m_clear_color[1], m_clear_color[2],
								m_clear_color[3]));
		Try(renderer::gl().call(glClear, GL_COLOR_BUFFER_BIT));
		return {};
	}
	error::result<> on_imgui_render() override
	{
		ImGui::ColorEdit4("Clear Color", m_clear_color.data());
		return {};
	}
	[[nodiscard]] const char* get_name() const override
	{
		return "Clear color.";
	};
};
} // namespace moonstone::scenes
