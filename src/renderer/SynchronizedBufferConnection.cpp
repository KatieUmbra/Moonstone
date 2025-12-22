/*
* File name: SynchronizedBufferConnection.cpp
* Author: Katherine
* Date created: 2025-12-20 23:09:36
// Date modified: 2025-12-22 16:52:22
* ===============
*/

module;

#include <array>
#include <concepts>
#include <cstddef>
#include <functional>
#include <vector>

export module moonstone:sync_buffer_connection;

namespace moonstone::renderer
{
} // namespace moonstone::renderer

export namespace moonstone::renderer
{
template <typename Tm, typename Tt, std::size_t N>
concept is_async_buffer =
	requires(Tm v, std::size_t i, const std::array<Tt, N>& d) {
		{ v.is_locked() } -> std::same_as<bool>;
		{ v.changes_last_index() } -> std::same_as<std::size_t>;
		{
			v.get_change(i)
		} -> std::same_as<std::pair<std::size_t, std::size_t>>;
		{ v.lock_handler(i) };
		{ v.unlock_handler(i) };
		{ v.update(i, d) };
		{ v.erase(i) };
	};
template <typename Tm, typename Tt, std::size_t N> class buffer_connection
{
	std::size_t m_handle_id;
	std::size_t m_buffer_position;
	std::size_t m_latest_change;
	std::reference_wrapper<Tm> m_buffer;

	void update_connection()
	{
		if (this->m_buffer.get().changes_last_index() == 0)
		{
			return;
		}
		while (this->m_latest_change <
			   this->m_buffer.get().changes_last_index())
		{
			++this->m_latest_change;
			auto [old_index, new_index] =
				this->m_buffer.get().get_change(this->m_latest_change);
			if (this->m_buffer_position == old_index)
			{
				this->m_buffer_position = new_index;
			}
		}
	}

	void erase()
	{
		this->update_connection();
		this->m_buffer.get().erase(this->m_buffer_position);
	}

public:
	constexpr buffer_connection(std::size_t handle_id,
								std::size_t buffer_position, Tm& buffer,
								std::size_t current_change)
		: m_handle_id(handle_id), m_buffer(buffer),
		  m_latest_change(current_change), m_buffer_position(buffer_position)
	{
		// https://medium.com/@rogerbooth/using-the-crtp-and-c-20-concepts-to-enforce-contracts-for-static-polymorphism-a27d93111a75
		static_assert(is_async_buffer<Tm, Tt, N>);
	}
	~buffer_connection()
	{
		this->erase();
	}

	void update(const std::array<Tt, N>& data)
	{
		this->update_connection();
		this->m_buffer.get().lock_handler(this->m_handle_id);
		this->m_buffer.get().update(this->m_buffer_position, data);
		this->m_buffer.get().unlock_handler(this->m_handle_id);
	}
};
} // namespace moonstone::renderer
