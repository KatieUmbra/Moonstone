/*
* File name: SynchronizedBuffer.cpp
* Author: Katherine
* Date created: 2025-12-20 23:09:46
// Date modified: 2025-12-29 15:29:18
* ===============
*/

module;

#include <algorithm>
#include <array>
#include <cstddef>
#include <flat_map>
#include <iterator>
#include <numeric>
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

template <typename T, std::size_t N>
class synchronized_buffer
{
	std::flat_map<std::size_t, std::array<T, N>> m_buffer{};
	std::vector<moonstone::renderer::lock_status> m_locks;
	std::size_t m_last_key{0};
	bool m_locked{false};

	struct inner_buffer_scoped_lock
	{
		synchronized_buffer<T, N>& m_element;
		explicit inner_buffer_scoped_lock(synchronized_buffer<T, N>& element) :
			m_element{element}
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

		this->m_buffer.emplace(this->m_last_key, std::array<T, N>{});
		std::size_t buffer_position = this->m_buffer.size() - 1;

		auto handle_id = this->m_locks.size() - 1;
		buffer_connection<synchronized_buffer<T, N>, T, N> connection{
			handle_id, buffer_position, this->m_last_key, *this};
		++this->m_last_key;
		return connection;
	}

	std::tuple<const T*, std::size_t, inner_buffer_scoped_lock&&> read()
	{
		// This one simply creates a lock that's passed down to the
		// connection so if you wanna read the buffer nothing touches it while
		// it's copied in opengl
		inner_buffer_scoped_lock lock(*this);
		return std::tuple(this->m_buffer.values().data()->data(),
						  this->m_buffer.size() * N * sizeof(T),
						  std::move(lock));
	}

	void erase(std::size_t key)
	{
		inner_buffer_scoped_lock lock(*this);
		this->m_buffer.erase(key);
	}

	void update(std::size_t key, const std::array<T, N>& data)
	{
		this->m_buffer.at(key) = data;
	}

	constexpr void lock_handler(std::size_t key)
	{
		this->m_locks.at(key) = lock_status::locked;
	}

	constexpr void unlock_handler(std::size_t key)
	{
		this->m_locks.at(key) = lock_status::unlocked;
	}

	constexpr bool is_locked()
	{
		return this->m_locked;
	}

	constexpr std::size_t calc_position(std::size_t key)
	{
		auto iterator = this->m_buffer.find(key);
		if (iterator != this->m_buffer.end())
		{
			auto index = std::distance(this->m_buffer.begin(), iterator);
			return index;
		}
		return {};
	}
};
} // namespace moonstone::renderer
