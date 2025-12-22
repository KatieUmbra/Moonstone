/*
* File name: SynchronizedBufferConnection.cpp
* Author: Katherine
* Date created: 2025-12-20 23:09:36
// Date modified: 2025-12-21 18:21:34
* ===============
*/

module;

#include <array>
#include <concepts>
#include <cstddef>
#include <vector>

export module moonstone:sync_buffer_connection;

namespace moonstone::renderer
{
// What a mouthful...
using changes_iter = std::vector<std::pair<std::size_t, std::size_t>>::iterator;
} // namespace moonstone::renderer

export namespace moonstone::renderer
{
template <typename Tm, typename Tt, std::size_t N>
concept is_async_buffer =
	requires(Tm v, std::size_t i, const std::array<Tt, N>& d) {
		{ v.is_locked() } -> std::same_as<bool>;
		{ v.changes_end() } -> std::same_as<changes_iter>;
		{ v.lock_handler(i) };
		{ v.unlock_handler(i) };
		{ v.update(i, d) };
		{ v.erase(i) };
	};
template <typename Tm, typename Tt, std::size_t N> class buffer_connection
{
	std::size_t m_handle_id;
	std::size_t m_buffer_position;
	std::size_t m_key;
	changes_iter m_changes_iter;
	Tm& m_buffer;

	void update_connection()
	{
		while (this->m_changes_iter != this->m_buffer.changes_end())
		{
			++this->m_changes_iter;
			auto [old_index, new_index] = *this->m_changes_iter;
			if (this->m_buffer_position == old_index)
			{
				this->m_buffer_position = new_index;
			}
		}
	}

	void erase()
	{
		this->update_connection();
		this->m_buffer.erase(this->m_buffer_position);
	}

public:
	explicit constexpr buffer_connection(std::size_t handle_id,
										 std::size_t buffer_position,
										 std::size_t key, Tm& buffer,
										 changes_iter iter)
		: m_handle_id(handle_id), m_buffer(buffer), m_changes_iter(iter),
		  m_key(key), m_buffer_position(buffer_position)
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
		this->m_buffer.lock_handler(this->m_handle_id);
		this->m_buffer.update(this->m_key, data);
		this->m_buffer.unlock_handler(this->m_handle_id);
	}

	buffer_connection(const buffer_connection&) = delete;
	buffer_connection(buffer_connection&&) = default;
	buffer_connection& operator=(const buffer_connection&) = delete;
	buffer_connection& operator=(buffer_connection&&) = delete;
};
} // namespace moonstone::renderer
