module;

#define GLFW_INCLUDE_NONE
#include <array>
#include <cstdint>
#include <glad/glad.h>
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
	renderer::vertex_array vao;
	renderer::index_buffer ibo{};
	renderer::vertex_buffer vbo{};
	renderer::buffer_layout blo{};
	moonstone::engine::quad quad1, quad2, quad3;
	renderer::shader shader;
	renderer::texture_array<256, 256> tex_arr;
	renderer::renderer& renderer;
	glm::vec2 new_pos1{}, new_pos2{}, new_pos3{};

public:
	explicit texture(renderer::renderer& renderer)
		: quad1{{}, {200.0F, 200.0F}, {0.0F, 0.0F}, 0, vbo, ibo},
		  quad2{{}, {50.0F, 50.0F}, {0.5F, 0.5F}, 1, vbo, ibo},
		  quad3{{}, {300.0F, 300.0F}, {0.0F, 0.0F}, 2, vbo, ibo},
		  shader{"shader.vert", "shader.frag"}, renderer{renderer},
		  tex_arr({"texarr1.png", "texarr2.png", "texarr3.png"})
	{
		renderer::vertex_element::register_layout(blo);
		vao.add_buffer(vbo, blo);
		vao.bind();
		ibo.bind();
		shader.bind();
		tex_arr.bind();

		shader.setUniformInt1("u_textureArray", 1);

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
		GL_INVALID_VALUE;
	};
	void on_imgui_render() override
	{
		ImGui::SliderFloat2("Position1", &this->new_pos1.x, 0.0F, 500.0F);
		ImGui::SliderFloat2("Position2", &this->new_pos2.x, 0.0F, 500.0F);
		ImGui::SliderFloat2("Position3", &this->new_pos3.x, 0.0F, 500.0F);
		this->quad1.set_position(this->new_pos1);
		this->quad2.set_position(this->new_pos2);
		this->quad3.set_position(this->new_pos3);
		this->vbo.update();
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
