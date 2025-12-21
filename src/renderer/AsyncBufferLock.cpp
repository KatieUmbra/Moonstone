/*
* File name: AsyncBufferLock.cpp
* Author: Katherine
* Date created: 2025-12-18 19:05:16
// Date modified: 2025-12-18 19:09:33
* ===============
*/

module;

export module moonstone:async_buffer_lock;

namespace moonstone::renderer
{
template <typename T> class buffer_lock
{
	T& m_connection;

public:
	explicit buffer_lock(T& m_connection) : m_connection(m_connection)
	{
	}
};
} // namespace moonstone::renderer
