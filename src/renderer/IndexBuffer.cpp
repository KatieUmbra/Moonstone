module;

#include "Assert.hpp"
#include "glad/glad.h"
#include <flat_map>
#include <print>
#include <ranges>
#include <stdexcept>
#include <tuple>

export module moonstone:index_buffer;

export namespace moonstone::renderer
{
class index_buffer
{
	std::uint32_t m_renderer_id{};
	std::uint32_t m_index_count{0};
	std::uint32_t m_highest{0};
	std::flat_map<std::uint32_t, std::uint32_t> m_indices;

public:
	index_buffer()
	{
		GL_CALL(glGenBuffers(1, &this->m_renderer_id));
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_renderer_id));
		GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
							 this->m_indices.size() * sizeof(std::uint32_t),
							 this->m_indices.values().data(), GL_DYNAMIC_DRAW));
	}

	~index_buffer()
	{
		GL_CALL(glDeleteBuffers(1, &this->m_renderer_id));
	}

	template <std::size_t N>
	std::array<std::uint32_t, N> insert(std::array<std::uint32_t, N> indices)
	{
		std::array<std::uint32_t, N> returned{};
		std::ranges::for_each(
			std::views::zip(indices, returned),
			[this](std::tuple<std::uint32_t, std::uint32_t&> values) {
				auto [index, returned] = values;
				this->m_indices.insert({this->m_index_count, index});
				if (index > this->m_highest)
				{
					this->m_highest = index;
				}
				returned = this->m_index_count++;
			});
		this->update();
		return returned;
	}

	void replace(std::uint32_t index_id, std::uint32_t index)
	{
		try
		{
			std::uint32_t& index_ref = this->m_indices.at(index_id);
			index_ref = index;
		}
		catch (const std::out_of_range& e)
		{
			std::println("Oops! called index: {}", index_id);
		}
	}

	void update()
	{
		this->bind();
		std::int64_t old_size = 0;
		GL_CALL(glGetBufferParameteri64v(GL_ELEMENT_ARRAY_BUFFER,
										 GL_BUFFER_SIZE, &old_size));
		auto size = this->m_indices.size() * sizeof(std::uint32_t);
		if (size > old_size)
		{
			GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size,
								 this->m_indices.values().data(),
								 GL_DYNAMIC_DRAW));
		}
		else
		{
			GL_CALL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size,
									this->m_indices.values().data()));
		}
	}

	void bind() const
	{
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_renderer_id));
	}

	static void unbind()
	{
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
	std::uint32_t get_size()
	{
		return this->m_indices.size();
	}
	std::uint32_t get_highest()
	{
		return this->m_highest;
	}
	index_buffer(const index_buffer&) = default;
	index_buffer(index_buffer&&) = delete;
	index_buffer& operator=(const index_buffer&) = default;
	index_buffer& operator=(index_buffer&&) = delete;
};
} // namespace moonstone::renderer
