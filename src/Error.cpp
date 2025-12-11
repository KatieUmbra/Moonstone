module;

#include <cstdint>
#include <glad/glad.h>
#include <queue>
#include <source_location>
#include <stdexcept>
#include <string>
#include <string_view>

export module moonstone:error;

namespace moonstone
{
void GLAPIENTRY gl_message_callback(GLenum source, GLenum type, GLuint id,
									GLenum severity, GLsizei length,
									const GLchar* message,
									const void* userParam);
}

export namespace moonstone
{
struct gl_error
{
	std::string_view source;
	std::string_view type;
	std::uint32_t id;
	std::string_view severity;
} __attribute__((packed, aligned(64)));

enum kind
{
	OpenGL
};

template <typename T = void> class error
{
public:
	kind m_kind;
	std::source_location m_location;
	std::string m_message;
	T m_data;
	error(kind kind, std::source_location location, std::string message, T data)
		: m_kind(kind), m_location(location), m_message(std::move(message)),
		  m_data(std::move(data))
	{
	}

	~error() = default;

	static void init()
	{
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(gl_message_callback, nullptr);
	};

	template <typename V> static error<T> from(const T& t)
	{
		throw std::runtime_error{
			"Invalid usage of function moonstone::error::from"};
	};

	template <> error<gl_error> from<gl_error>(const gl_error& t)
	{
		return error{kind::OpenGL, std::source_location::current(), "", t};
	};
	error(const error&) = default;
	error(error&&) = default;
	error& operator=(const error&) = delete;
	error& operator=(error&&) = delete;
};
} // namespace moonstone

namespace moonstone
{
std::queue<error<gl_error>> errors{};
void GLAPIENTRY gl_message_callback(GLenum source, GLenum type, GLuint id,
									GLenum severity, GLsizei length,
									const GLchar* message,
									const void* userParam)
{
	const char* n_source;
	const char* n_type;
	const char* n_severity;
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		n_type = "ERROR";
		break;
	default:
		return;
		break;
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
	error<gl_error> err =
		error<gl_error>::from<gl_error>(gl_error{.source = {n_source},
												 .type = {n_type},
												 .id = id,
												 .severity = {n_severity}});
	err.m_message = message;
}
} // namespace moonstone
