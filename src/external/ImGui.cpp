module;

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

export module external:imgui;

export namespace moonstone::external::imgui
{
void cleanup_imgui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void init_imgui(GLFWwindow* wnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	auto& style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
	style.Colors[ImGuiCol_WindowBg].w = 1.0f;

	ImGui_ImplGlfw_InitForOpenGL(
		wnd, true); // Second param install_callback=true will install GLFW
					// callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();
}

void start_render_imgui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void end_render_imgui()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace moonstone::external::imgui
