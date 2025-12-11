module;

#include <Assert.hpp>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <glad/glad.h>
#include <initializer_list>
#include <print>
#include <ranges>
#include <span>
#include <stb/stb_image.h>
#include <string>
#include <vector>

export module moonstone:texture_array;

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
	std::uint32_t m_renderer_id;
	std::vector<std::array<texel, W * H>> m_image_buffers{};

public:
	texture_array(std::initializer_list<const char*> texture_paths =
					  std::initializer_list<const char*>{})
		: m_renderer_id(0), m_image_buffers(texture_paths.size())
	{
		stbi_set_flip_vertically_on_load_thread(true);
		for (const auto& [texture_path, element] :
			 std::views::zip(texture_paths, this->m_image_buffers))
		{
			const auto* root_path = _ROOTDIR "/assets/";
			std::string path{root_path};
			path.append(texture_path);
			int width, height, channels;
			stbi_uc* texels = stbi_load(path.c_str(), &width, &height,
										&channels, STBI_rgb_alpha);
			/*
			ASSERT(width == W)
			ASSERT(height == H)
			ASSERT(channels == s_pixel_size)
			*/
			if (texels == nullptr)
			{
				std::println("Failed to load texture file {}", texture_path);
				throw std::exception{};
			}
			std::span<texel, (W * H)> texels_span{
				reinterpret_cast<texel*>(texels), W * H};
			std::copy(texels_span.begin(), texels_span.end(), element.begin());
			stbi_image_free(static_cast<void*>(texels));
		}
		GL_CALL(glActiveTexture);
		GL_CALL(glGenTextures(1, &this->m_renderer_id));
		GL_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, this->m_renderer_id));
		// Allocate the storage.
		GL_CALL(glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, W, H,
							   texture_paths.size()));
		// Upload pixel data.
		// The first 0 refers to the mipmap level (level 0, since there's only
		// 1) The following 2 zeroes refers to the x and y offsets in case you
		// only want to specify a subrectangle. The final 0 refers to the layer
		// index offset (we start from index 0 and have 2 levels). Altogether
		// you can specify a 3D box subset of the overall texture, but only one
		// mip level at a time.
		GL_CALL(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, W, H,
								texture_paths.size(), GL_RGBA, GL_UNSIGNED_BYTE,
								this->m_image_buffers.data()));
		// Always set reasonable texture parameters
		GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,
								GL_LINEAR));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER,
								GL_LINEAR));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S,
								GL_CLAMP_TO_EDGE));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T,
								GL_CLAMP_TO_EDGE));
		GL_CALL(glGenerateMipmap(GL_TEXTURE_2D_ARRAY));
	}
	~texture_array()
	{
		GL_CALL(glDeleteTextures(1, &this->m_renderer_id));
	}

	void bind(std::uint32_t slot = 1) const
	{
		GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, this->m_renderer_id));
	}
	static void unbind()
	{
		GL_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
	}
};
} // namespace moonstone::renderer
