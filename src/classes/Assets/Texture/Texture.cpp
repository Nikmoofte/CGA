#include "Texture.hpp"

#include "stb_image.h"

namespace Assets
{
	Texture::Texture(const std::string& path) : path(path)
	{
		loader = std::async(std::launch::async, [this, path]()
		{
			auto* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
			imageSize = height * width * 4;

			if (!data)
			{
				throw std::runtime_error("Failed to load texture image!");
			}

			for (int i = 0; i < imageSize; ++i)
				pixels.push_back(static_cast<float>(data[i]) / 255.f);

			stbi_image_free(data);
		});
	}

	Texture::Texture(Texture&& texture) noexcept
	{
		path = std::move(texture.path);
		height = texture.height;
		width = texture.width;
		channels = texture.channels;
		imageSize = texture.imageSize;
		pixels = texture.pixels;

		texture.pixels.clear();
	}

	Texture& Texture::operator=(Texture&& texture) noexcept
	{
		if (this != &texture)
		{
			path = std::move(texture.path);
			height = texture.height;
			width = texture.width;
			channels = texture.channels;
			imageSize = texture.imageSize;
			pixels = texture.pixels;

			texture.pixels.clear();
		}

		return *this;
	}

	Texture::~Texture() = default;

	void Texture::Wait()
	{
		if (loader.valid())
			loader.get();
	}
}