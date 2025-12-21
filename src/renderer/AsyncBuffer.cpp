/*
* File name: AsyncBuffer.cpp
* Author: Katherine
* Date created: 2025-12-18 19:06:01
// Date modified: 2025-12-20 22:36:41
* ===============
*/

// TODO: Please for the love of satan, improve the naming in this module

module;

#include <algorithm>
#include <cstddef>
#include <execution>
#include <flat_map>
#include <iterator>
#include <numeric>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

export module moonstone:async_buffer;

import :async_buffer_lock;
import :async_buffer_connection;

export namespace moonstone::renderer
{

class lock_status
{
public:
	enum Value : unsigned char
	{
		locked,
		unlocked
	};
	lock_status() = default;
	constexpr explicit lock_status(Value status) : m_value(status)
	{
	}

	// https://stackoverflow.com/a/53284026
	constexpr explicit operator Value() const
	{
		return this->m_value;
	}
	explicit operator bool() const = delete;
	constexpr bool operator==(lock_status a) const
	{
		return this->m_value == a.m_value;
	}
	constexpr bool operator==(Value a) const
	{
		return this->m_value == a;
	}
	constexpr bool operator!=(lock_status a) const
	{
		return this->m_value != a.m_value;
	}
	constexpr bool is_locked()
	{
		return this->m_value == Value::locked;
	}

private:
	Value m_value;
};

template <typename T, std::size_t N> class async_buffer
{
	std::flat_map<std::size_t, std::array<T, N>> m_buffer{};
	std::vector<moonstone::renderer::lock_status> m_locks;
	std::vector<std::pair<std::size_t, std::size_t>> m_changes;
	bool m_locked{false};

	struct inner_buffer_scoped_lock
	{
		async_buffer<T, N>& m_element;
		explicit inner_buffer_scoped_lock(async_buffer<T, N>& element)
			: m_element{element}
		{
			std::thread buffer_locks_yield([element]() {
				while (true)
				{
					auto is_free = std::reduce(std::execution::par_unseq,
											   element.m_locks.cbegin(),
											   element.m_locks.cend());
					if (is_free == lock_status::unlocked)
					{
						break;
					}
				}
			});
			std::thread unlock_yield([&]() {
				while (!element.m_locked)
					;
				element.m_locked = true;
			});
			unlock_yield.join();
			buffer_locks_yield.join();
		}
		~inner_buffer_scoped_lock()
		{
			this->m_element.m_locked = false;
		}

	private:
		inner_buffer_scoped_lock(const inner_buffer_scoped_lock&) = delete;
		inner_buffer_scoped_lock(inner_buffer_scoped_lock&&) = delete;
		inner_buffer_scoped_lock& operator=(const inner_buffer_scoped_lock&) =
			delete;
		inner_buffer_scoped_lock& operator=(inner_buffer_scoped_lock&&) =
			delete;
	};

public:
	async_buffer<T, N>() = default;
	buffer_connection<async_buffer<T, N>> connect()
	{
		this->m_locks.emplace_back(lock_status::unlocked);
		return buffer_connection(this->m_locks.size() - 1, this);
	}

	std::tuple<T*, std::size_t, inner_buffer_scoped_lock&&> read()
	{
		// This one simply creates a lock that's passed down to the
		// connection so if you wanna read the buffer nothing touches it while
		// it's copied in opengl
		inner_buffer_scoped_lock lock(this, this->m_changes.begin());
		return (this->m_buffer.values().data()->data(),
				this->m_buffer.size() * N * sizeof(T), std::move(lock));
	}

	void erase(std::size_t key)
	{
		// Explaining so my future self knows what the hell I'm doing.
		// so wat I do is create a scoped lock here so it automatically unlocks
		// after the function is done, and then I swap the last element in the
		// buffer with the deleted one (Unless the deleted one is the last, in
		// which case I just delete), I then publish to a vector called
		// "m_changes" so every thread can check if it was their data the one
		// that changed, and if so, the connection checks if it was its index
		// that changed so it will use the new one now.
		// (Also this will help update the index buffers aswell)
		inner_buffer_scoped_lock lock(this);
		auto element = this->m_buffer.find(key);
		if (element == this->m_buffer.end())
		{
			this->m_buffer.erase(element);
			return;
		}
		std::swap(element, this->m_buffer.end());
		auto new_index = std::distance(this->m_buffer.begin(), element);
		auto old_index = this->m_buffer.size() - 1;
		this->m_changes.emplace_back({old_index, new_index});
		this->m_buffer.erase(this->m_buffer.end());
	}

	constexpr bool is_locked()
	{
		return this->m_locked;
	}

	constexpr std::vector<std::pair<std::size_t, std::size_t>>::iterator
	changes_end()
	{
		return this->m_changes.end();
	}
};
} // namespace moonstone::renderer
