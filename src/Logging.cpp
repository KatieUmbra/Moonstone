module;

#include <chrono>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <print>
#include <string>

export module moonstone:logging;

export namespace moonstone
{
constexpr auto setup_logging() -> std::unique_ptr<std::ofstream>
{
	const std::string file{_SOLUTIONDIR "/log.txt"};
	auto logging_file = std::make_unique<std::ofstream>(file, std::ios::app);
	auto current = std::chrono::system_clock::now();
	std::println("\n===================");
	std::println("{}", current);
	std::println("===================");
	return logging_file;
}
} // namespace moonstone
