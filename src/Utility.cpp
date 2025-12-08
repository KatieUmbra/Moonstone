module;

#include <filesystem>
#include <fstream>
#include <functional>
#include <print>
#include <string>
#include <utility>

export module moonstone:utility;

export namespace moonstone
{
auto get_executable_path() -> std::string
{
#ifdef _DEBUG
	auto path = std::filesystem::canonical("/proc/self/exe").parent_path().parent_path();
#else
	auto path = std::filesystem::canonical("/proc/self/exe").parent_path();
#endif
	return path.string();
}
auto read_shader_file(const std::string& file) -> std::string
{
	const auto* shader_path = _ROOTDIR "/shaders/";
	std::println("[OpenGL][INFO]: Shader Path {}", shader_path);
	const std::string path{shader_path + file};
	std::ifstream fs{path};
	std::string shader;
	shader.assign((std::istreambuf_iterator<char>(fs)), (std::istreambuf_iterator<char>()));
	return shader;
}
class defer
{
	std::function<void()> m_defered;

public:
	explicit defer(std::function<void()> defered) : m_defered{std::move(defered)}
	{
	}
	~defer()
	{
		m_defered();
	}
	defer(const defer&) = default;
	defer(defer&&) = delete;
	defer& operator=(const defer&) = default;
	defer& operator=(defer&&) = delete;
};
} // namespace moonstone
