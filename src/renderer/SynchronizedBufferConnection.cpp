/*
* File name: SynchronizedBufferConnection.cpp
* Author: Katherine
* Date created: 2025-12-20 23:09:36
// Date modified: 2025-12-29 15:25:07
* ===============
*/

module;

#include <array>
#include <concepts>
#include <cstddef>
#include <functional>
#include <print>

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
		{ v.lock_handler(i) };
		{ v.unlock_handler(i) };
		{ v.update(i, d) };
		{ v.erase(i) };
		{ v.calc_position(i) } -> std::same_as<std::size_t>;
	};
template <typename Tm, typename Tt, std::size_t N>
class buffer_connection
{
	std::size_t m_handle_id;
	std::size_t m_buffer_position;
	std::size_t m_key;
	std::reference_wrapper<Tm> m_buffer;

	void update_connection()
	{
		this->m_buffer_position =
			this->m_buffer.get().calc_position(this->m_key);
	}

public:
	constexpr buffer_connection(std::size_t handle_id,
								std::size_t buffer_position, std::size_t key,
								Tm& buffer) :
		m_handle_id(handle_id),
		m_buffer(buffer),
		m_key(key),
		m_buffer_position(buffer_position)
	{
		// https://medium.com/@rogerbooth/using-the-crtp-and-c-20-concepts-to-enforce-contracts-for-static-polymorphism-a27d93111a75
		static_assert(is_async_buffer<Tm, Tt, N>);
	}
	~buffer_connection() = default;

	void update(const std::array<Tt, N>& data)
	{
		this->update_connection();
		this->m_buffer.get().lock_handler(this->m_handle_id);
		this->m_buffer.get().update(this->m_key, data);
		this->m_buffer.get().unlock_handler(this->m_handle_id);
	}

	void erase()
	{
		this->update_connection();
		this->m_buffer.get().erase(this->m_buffer_position);
	}
};
} // namespace moonstone::renderer
