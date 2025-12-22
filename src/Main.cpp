/*
* File name: Main.cpp
* Author: Katherine
* Date created: 2025-12-18 17:53:02
// Date modified: 2025-12-21 22:23:58
* ===============
*/

#include <cstdlib>
#include <glm/glm.hpp>
#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>

import moonstone;
import scenes;

int main()
{
	// =======================================================
	//                    Initialization
	// =======================================================
	auto logger = moonstone::setup_logging();
	[[maybe_unused]] auto library = glfwInit();
	const moonstone::window_properties props{.width = 800,
											 .height = 800,
											 .title = "Hello OpenGL",
											 .vsync = false,
											 .fullscreen = true};
	moonstone::window window{props};
	moonstone::external::imgui::init_imgui(window.get_glfw_window());
	moonstone::renderer::renderer renderer{window};

	// =======================================================
	//                       Testing
	// =======================================================

	moonstone::scenes::clear_color clear_screen_scene{};
	moonstone::scenes::texture texture_scene{renderer};

	moonstone::scene::register_scene(texture_scene);
	moonstone::scene::register_scene(clear_screen_scene);

	// =======================================================

	auto& tests = moonstone::scene::registered_scenes;
	moonstone::scene base_reference{};
	std::reference_wrapper<moonstone::scene> current_test = base_reference;

	// =======================================================
	//                      Main Loop
	// =======================================================

	while (window.loop())
	{
		renderer.clear();
		if (nullptr != current_test.get().get_name())
		{
			current_test.get().on_update(0.0F);
			current_test.get().on_render();
		}
		else
		{
			renderer.clear({0.1F, 0.1F, 0.1F, 0.1F});
		}

		moonstone::external::imgui::start_render_imgui();
		ImGui::Text("OpenGL Test Application");
		auto framerate = ImGui::GetIO().Framerate;
		ImGui::Text("Framerate %.2f", framerate);
		ImGui::Separator();
		if (current_test.get().get_name() != nullptr)
		{
			ImGui::Text("Test Properties:");
			ImGui::Separator();
			current_test.get().on_imgui_render();
			ImGui::Separator();
			if (ImGui::Button("Back"))
			{
				current_test = base_reference;
			}
		}
		else
		{
			ImGui::Text("Tests Available:");
			ImGui::BeginGroup();
#pragma unroll 2
			for (auto& element : tests)
			{
				if (ImGui::Button(element.get().get_name()))
				{
					current_test = element;
				}
			}
			ImGui::EndGroup();
		}

		moonstone::external::imgui::end_render_imgui();
		renderer.update_buffers();
	}
	moonstone::external::imgui::cleanup_imgui();
	logger->close();
}
