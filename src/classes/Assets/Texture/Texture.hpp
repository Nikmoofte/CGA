#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <vector>
#include <future>

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

		~Texture();

		void Wait();

		int GetWidth() const { return width; };
		int GetHeight() const { return height; };
		int GetImageSize() const { return imageSize; };
		int GetChannels() const { return channels; };
		const std::vector<float>& GetPixels() const { return pixels; };

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