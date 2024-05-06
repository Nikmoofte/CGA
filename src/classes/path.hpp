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
	static Path& get()
	{
		static Path path;
		return path;
	}


private:
	Path() 
	{
		auto root = std::filesystem::current_path();
		auto current = root.string();
		while (current.contains("build"))
		{
			root = root.parent_path();
			current = root.string();
		}

		this->root = root;
	}
	std::filesystem::path root;
};

#endif