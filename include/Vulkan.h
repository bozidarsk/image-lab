#pragma once

#include <cstdint>
#include <functional>
#include <vulkan/vulkan_core.h>

class Vulkan
{
private:
	VkAllocationCallbacks* allocator = nullptr;
	VkDebugUtilsMessengerEXT debugUtilsMessenger;
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkCommandPool commandPool;
	VkQueue computeQueue;
	uint32_t computeQueueFamilyIndex;
	bool isRunning = false;

	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = nullptr;
	PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = nullptr;

	std::function<void(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagBitsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data)> debugMessageCallback;

	static inline uint32_t MakeVersion(int major, int minor, int patch) { return ((((uint32_t)major) << 22) | (((uint32_t)minor) << 12) | ((uint32_t)patch)); }
	static inline uint32_t MakeApiVersion(int variant, int major, int minor, int patch) { return ((((uint32_t)variant) << 29) | (((uint32_t)major) << 22) | (((uint32_t)minor) << 12) | ((uint32_t)patch)); }

	static unsigned int OnDebugMessage(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data, void* userData);

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

	void Initialize();
	void InitializeInstance();
	void InitializeExtensions();
	void InitializeDebugMessages();
	void InitializePhysicalDevice();
	void InitializeDevice();
	void InitializeCommandPool();

public:
	void SetDebugMessageCallback(const decltype(debugMessageCallback)& callback);

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer* buffer) const;
	void CreateBufferMemory(VkBuffer buffer, VkMemoryPropertyFlags properties, VkDeviceMemory* memory) const;
	void CreateImage(int width, int height, VkImageType type, VkImageUsageFlags usage, VkFormat format, VkImage* image) const;
	void CreateImageMemory(VkImage image, VkDeviceMemory* memory) const;
	void CopyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize size, VkCommandBuffer cmd = nullptr) const;
	void CopyBufferToImage(VkBuffer buffer, VkImage image, int width, int height, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT, VkCommandBuffer cmd = nullptr) const;
	void CopyImageToBuffer(VkImage image, VkBuffer buffer, int width, int height, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT, VkCommandBuffer cmd = nullptr) const;
	void CreateTexture(void* data, int width, int height, VkImageType type, VkFormat format, VkImage* image, VkImageView* imageView, VkDeviceMemory* memory, VkSampler* sampler) const;
	void CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect, VkImageView* imageView) const;
	void CreateSampler(VkSampler* sampler) const;
	void TransitionImageLayout(
		VkImage image,

		VkImageLayout from,
		VkAccessFlags sourceAccess,
		VkPipelineStageFlags sourceStage,

		VkImageLayout to,
		VkAccessFlags destinationAccess,
		VkPipelineStageFlags destinationStage,

		VkCommandBuffer cmd = nullptr
	) const;
	void TransitionImageLayout(VkImage image, VkImageLayout from, VkImageLayout to, VkCommandBuffer cmd = nullptr) const;
	VkCommandBuffer BeginSingleTimeCommand() const;
	void EndSingleTimeCommand(VkCommandBuffer cmd) const;

	Vulkan& operator = (const Vulkan&) = delete;
	Vulkan& operator = (Vulkan&&) = delete;

	Vulkan(const Vulkan&) = delete;
	Vulkan(Vulkan&&) = delete;

	Vulkan();
	~Vulkan();
};
