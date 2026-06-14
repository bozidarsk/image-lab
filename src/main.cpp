#include <print>
#include <vulkan/vulkan_core.h>

#include "Vulkan.h"
#include "Application.h"

int main(int argc, char** argv)
{
	Vulkan vk;
	vk.SetDebugMessageCallback([](VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagBitsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data)
		{
			std::println("{} {}", data->pMessageIdName, data->pMessage);
		}
	);

	Application app(vk);
	app.Run();
}
