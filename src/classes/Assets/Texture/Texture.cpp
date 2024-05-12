#include "Texture.hpp"

#include "stb_image.h"
#include <glm/glm.hpp>

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
		pixels = std::move(texture.pixels);
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
			pixels = std::move(texture.pixels);
		}

		return *this;
	}


	void Texture::Wait() const
	{
		if (loader.valid())
			loader.wait();
	}

    glm::vec4 Texture::GetPixel(glm::vec2 texCoords) const
    {
		Wait();
		while(texCoords.x < 0)
			texCoords.x += 1.0;
		while(texCoords.y < 0)
			texCoords.y += 1.0;
		while(texCoords.x > 1)
			texCoords.x -= 1.0;
		while(texCoords.y > 1)
			texCoords.y -= 1.0;

		int x = texCoords.x * width;
		int y = texCoords.y * height;

		int index = (y * width + x) * 4;

		return { pixels[index + 0], pixels[index + 1], pixels[index + 2], pixels[index + 3] };
    }
    Texture::Texture()
    {

    }
}