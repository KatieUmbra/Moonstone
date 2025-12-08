module;

#define GLFW_INCLUDE_NONE
#include <array>
#include <cstdint>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>
#include <imgui.h>
#include <string>

export module scenes:texture;

import moonstone;

static glm::mat4 view = glm::translate(glm::mat4{1.0F}, glm::vec3(0, 0, 0));
static glm::mat4 projection =
	glm::ortho(0.0f, 800.0f, 0.0f, 800.0f, -1.0f, 1.0f);

export namespace moonstone::scenes
{
class texture : public moonstone::scene
{
	std::array<unsigned int, 12> indices;
	renderer::vertex_array vao;
	renderer::index_buffer ibo;
	renderer::vertex_buffer vbo;
	moonstone::engine::quad quad1, quad2;
	renderer::buffer_layout blo;
	renderer::shader shader;
	renderer::texture tex;
	renderer::renderer& renderer;
	glm::vec2 new_pos1, new_pos2;

public:
	explicit texture(renderer::renderer& renderer)
		: quad1{{}, {200.0F, 200.0F}, {0.0F, 0.0F}, 0, vbo, ibo},
		  quad2{{}, {50.0F, 50.0F}, {0.5F, 0.5F}, 0, vbo, ibo},
		  indices{0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4}, ibo{}, blo{},
		  shader{"shader.vert", "shader.frag"}, tex{"texture.png"},
		  renderer{renderer}, new_pos1()
	{
		renderer::vertex_element::register_layout(blo);
		vao.add_buffer(vbo, blo);
		vao.bind();
		ibo.bind();
		shader.bind();
		tex.bind();

		shader.setUniformInt1("u_texture", 0);

		moonstone::renderer::vertex_array::unbind();
		ibo.unbind();
		shader.unbind();
	}
	~texture()
	{
		moonstone::renderer::vertex_array::unbind();
		moonstone::renderer::vertex_buffer::unbind();
		ibo.unbind();
		shader.unbind();
	}
	void on_update(float delta_time) override
	{
	}
	void on_render() override
	{
		shader.bind();
		glm::mat4 model = glm::mat4{1.0F};
		glm::mat4 mvp = projection * view * model;
		shader.setUniformMatf4("u_model_view_projection", mvp);
		renderer.draw(vao, ibo, shader);
	};
	void on_imgui_render() override
	{
		ImGui::SliderFloat2("Position1", &this->new_pos1.x, -300.0F, 300.0F);
		ImGui::SliderFloat2("Position2", &this->new_pos2.x, -300.0F, 300.0F);
		this->quad1.set_position(this->new_pos1);
		this->quad2.set_position(this->new_pos2);
		this->vbo.update();
	}
	bool operator==(const scene& other) override
	{

		return this->get_name() == other.get_name();
	}
	[[nodiscard]] std::string get_name() const override
	{
		return "Texture";
	}
	texture(const texture&) = delete;
	texture(texture&&) = delete;
	texture& operator=(const texture&) = delete;
	texture& operator=(texture&&) = delete;
};
} // namespace moonstone::scenes
