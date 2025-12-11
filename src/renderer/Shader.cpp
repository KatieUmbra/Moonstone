module;

#define GLFW_INCLUDE_NONE
#include "Try.hpp"
#include "glad/glad.h"
#include <cstdint>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <print>
#include <source_location>
#include <string>
#include <unordered_map>
#include <utility>

export module moonstone:shader;

import :utility;
import :call;
import :error;

export namespace moonstone::renderer
{
class shader
{
	unsigned int m_renderer_id;
	std::string m_vs_file_path;
	std::string m_fs_file_path;
	std::unordered_map<std::string, int> m_uniform_location_cache;

	shader(std::string vs_path, std::string fs_path, std::uint32_t id)
		: m_fs_file_path{std::move(fs_path)},
		  m_vs_file_path{std::move(vs_path)}, m_renderer_id{id}
	{
	}

	auto get_uniform_location(const std::string& name)
		-> error::result<std::uint32_t>
	{
		if (this->m_uniform_location_cache.find(name) !=
			this->m_uniform_location_cache.end())
		{
			return this->m_uniform_location_cache[name];
		}

		const std::uint32_t location = Try(gl().call_returning<std::uint32_t>(
			glGetUniformLocation, this->m_renderer_id, name.c_str()));

		if (location == -1)
		{
			auto location = std::source_location::current();
			std::println(stderr,
						 "[{}:{}][OpenGL][WARN]: uniform {} doesn't exist!",
						 location.file_name(), location.line(), name);
		}

		this->m_uniform_location_cache[name] = location;
		return location;
	}
	[[nodiscard]] static auto create_shader(
		const std::string& vertex_shader_file,
		const std::string& fragment_shader_file) -> error::result<std::uint32_t>
	{

		auto vertex_shader = read_shader_file(vertex_shader_file);

		auto fragment_shader = read_shader_file(fragment_shader_file);

		auto program = Try(gl().call_returning<std::uint32_t>(glCreateProgram));
		auto vs = Try(compile_shader(vertex_shader, GL_VERTEX_SHADER));
		auto fs = Try(compile_shader(fragment_shader, GL_FRAGMENT_SHADER));

		Try(gl().call(glAttachShader, program, vs));
		Try(gl().call(glAttachShader, program, fs));
		Try(gl().call(glLinkProgram, program));
		Try(gl().call(glValidateProgram, program));
		Try(gl().call(glDeleteShader, vs));
		Try(gl().call(glDeleteShader, fs));

		return program;
	}
	[[nodiscard]] static auto compile_shader(const std::string& source,
											 unsigned int type)
		-> error::result<std::uint32_t>
	{

		std::uint32_t id =
			Try(gl().call_returning<std::uint32_t>(glCreateShader, type));
		const char* src = source.c_str();
		Try(gl().call(glShaderSource, id, 1, &src, nullptr));
		Try(gl().call(glCompileShader, id));
		std::int32_t result{};
		Try(gl().call(glGetShaderiv, id, GL_COMPILE_STATUS, &result));
		if (result == GL_FALSE)
		{
			int length{};
			Try(gl().call(glGetShaderiv, id, GL_INFO_LOG_LENGTH, &length));
			char* message = static_cast<char*>(alloca(length * sizeof(char)));
			Try(gl().call(glGetShaderInfoLog, id, length, &length, message));
			std::println(stderr, "Failed to compile {} shader: {}",
						 (type == GL_VERTEX_SHADER ? "vertex" : "fragment"),
						 message);
			Try(gl().call(glDeleteShader, id));
			return std::unexpected(error::gl_error{
				"SHADER COMPILER", {}, "ERROR", 0, "HIGH", message});
		}
		return id;
	}

public:
	~shader()
	{
		const auto& result = gl().call(glDeleteProgram, m_renderer_id);
		if (!result.has_value())
		{
			std::println(stderr, "{}", result.error().format());
			__builtin_debugtrap();
		}
	}
	shader(const std::string& vs_path, const std::string& fs_path)
	{
		const auto& result = create_shader(vs_path, fs_path);
		if (!result.has_value())
		{
			std::println(stderr, "{}", result.error().format());
			__builtin_debugtrap();
		}
		this->m_vs_file_path = vs_path;
		this->m_fs_file_path = fs_path;
		this->m_renderer_id = result.value();
	}

	[[nodiscard]] error::result<> bind() const
	{
		Try(gl().call(glUseProgram, this->m_renderer_id));
		return {};
	}
	static error::result<> unbind()
	{
		Try(gl().call(glUseProgram, 0));
		return {};
	}

	auto setUniformVecf4(const std::string& name, glm::vec4 data)
		-> error::result<>
	{
		std::uint32_t location = Try(this->get_uniform_location(name));
		Try(gl().call(glUniform4f, location, data.x, data.y, data.z, data.w));
		return {};
	}
	auto setUniformVecf3(const std::string& name, glm::vec3 data)
		-> error::result<>
	{
		std::uint32_t location = Try(this->get_uniform_location(name));
		Try(gl().call(glUniform3f, location, data.x, data.y, data.z));
		return {};
	}

	auto setUniformMatf4(const std::string& name, const glm::mat4& data)
		-> error::result<>
	{
		std::uint32_t location = Try(this->get_uniform_location(name));
		Try(gl().call(glUniformMatrix4fv, location, 1, GL_FALSE,
					  glm::value_ptr(data)));
		return {};
	}

	auto setUniformInt1(const std::string& name, int data) -> error::result<>
	{
		std::uint32_t location = Try(this->get_uniform_location(name));
		Try(gl().call(glUniform1i, location, data));
		return {};
	}

	shader(const shader&) = delete;
	shader(shader&&) = default;
	shader& operator=(const shader&) = delete;
	shader& operator=(shader&&) = default;
};
} // namespace moonstone::renderer
