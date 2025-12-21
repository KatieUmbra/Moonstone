module;

#include <filesystem>
#include <fstream>
#include <iterator>
#include <source_location>
#include <string>
#include <string_view>

export module moonstone:utility;

export namespace moonstone
{
auto read_shader_file(const std::string& file) -> std::string
{
	std::source_location location = std::source_location::current();
	std::string_view file_location{location.file_name()};
	std::string_view pretty_file = file_location.substr(
		file_location.rfind('/') + 1, file_location.rfind('.'));
	const std::string path{"shaders/" + file};
	std::ifstream fs{path};
	std::string shader;
	shader.assign((std::istreambuf_iterator<char>(fs)),
				  (std::istreambuf_iterator<char>()));
	return shader;
}
} // namespace moonstone
