module;

#include "Try.hpp"
#include "glad/glad.h"
#include <algorithm>
#include <flat_map>
#include <print>
#include <ranges>
#include <stdexcept>
#include <tuple>

export module moonstone:index_buffer;

import :error;
import :call;

export namespace moonstone::renderer
{
class index_buffer
{
	std::uint32_t m_renderer_id{};
	std::uint32_t m_index_count{0};
	std::uint32_t m_highest{0};
	std::flat_map<std::uint32_t, std::uint32_t> m_indices;

	error::result<> create()
	{
		Try(gl().call(glGenBuffers, 1, &this->m_renderer_id));
		Try(gl().call(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER,
					  this->m_renderer_id));
		Try(gl().call(glBufferData, GL_ELEMENT_ARRAY_BUFFER,
					  this->m_indices.size() & sizeof(std::uint32_t),
					  this->m_indices.values().data(), GL_DYNAMIC_DRAW));
		return {};
	}

public:
	index_buffer()
	{
		auto err = this->create();
		if (!err.has_value())
		{
			throw std::runtime_error(err.error().format());
		}
	}

	~index_buffer()
	{
		auto err = gl().call(glDeleteBuffers, 1, &this->m_renderer_id);
		if (!err.has_value())
		{
			throw std::runtime_error(err.error().format());
		}
	}

	template <std::size_t N>
	error::result<std::array<std::uint32_t, N>> insert(
		std::array<std::uint32_t, N> indices)
	{
		std::array<std::uint32_t, N> returned{};
		std::ranges::for_each(
			std::views::zip(indices, returned),
			[this](std::tuple<std::uint32_t, std::uint32_t&> values) {
				auto [index, returned] = values;
				this->m_indices.insert({this->m_index_count, index});
				this->m_highest = std::max(index, this->m_highest);
				returned = this->m_index_count++;
			});
		Try(this->update());
		return std::move(returned);
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

	error::result<> update()
	{
		Try(this->bind());
		std::int64_t old_size = 0;
		Try(gl().call(glGetBufferParameteri64v, GL_ELEMENT_ARRAY_BUFFER,
					  GL_BUFFER_SIZE, &old_size));
		auto size = this->m_indices.size() * sizeof(std::uint32_t);
		if (size > old_size)
		{
			Try(gl().call(glBufferData, GL_ELEMENT_ARRAY_BUFFER, size,
						  this->m_indices.values().data(), GL_DYNAMIC_DRAW));
		}
		else
		{
			Try(gl().call(glBufferSubData, GL_ELEMENT_ARRAY_BUFFER, 0, size,
						  this->m_indices.values().data()));
		}
		return {};
	}

	[[nodiscard]] error::result<> bind() const
	{
		Try(gl().call(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER,
					  this->m_renderer_id));
		return {};
	}

	static error::result<> unbind()
	{
		Try(gl().call(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0));
		return {};
	}
	std::uint32_t get_size()
	{
		return this->m_indices.size();
	}
	[[nodiscard]] std::uint32_t get_highest() const
	{
		return this->m_highest;
	}
	index_buffer(const index_buffer&) = default;
	index_buffer(index_buffer&&) = delete;
	index_buffer& operator=(const index_buffer&) = default;
	index_buffer& operator=(index_buffer&&) = delete;
};
} // namespace moonstone::renderer
