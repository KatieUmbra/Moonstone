module;

#include <concepts>
#include <expected>
#include <queue>
#include <source_location>
#include <utility>

export module moonstone:call;

import :error;

export namespace moonstone::renderer
{
// https://www.cppstories.com/2021/non-terminal-variadic-args/
struct gl
{
	gl(std::source_location l = std::source_location::current())
		: m_location(std::move(l))
	{
	}
	template <typename T, typename F, typename... Args>
	std::expected<T, error<gl_error>> call_returning(F f, Args... args)
	{
		// Empty the queue
		std::queue<error<gl_error>>().swap(errors);
		// Call opengl function
		T returned = f(args...);
		// Check for errors in the queue
		if (errors.empty())
		{
			return returned;
		}
		while (errors.size() > 1)
		{
			errors.pop();
		}
		auto err = errors.back();
		err.m_location = this->m_location;
		return std::unexpected{std::move(err)};
	}
	template <typename F, typename... Args>
	std::expected<void, error<gl_error>> call(F f, Args... args)
	{
		// Empty the queue
		std::queue<error<gl_error>>().swap(errors);
		// Call opengl function
		f(args...);
		// Check for errors in the queue
		if (errors.empty())
		{
			return {};
		}
		while (errors.size() > 1)
		{
			errors.pop();
		}
		auto err = errors.back();
		err.m_location = this->m_location;
		return std::unexpected{std::move(err)};
	}

private:
	std::source_location m_location;
};
} // namespace moonstone::renderer
