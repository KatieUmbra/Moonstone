module;

#define GLFW_INCLUDE_NONE
#include "Assert.hpp"
#include <array>
#include <imgui.h>

export module scenes:clear_color;

import moonstone;

export namespace moonstone::scenes
{
class clear_color : public scene
{
	std::array<float, 4> m_clear_color;

public:
	clear_color() : m_clear_color{1.0F, 0.8F, 0.8F, 1.0F} {};
	~clear_color() override = default;

	void on_update(float delta_time) override {};
	void on_render() override
	{
		GL_CALL(glClearColor(m_clear_color[0], m_clear_color[1], m_clear_color[2], m_clear_color[3]));
		GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
	}
	void on_imgui_render() override
	{
		ImGui::ColorEdit4("Clear Color", reinterpret_cast<float*>(&m_clear_color));
	}
	bool operator==(const scene& other) override
	{
		return this->get_name() == other.get_name();
	};
	[[nodiscard]] std::string get_name() const override
	{
		return "Clear color.";
	};
};
} // namespace moonstone::scenes
