module;

#include <concepts>
#include <cstdint>
#include <expected>
#include <format>
#include <glad/glad.h>
#include <print>
#include <queue>
#include <source_location>
#include <string>
#include <string_view>
#include <utility>

export module moonstone:error;

namespace moonstone::error
{
void GLAPIENTRY gl_message_callback(GLenum source, GLenum type, GLuint id,
									GLenum severity, GLsizei length,
									const GLchar* message,
									const void* userParam);
}

export namespace moonstone::error
{
void init()
{
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(gl_message_callback, nullptr);
	GLuint unusedIds = 0;
	glDebugMessageControl(
		GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, 1U);
}
template <typename T>
concept is_error = requires(T v) {
	{ v.set_location(std::source_location()) };
	{ v.set_file(std::string()) };
	{ v.get_location() } -> std::same_as<std::source_location>;
	{ v.format() } -> std::same_as<std::string>;
};
struct shader_error
{
	std::source_location location;
	std::string message;
	shader_error(std::source_location l = std::source_location::current(),
				 std::string message = "") :
		location(l),
		message(std::move(message))
	{
	}
	void set_location(std::source_location loc)
	{
		this->location = loc;
	}
	void set_file(std::string)
	{
	}
	std::source_location get_location()
	{
		return this->get_location();
	}
	std::string format()
	{
		return std::format("[{}:{}][Shader]: {}",
						   location.file_name(),
						   location.line(),
						   message);
	}
};
struct gl_error
{
	std::source_location location;
	std::string file;
	std::string_view source;
	std::string_view type;
	std::uint32_t id;
	std::string_view severity;
	std::string message;

	gl_error(std::string_view source, std::string file, std::string_view type,
			 std::uint32_t id, std::string_view severity, std::string message) :
		source(source),
		file(std::move(file)),
		type(type),
		id(id),
		severity(severity),
		message(std::move(message))
	{
	}

	[[nodiscard]] std::string format() const
	{
		return std::format("[{}:{}][{};{};{}][{}]: {}",
						   location.file_name(),
						   location.line(),
						   source,
						   type,
						   severity,
						   id,
						   message);
	};
	void set_location(std::source_location location)
	{
		this->location = location;
	}
	void set_file(std::string file)
	{
	}
	std::source_location get_location()
	{
		return this->location;
	}

	gl_error(const gl_error&) = default;
	gl_error(gl_error&&) = default;
	gl_error& operator=(const gl_error&) = default;
	gl_error& operator=(gl_error&&) = default;
};

template <typename T = void, is_error E = gl_error>
using result = std::expected<T, E>;
}; // namespace moonstone::error
namespace moonstone::error
{
std::queue<gl_error> errors{};
void GLAPIENTRY gl_message_callback(GLenum source, GLenum type, GLuint id,
									GLenum severity, GLsizei length,
									const GLchar* message,
									const void* userParam)
{
	const char* n_source{""};
	const char* n_type{""};
	const char* n_severity{""};
	if (type == GL_DEBUG_TYPE_ERROR)
	{
		n_type = "ERROR";
	}
	else
	{
		n_type = "OTHER";
	}
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		n_source = "API";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		n_source = "SHADER COMPILER";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		n_source = "WINDOW SYSTEM";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		n_source = "THIRD PARTY";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		n_source = "APPLICATION";
		break;
	case GL_DEBUG_SOURCE_OTHER:
	default:
		n_source = "OTHER";
		break;
	}
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		n_severity = "HIGH";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		n_severity = "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		n_severity = "LOW";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		n_severity = "NOTIFICATION";
		break;
	default:
		n_severity = "OTHER";
		break;
	}
	gl_error err =
		gl_error{{n_source}, {}, {n_type}, id, {n_severity}, message};
	errors.emplace(err);
}
} // namespace moonstone::error
