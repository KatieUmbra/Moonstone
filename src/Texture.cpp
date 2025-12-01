module;

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "Assert.hpp"
#include "glad/glad.h"

export module moonstone:texture;

import :utility;

export namespace moonstone
{
class texture
{
	unsigned int m_renderer_id;
	unsigned char* m_local_buffer;
	std::string m_file_path;
	int m_width, m_height, m_pixel_size;

public:
	texture(const std::string& path)
		: m_renderer_id{0}, m_file_path{path}, m_local_buffer{nullptr}, m_width{0}, m_height{0}, m_pixel_size{0}
	{
		stbi_set_flip_vertically_on_load(1);
		auto real_path = _SOLUTIONDIR "/assets/" + path;
		m_local_buffer = stbi_load(real_path.c_str(), &this->m_width, &this->m_height, &this->m_pixel_size, 4);

		GL_CALL(glGenTextures(1, &this->m_renderer_id));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_renderer_id));

		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		GL_CALL(
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_local_buffer));
		texture::unbind();

		if (m_local_buffer)
		{
			stbi_image_free(m_local_buffer);
		}
	}
	~texture()
	{

		GL_CALL(glDeleteTextures(1, &m_renderer_id));
	}

	void bind(unsigned int slot = 0) const
	{
		GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_renderer_id));
	}
	static void unbind()
	{
		GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	}

	[[nodiscard]] auto get_width() const -> int
	{
		return this->m_width;
	}
	[[nodiscard]] auto get_height() const -> int
	{
		return this->m_height;
	}
};
} // namespace moonstone
