#pragma once

#include <memory>
#include <optional>

#include "Image.h"
#include "Vulkan.h"
#include "Program.h"

class Application
{
private:
	struct LoadedFilter
	{
		int index;
		std::string name;
		size_t parametersSize = 0;
		std::shared_ptr<void> parameters = nullptr;
		std::optional<std::string> alias = std::nullopt;
	};

	struct LoadedImage
	{
		const Vulkan& vk;
		Image image;
		std::string path;
		VkBuffer bufferFrom, bufferTo;
		VkDeviceMemory memoryFrom, memoryTo;
		std::vector<LoadedFilter> filters;
		std::optional<std::string> alias = std::nullopt;

		auto FindFilter(const std::string& id);

		void ApplyFilters();

		// LoadedImage(const Vulkan& vk, const char* path);
		~LoadedImage();
	};

	// struct LoadedFilter
	// {
	// 	int index;
	// 	size_t parametersSize = 0;
	// 	std::shared_ptr<void> parameters = nullptr;
	// };

	// struct LoadedImage
	// {
	// 	const Vulkan& vk;
	// 	Image image;
	// 	VkBuffer bufferFrom, bufferTo;
	// 	VkDeviceMemory memoryFrom, memoryTo;
	// 	std::vector<LoadedFilter> filters;

	// 	inline LoadedImage(const Vulkan& vk, const char* path) : vk(vk), image(path) {}

	// 	inline ~LoadedImage()
	// 	{
	// 		vk.Destroy(memoryFrom);
	// 		vk.Destroy(bufferFrom);
	// 		vk.Destroy(memoryTo);
	// 		vk.Destroy(bufferTo);
	// 	}
	// };

	const Vulkan& vk;
	std::vector<LoadedImage> loadedImages;
	bool isRunning = false;

	auto FindImage(const std::string& id);

	int Help(const ProgramArguments& args);
	int Load(const ProgramArguments& args);
	int AddFilter(const ProgramArguments& args);
	int RemoveFilter(const ProgramArguments& args);
	int ShowFilters(const ProgramArguments& args);
	int ShowAllFilters(const ProgramArguments& args);
	int Run(const ProgramArguments& args);
	int RunAll(const ProgramArguments& args);
	int Save(const ProgramArguments& args);
	int Quit(const ProgramArguments& args);

public:
	bool IsRunning() const;
	void Run();

	Application(const Vulkan& vk);
};
