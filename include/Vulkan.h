#pragma once

#include <string>
#include <functional>
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

class Vulkan
{
public:
	VkAllocationCallbacks* allocator = nullptr;
	VkDebugUtilsMessengerEXT debugUtilsMessenger;
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
	VkQueue computeQueue;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;
	VkPipelineLayout pipelineLayout;
	std::vector<VkShaderModule> shaderModules;
	std::vector<VkPipeline> pipelines;
	uint32_t computeQueueFamilyIndex;

	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = nullptr;
	PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = nullptr;
	PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR = nullptr;

	std::function<void(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagBitsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data)> debugMessageCallback;

	static inline uint32_t MakeVersion(int major, int minor, int patch) { return ((((uint32_t)major) << 22) | (((uint32_t)minor) << 12) | ((uint32_t)patch)); }
	static inline uint32_t MakeApiVersion(int variant, int major, int minor, int patch) { return ((((uint32_t)variant) << 29) | (((uint32_t)major) << 22) | (((uint32_t)minor) << 12) | ((uint32_t)patch)); }

	static unsigned int OnDebugMessage(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data, void* userData);

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
	VkShaderModule CreateShaderModule(const std::string& path) const;

	void Initialize();
	void InitializeInstance();
	void InitializeExtensions();
	void InitializeDebugMessages();
	void InitializePhysicalDevice();
	void InitializeDevice();
	void InitializeCommandPool();
	void InitializeCommandBuffer();
	void InitializeShaderModules();
	void InitializeDescriptorSetLayout();
	void InitializeDescriptorPool();
	void InitialzieDescriptorSet();
	void InitializePipelineLayout();
	void InitializePipelines();

public:
	void SetDebugMessageCallback(const decltype(debugMessageCallback)& callback);

	void CreateStorageBuffer(const void* data, VkDeviceSize size, VkBuffer* buffer, VkDeviceMemory* memory) const;
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer* buffer) const;
	void CreateBufferMemory(VkBuffer buffer, VkMemoryPropertyFlags properties, VkDeviceMemory* memory) const;
	void CopyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize size, VkCommandBuffer cmd = nullptr) const;
	VkCommandBuffer BeginSingleTimeCommand() const;
	void EndSingleTimeCommand(VkCommandBuffer cmd) const;

	inline void* Map(VkDeviceMemory memory, VkDeviceSize size) const { void* data; vkMapMemory(device, memory, 0, size, 0, &data); return data; }
	inline void Unmap(VkDeviceMemory memory) const { vkUnmapMemory(device, memory); }
	inline void Destroy(VkBuffer object) const { vkDestroyBuffer(device, object, allocator); }
	inline void Destroy(VkDeviceMemory object) const { vkFreeMemory(device, object, allocator); }
	inline void Destroy(VkCommandBuffer object) const { vkFreeCommandBuffers(device, commandPool, 1, &object); }

	Vulkan& operator = (const Vulkan&) = delete;
	Vulkan& operator = (Vulkan&&) = delete;

	Vulkan(const Vulkan&) = delete;
	Vulkan(Vulkan&&) = delete;

	Vulkan();
	~Vulkan();
};
