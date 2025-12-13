module;

#define GLFW_INCLUDE_NONE
#include "Try.hpp"
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>
#include <imgui.h>
#include <print>
#include <stdexcept>
#include <string>

export module scenes:texture;

import moonstone;

constexpr glm::mat4 view = glm::translate(glm::mat4{1.0F}, glm::vec3(0, 0, 0));
static const glm::mat4 projection =
	glm::ortho(0.0F, 800.0F, 0.0F, 800.0F, -1.0F, 1.0F);

export namespace moonstone::scenes
{
class texture : public moonstone::scene
{
	renderer::vertex_array vao;
	renderer::index_buffer ibo;
	renderer::vertex_buffer vbo;
	renderer::buffer_layout blo;
	moonstone::engine::quad quad1, quad2, quad3;
	renderer::shader shader;
	renderer::texture_array<256, 256> tex_arr;
	renderer::renderer& renderer;
	glm::vec2 new_pos1{}, new_pos2{}, new_pos3{};

	error::result<> create()
	{
		Try(vao.add_buffer(vbo, blo));
		Try(vao.bind());
		Try(ibo.bind());
		Try(shader.bind());
		Try(tex_arr.bind());

		Try(shader.setUniformInt1("u_textureArray", 1));

		Try(moonstone::renderer::vertex_array::unbind());
		Try(ibo.unbind());
		Try(shader.unbind());
		return {};
	}

	error::result<> destroy()
	{
		Try(moonstone::renderer::vertex_array::unbind());
		Try(moonstone::renderer::vertex_buffer::unbind());
		Try(ibo.unbind());
		Try(shader.unbind());
		return {};
	}

public:
	explicit texture(renderer::renderer& renderer)
		: quad1{{}, {200.0F, 200.0F}, {0.0F, 0.0F}, 2, vbo, ibo},
		  quad2{{}, {50.0F, 50.0F}, {0.5F, 0.5F}, 0, vbo, ibo},
		  quad3{{}, {300.0F, 300.0F}, {0.0F, 0.0F}, 1, vbo, ibo},
		  tex_arr({"texarr1.png", "texarr2.png", "texarr3.png"}),
		  renderer(renderer), shader{"shader.vert", "shader.frag"}
	{
		error::init();
		renderer::vertex_element::register_layout(blo);
		auto err = this->create();
		if (!err.has_value())
		{
			throw std::runtime_error(err.error().format());
		}
	}
	~texture() override = default;
	error::result<> on_update(float delta_time) override
	{
		return {};
	}
	error::result<> on_render() override
	{
		Try(shader.bind());
		glm::mat4 model = glm::mat4{1.0F};
		glm::mat4 mvp = projection * view * model;
		auto err = shader.setUniformMatf4("u_model_view_projection", mvp);
		if (!err.has_value())
		{
			std::println(stderr, "{}", err.error().format());
		}
		Try(renderer.draw(vao, ibo, shader));
		return {};
	};
	error::result<> on_imgui_render() override
	{
		ImGui::SliderFloat2("Position1", &this->new_pos1.x, 0.0F, 500.0F);
		ImGui::SliderFloat2("Position2", &this->new_pos2.x, 0.0F, 500.0F);
		ImGui::SliderFloat2("Position3", &this->new_pos3.x, 0.0F, 500.0F);
		this->quad1.set_position(this->new_pos1);
		this->quad2.set_position(this->new_pos2);
		this->quad3.set_position(this->new_pos3);
		Try(this->vbo.update());
		return {};
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
