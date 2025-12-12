module;

#include "Try.hpp"
#include <cstddef>
#include <cstdint>
#include <glad/glad.h>
#include <initializer_list>
#include <print>
#include <ranges>
#include <span>
#include <stb/stb_image.h>

export module moonstone:texture_array;

import :call;
import :error;

namespace moonstone::renderer
{
struct texel
{
	std::byte r;
	std::byte g;
	std::byte b;
	std::byte a;
} __attribute__((aligned(4), packed));
} // namespace moonstone::renderer

export namespace moonstone::renderer
{
template <std::size_t W, std::size_t H> class texture_array
{
	static constexpr std::int32_t s_pixel_size = 4;
	std::uint32_t m_renderer_id{};
	std::vector<std::array<texel, W * H>> m_image_buffers{};

	error::result<> create(std::size_t size)
	{
		Try(gl().call(glActiveTexture, GL_TEXTURE0));
		Try(gl().call(glGenTextures, 1, &this->m_renderer_id));
		Try(gl().call(glGenTextures, 1, &this->m_renderer_id));
		Try(gl().call(glBindTexture, GL_TEXTURE_2D_ARRAY, this->m_renderer_id));
		// Allocate the storage.
		Try(gl().call(glTexStorage3D, GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, W, H,
					  size));
		// Upload pixel data.
		// The first 0 refers to the mipmap level (level 0, since there's only
		// 1) The following 2 zeroes refers to the x and y offsets in case you
		// only want to specify a subrectangle. The final 0 refers to the layer
		// index offset (we start from index 0 and have 2 levels). Altogether
		// you can specify a 3D box subset of the overall texture, but only one
		// mip level at a time.
		Try(gl().call(glTexSubImage3D, GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, W, H,
					  size, GL_RGBA, GL_UNSIGNED_BYTE,
					  this->m_image_buffers.data()));
		// Always set reasonable texture parameters
		Try(gl().call(glTexParameteri, GL_TEXTURE_2D_ARRAY,
					  GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		Try(gl().call(glTexParameteri, GL_TEXTURE_2D_ARRAY,
					  GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		Try(gl().call(glTexParameteri, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S,
					  GL_CLAMP_TO_EDGE));
		Try(gl().call(glTexParameteri, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T,
					  GL_CLAMP_TO_EDGE));
		Try(gl().call(glGenerateMipmap, GL_TEXTURE_2D_ARRAY));
		return {};
	}

public:
	texture_array(std::initializer_list<const char*> texture_paths =
					  std::initializer_list<const char*>{})
		: m_image_buffers(texture_paths.size())
	{
		stbi_set_flip_vertically_on_load_thread(1);
		for (const auto& [texture_path, element] :
			 std::views::zip(texture_paths, this->m_image_buffers))
		{
			const auto* root_path = _ROOTDIR "/assets/";
			std::string path{root_path};
			path.append(texture_path);
			int width = 0;
			int height = 0;
			int channels = 0;
			stbi_uc* texels = stbi_load(path.c_str(), &width, &height,
										&channels, STBI_rgb_alpha);
			/*
			ASSERT(width == W)
			ASSERT(height == H)
			ASSERT(channels == s_pixel_size)
			*/
			if (texels == nullptr)
			{
				throw std::runtime_error{std::format(
					"Failed to load texture file {}", texture_path)};
			}
			std::span<texel, (W * H)> texels_span{
				reinterpret_cast<texel*>(texels), W * H};
			std::copy(texels_span.begin(), texels_span.end(), element.begin());
			stbi_image_free(static_cast<void*>(texels));
			auto err = this->create(texture_paths.size());
			if (!err.has_value())
			{
				throw std::runtime_error(err.error().format().c_str());
			}
		}
	}
	~texture_array()
	{
		auto err = gl().call(glDeleteTextures, 1, &this->m_renderer_id);
#ifdef _DEBUG
		if (!err.has_value())
		{
			std::println("{}", err.error().format());
			__builtin_trap();
		}
#endif
	}

	[[nodiscard]] error::result<> bind(std::uint32_t slot = 1) const
	{
		Try(gl().call(glActiveTexture, GL_TEXTURE0 + slot));
		Try(gl().call(glBindTexture, GL_TEXTURE_2D_ARRAY, this->m_renderer_id));
		return {};
	}
	static error::result<> unbind()
	{
		Try(gl().call(glBindTexture, GL_TEXTURE_2D_ARRAY, 0));
		return {};
	}
	texture_array(const texture_array&) = delete;
	texture_array(texture_array&&) = delete;
	texture_array& operator=(const texture_array&) = delete;
	texture_array& operator=(texture_array&&) = delete;
};
} // namespace moonstone::renderer
