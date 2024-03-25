#ifndef PATH_HPP
#define PATH_HPP

#include <filesystem>

class Path
{
public:
	std::filesystem::path fromRoot(std::vector<std::string>&& segments)
	{
		auto path = root;
		for (const auto segment : segments)
			path /= segment;

		return path;
	}
	static Path& Get()
	{
		static Path path;
		return path;
	}


private:
	Path() 
	{
		auto root = std::filesystem::current_path();
		auto current = root.filename();
		while (current.string().contains("build"))
		{
			root = root.parent_path();
			current = root.filename();
		}

		this->root = root;
	}
	std::filesystem::path root;
};

#endif