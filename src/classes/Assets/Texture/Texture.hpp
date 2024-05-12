#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <vector>
#include <future>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace Assets
{
    class Texture
    {
    public:
		Texture(const std::string& path);

		Texture(const Texture&) = delete;
		Texture(Texture&&) noexcept;

		Texture& operator =(Texture&&) noexcept;
		Texture& operator =(const Texture&) = delete;

		~Texture() = default;

		void Wait() const;

		int GetWidth() const { return width; };
		int GetHeight() const { return height; };
		int GetImageSize() const { return imageSize; };
		int GetChannels() const { return channels; };
		virtual glm::vec4 GetPixel(glm::vec2 texCoords) const;
		const std::vector<float>& GetPixels() const { return pixels; };
	protected:
		Texture();
	private:
		std::future<void> loader{};
		std::string path;
		std::vector<float> pixels;
		int width{};
		int height{};
		int channels{};
		int imageSize;
    };
}

#endif