/*
* File name: AsyncBufferConnection.cpp
* Author: Katherine
* Date created: 2025-12-18 19:04:13
// Date modified: 2025-12-20 22:38:12
* ===============
*/
module;

#include <concepts>
#include <cstddef>
#include <optional>
#include <vector>

export module moonstone:async_buffer_connection;

import :async_buffer_lock;

namespace moonstone::renderer
{
// What a mouthful...
using changes_iter = std::vector<std::pair<std::size_t, std::size_t>>::iterator;
} // namespace moonstone::renderer

export namespace moonstone::renderer
{
template <typename T>
concept is_async_buffer = requires(T v) {
	{ v.is_locked() } -> std::same_as<bool>;
	{ v.changes_end() } -> std::same_as<changes_iter>;
};
template <is_async_buffer T> class buffer_connection
{
	std::size_t m_id;
	std::optional<std::size_t> m_buffer_element{std::nullopt};
	changes_iter m_changes_iter;
	T& m_buffer;

	void update_connection()
	{
		if (!this->m_buffer_element.has_value())
		{
			return;
		}
		while (this->m_changes_iter != this->m_buffer.end())
		{
			++this->m_changes_iter;
			auto [old_index, new_index] = *this->m_changes_iter;
			if (this->m_buffer_element.value() == old_index)
			{
				this->m_buffer_element.emplace(new_index);
			}
		}
	}

public:
	explicit constexpr buffer_connection(std::size_t id, T& buffer,
										 changes_iter iter)
		: m_id(id), m_buffer(buffer), m_changes_iter(iter)
	{
	}

	buffer_lock<buffer_connection<T>> lock()
	{
		return buffer_lock<buffer_connection<T>>(this);
	}
};
} // namespace moonstone::renderer
