module;

#include "Try.hpp"
#include "glad/glad.h"
#include <exception>
#include <format>
#include <print>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>

export module moonstone:texture;

import :utility;
import :error;
import :call;

export namespace moonstone::renderer
{
class texture
{
	std::uint32_t m_renderer_id{};
	unsigned char* m_local_buffer{};
	std::string m_file_path;
	std::int32_t m_width{}, m_height{}, m_pixel_size{};

	moonstone::error::result<> create()
	{
		Try(gl().call(glGenTextures, 1, &this->m_renderer_id));
		Try(gl().call(glBindTexture, GL_TEXTURE_2D, this->m_renderer_id));
		Try(gl().call(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					  GL_LINEAR));
		Try(gl().call(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
					  GL_LINEAR));
		Try(gl().call(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
					  GL_CLAMP_TO_EDGE));
		Try(gl().call(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
					  GL_CLAMP_TO_EDGE));
		Try(gl().call(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA8, this->m_width,
					  this->m_height, 0, GL_RGBA8, GL_UNSIGNED_BYTE,
					  this->m_local_buffer));
		Try(texture::unbind());
		return {};
	}

public:
	explicit texture(const std::string& path) : m_file_path{path}
	{
		stbi_set_flip_vertically_on_load(1);
		auto real_path = std::format("{}{}", _ROOTDIR "/assets/", path);

		m_local_buffer = stbi_load(real_path.c_str(), &this->m_width,
								   &this->m_height, &this->m_pixel_size, 4);

		auto err = this->create();
		if (!err.has_value())
		{
			throw std::runtime_error(err.error().format());
		}

		if (m_local_buffer != nullptr)
		{
			stbi_image_free(m_local_buffer);
		}
	}
	~texture()
#ifdef _DEBUG
	{
		auto err = gl().call(glDeleteTextures, 1, &this->m_renderer_id);
		if (!err.has_value())
		{
			std::println(stderr, "{}", err.error().format());
			std::terminate();
		}
	}
#else
	{
		gl().call(glDeleteTextures, 1, &this->m_renderer_id);
	}
#endif
	[[nodiscard]] error::result<> bind(std::uint32_t slot = 0) const
	{
		Try(gl().call(glActiveTexture, GL_TEXTURE0 + slot));
		Try(gl().call(glBindTexture, GL_TEXTURE_2D, this->m_renderer_id));
		return {};
	}
	static error::result<> unbind()
	{
		Try(gl().call(glBindTexture, GL_TEXTURE_2D, 0));
		return {};
	}

	[[nodiscard]] std::int32_t get_width() const
	{
		return this->m_width;
	}
	[[nodiscard]] std::int32_t get_height() const
	{
		return this->m_height;
	}
};
} // namespace moonstone::renderer
