module;

#include <expected>
#include <queue>
#include <source_location>
#include <utility>
#include <variant>

export module moonstone:call;

import :error;

export namespace moonstone::renderer
{
// https://www.cppstories.com/2021/non-terminal-variadic-args/
struct gl
{
#ifdef _DEBUG
	explicit gl(std::source_location l = std::source_location::current())
		: m_location(l)
	{
		std::string_view file{l.file_name()};
		std::string new_file{file.begin() + file.rfind('/'), file.end()};
		this->m_short_file = std::move(new_file);
	}
	template <typename T, typename F, typename... Args>
	std::expected<T, error::gl_error> call_returning(F f, Args... args)
	{
		// Empty the queue
		std::queue<error::gl_error> empty{};
		std::swap(error::errors, empty);
		// Call opengl function
		T returned = f(args...);
		// Check for errors in the queue
		if (error::errors.empty())
		{
			return returned;
		}
		while (error::errors.size() > 1)
		{
			error::errors.pop();
		}
		auto err = error::errors.back();
		err.set_location(this->m_location);
		err.set_file(this->m_short_file);
		return std::unexpected{const_cast<const error::gl_error&>(err)};
	}
	template <typename F, typename... Args>
	std::expected<std::monostate, error::gl_error> call(F f, Args... args)
	{
		// Empty the queue
		std::queue<error::gl_error> empty{};
		std::swap(error::errors, empty);
		// Call opengl function
		f(args...);
		// Check for errors in the queue
		if (error::errors.empty())
		{
			return {};
		}
		while (error::errors.size() > 1)
		{
			error::errors.pop();
		}
		auto err = error::errors.back();
		err.set_location(this->m_location);
		return std::unexpected{std::move(err)};
	}

private:
	std::source_location m_location;
	std::string m_short_file;
#else
	explicit gl() = default;
	template <typename F, typename... Args>
	inline std::expected<void, error::gl_error> call(F f, Args... args)
	{
		f(args...);
		return {};
	}
	template <typename T, typename F, typename... Args>
	inline std::expected<T, error::gl_error> call_returning(F f, Args... args)
	{
		return f(args...);
	}
#endif
};
} // namespace moonstone::renderer
