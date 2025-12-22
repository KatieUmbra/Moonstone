/*
* File name: SynchronizedBuffer.cpp
* Author: Katherine
* Date created: 2025-12-20 23:09:46
// Date modified: 2025-12-22 16:53:00
* ===============
*/

module;

#include <algorithm>
#include <array>
#include <cstddef>
#include <exception>
#include <execution>
#include <numeric>
#include <print>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

export module moonstone:sync_buffer;

import :sync_buffer_connection;

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
	constexpr explicit lock_status(Value status) : m_value(status)
	{
	}
	constexpr explicit lock_status() : m_value(Value::unlocked)
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
	constexpr lock_status& operator=(Value a)
	{
		this->m_value = a;
		return *this;
	}
	constexpr bool operator!=(lock_status a) const
	{
		return this->m_value != a.m_value;
	}
	constexpr bool is_locked()
	{
		return this->m_value == Value::locked;
	}
	constexpr lock_status operator+(lock_status rhs) const
	{
		if (this->m_value == Value::locked || rhs.m_value == Value::locked)
		{
			return lock_status{Value::locked};
		}
		return lock_status{Value::unlocked};
	}

private:
	Value m_value;
};

template <typename T, std::size_t N> class synchronized_buffer
{
	std::vector<std::array<T, N>> m_buffer{};
	std::vector<moonstone::renderer::lock_status> m_locks;
	// First: Old index, Second: New index
	std::vector<std::pair<std::size_t, std::size_t>> m_changes;
	bool m_locked{false};

	struct inner_buffer_scoped_lock
	{
		synchronized_buffer<T, N>& m_element;
		explicit inner_buffer_scoped_lock(synchronized_buffer<T, N>& element)
			: m_element{element}
		{
			while (element.m_locked)
			{
				// Hang until lock isn't being used
			}
			element.m_locked = true;
			while (true)
			{
				// Hang until handlers finish all their tasks
				auto is_free = std::reduce(element.m_locks.cbegin(),
										   element.m_locks.cend());
				if (is_free == lock_status::unlocked)
				{
					break;
				}
			}
		}
		~inner_buffer_scoped_lock()
		{
			this->m_element.m_locked = false;
		}

		inner_buffer_scoped_lock(const inner_buffer_scoped_lock&) = delete;
		inner_buffer_scoped_lock(inner_buffer_scoped_lock&&) = default;
		inner_buffer_scoped_lock& operator=(const inner_buffer_scoped_lock&) =
			delete;
		inner_buffer_scoped_lock& operator=(inner_buffer_scoped_lock&&) =
			default;
	};

public:
	synchronized_buffer<T, N>() = default;
	buffer_connection<synchronized_buffer<T, N>, T, N> connect()
	{
		inner_buffer_scoped_lock lock(*this);
		this->m_locks.emplace_back(lock_status::unlocked);

		this->m_buffer.emplace_back(std::array<T, N>{});
		std::size_t position = this->m_buffer.size() - 1;

		std::size_t last_change = 0;
		if (this->m_changes.size() > 0)
		{
			last_change = this->m_changes.size() - 1;
		}

		buffer_connection<synchronized_buffer<T, N>, T, N> connection{
			this->m_locks.size() - 1, position, *this, last_change};
		return connection;
	}

	std::tuple<T*, std::size_t, inner_buffer_scoped_lock&&> read()
	{
		// This one simply creates a lock that's passed down to the
		// connection so if you wanna read the buffer nothing touches it while
		// it's copied in opengl
		inner_buffer_scoped_lock lock(*this);
		return std::tuple(this->m_buffer.data()->data(),
						  this->m_buffer.size() * N * sizeof(T),
						  std::move(lock));
	}

	void erase(std::size_t index)
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
		inner_buffer_scoped_lock lock(*this);
		auto last = --this->m_buffer.end();
		auto current = this->m_buffer.begin() + index;
		std::iter_swap(last, current);
		auto old_index = this->m_buffer.size() - 1;
		this->m_buffer.erase(last);
		this->m_changes.emplace_back(std::make_pair(old_index, index));
	}

	void update(std::size_t index, const std::array<T, N>& data)
	{
		this->m_buffer.at(index) = data;
	}

	constexpr void lock_handler(std::size_t id)
	{
		this->m_locks.at(id) = lock_status::locked;
	}

	constexpr void unlock_handler(std::size_t id)
	{
		this->m_locks.at(id) = lock_status::unlocked;
	}

	constexpr bool is_locked()
	{
		return this->m_locked;
	}

	constexpr std::size_t changes_last_index()
	{
		std::size_t last_change = 0;
		if (this->m_changes.size() > 0)
		{
			last_change = this->m_changes.size() - 1;
		}
		return last_change;
	}

	constexpr std::pair<std::size_t, std::size_t> get_change(std::size_t index)
	{
		try
		{
			return this->m_changes.at(index);
		}
		catch (const std::out_of_range& e)
		{
			std::println(stderr, "{}, Index requested: {}, Size of vector: {}",
						 "You fucked up, this shouldn't be here", index,
						 this->m_changes.size());
			std::terminate();
		}
	}
};
} // namespace moonstone::renderer
