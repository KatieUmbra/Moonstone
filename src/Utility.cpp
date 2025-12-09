module;

#include <filesystem>
#include <fstream>
#include <functional>
#include <iterator>
#include <print>
#include <source_location>
#include <string>
#include <string_view>
#include <utility>

export module moonstone:utility;

export namespace moonstone
{
auto get_executable_path() -> std::string
{
#ifdef _DEBUG
	auto path = std::filesystem::canonical("/proc/self/exe")
					.parent_path()
					.parent_path();
#else
	auto path = std::filesystem::canonical("/proc/self/exe").parent_path();
#endif
	return path.string();
}
auto read_shader_file(const std::string& file) -> std::string
{
	std::source_location location = std::source_location::current();
	std::string_view file_location{location.file_name()};
	std::string_view pretty_file = file_location.substr(
		file_location.rfind('/') + 1, file_location.rfind('.'));
	const auto* shader_path = _ROOTDIR "/shaders/";
	const std::string path{shader_path + file};
	std::ifstream fs{path};
	std::string shader;
	shader.assign((std::istreambuf_iterator<char>(fs)),
				  (std::istreambuf_iterator<char>()));
	return shader;
}
} // namespace moonstone
