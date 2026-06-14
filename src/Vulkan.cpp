#include <vector>
#include <algorithm>
#include <unordered_map>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>

#include "Vulkan.h"

/* static */ unsigned int Vulkan::OnDebugMessage(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data, void* userData)
{
	Vulkan* appliaction = (Vulkan*)userData;

	if (appliaction->debugMessageCallback)
		appliaction->debugMessageCallback(severity, *(VkDebugUtilsMessageTypeFlagBitsEXT*)(&type), data);

	return 0;
}

void Vulkan::Initialize()
{
	InitializeInstance();
	InitializeExtensions();
	InitializeDebugMessages();
	InitializePhysicalDevice();
	InitializeDevice();
	InitializeCommandPool();

	vkGetDeviceQueue(device, computeQueueFamilyIndex, 0, &computeQueue);
}

uint32_t Vulkan::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
		    return i;
	}

	assert(!"Failed to find suitable memory type!");
}

void Vulkan::InitializeInstance()
{
	VkApplicationInfo appInfo =
	{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Image Lab",
		.applicationVersion = MakeVersion(1, 0, 0),
		.pEngineName = "No Engine",
		.engineVersion = MakeVersion(1, 0, 0),
		.apiVersion = MakeApiVersion(0, 1, 0, 0),
	};

	std::vector<const char*> layers;
	layers.push_back("VK_LAYER_KHRONOS_validation");

	std::vector<const char*> extensions;
	extensions.push_back("VK_KHR_portability_enumeration");
	extensions.push_back("VK_EXT_debug_utils");
	extensions.push_back("VK_KHR_get_physical_device_properties2");

	uint32_t requiredExtensionsCount;
	const char* const* requiredExtensions = glfwGetRequiredInstanceExtensions(&requiredExtensionsCount);

	for (uint32_t i = 0; i < requiredExtensionsCount; i++)
		if (requiredExtensions[i])
			extensions.push_back(requiredExtensions[i]);

	VkInstanceCreateInfo instanceCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = (uint32_t)layers.size(),
		.ppEnabledLayerNames = layers.data(),
		.enabledExtensionCount = (uint32_t)extensions.size(),
		.ppEnabledExtensionNames = extensions.data(),
	};

	vkCreateInstance(&instanceCreateInfo, allocator, &instance);
}

void Vulkan::InitializeExtensions()
{
	vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
}

void Vulkan::InitializeDebugMessages()
{
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = &OnDebugMessage,
		.pUserData = this,
	};

	vkCreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, allocator, &debugUtilsMessenger);
}

void Vulkan::InitializePhysicalDevice()
{
	uint32_t count;

	vkEnumeratePhysicalDevices(instance, &count, nullptr);

	std::vector<VkPhysicalDevice> physicalDevices;
	physicalDevices.resize(count);

	vkEnumeratePhysicalDevices(instance, &count, physicalDevices.data());

	auto found = std::find_if(physicalDevices.begin(), physicalDevices.end(), [](const VkPhysicalDevice& x) -> bool
		{
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(x, &properties);

			return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
		}
	);

	assert(found != physicalDevices.end());
	physicalDevice = *found;
}

void Vulkan::InitializeDevice()
{
	uint32_t queueFamilyCount = 0;

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies;
	queueFamilies.resize(queueFamilyCount);

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	computeQueueFamilyIndex = -1;

	for (int i = 0; i < queueFamilyCount; i++)
		if ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT))
			computeQueueFamilyIndex = i;

	assert(computeQueueFamilyIndex != -1);

	float priorities[] = { 1.0f };

	VkDeviceQueueCreateInfo computeQueueCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = computeQueueFamilyIndex,
		.queueCount = 1,
		.pQueuePriorities = priorities,
	};

	std::vector<const char*> layers;
	layers.push_back("VK_LAYER_KHRONOS_validation");

	std::vector<const char*> extensions;

	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(physicalDevice, &features);

	VkDeviceCreateInfo deviceCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &computeQueueCreateInfo,
		.enabledLayerCount = (uint32_t)layers.size(),
		.ppEnabledLayerNames = layers.data(),
		.enabledExtensionCount = (uint32_t)extensions.size(),
		.ppEnabledExtensionNames = extensions.data(),
		.pEnabledFeatures = &features,
	};

	vkCreateDevice(physicalDevice, &deviceCreateInfo, allocator, &device);
}

void Vulkan::InitializeCommandPool()
{
	VkCommandPoolCreateInfo commandPoolCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = computeQueueFamilyIndex,
	};

	vkCreateCommandPool(device, &commandPoolCreateInfo, allocator, &commandPool);
}

void Vulkan::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer* buffer) const
{
	VkBufferCreateInfo createInfo =
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr,
	};

	vkCreateBuffer(device, &createInfo, allocator, buffer);
}

void Vulkan::CreateBufferMemory(VkBuffer buffer, VkMemoryPropertyFlags properties, VkDeviceMemory* memory) const
{
	VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo =
    {
    	.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
     	.allocationSize = memoryRequirements.size,
      	.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, properties),
    };

    vkAllocateMemory(device, &allocateInfo, allocator, memory);
    vkBindBufferMemory(device, buffer, *memory, 0);
}

void Vulkan::CreateImage(int width, int height, VkImageType type, VkImageUsageFlags usage, VkFormat format, VkImage* image) const
{
	VkImageCreateInfo createInfo =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = type,
		.format = format,
		.extent = { .width = (uint32_t)width, .height = (uint32_t)height, .depth = 1 },
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};

	vkCreateImage(device, &createInfo, allocator, image);
}

void Vulkan::CreateImageMemory(VkImage image, VkDeviceMemory* memory) const
{
	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(device, image, &memoryRequirements);

	VkMemoryAllocateInfo allocateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memoryRequirements.size,
		.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
	};

	vkAllocateMemory(device, &allocateInfo, allocator, memory);
	vkBindImageMemory(device, image, *memory, 0);
}

void Vulkan::CopyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize size, VkCommandBuffer cmd) const
{
	bool createCmd = cmd == nullptr;

	if (createCmd)
		cmd = BeginSingleTimeCommand();

	VkBufferCopy region =
	{
		.srcOffset = 0,
		.dstOffset = 0,
		.size = size,
	};

	vkCmdCopyBuffer(cmd, source, destination, 1, &region);

	if (createCmd)
		EndSingleTimeCommand(cmd);
}

void Vulkan::CopyBufferToImage(VkBuffer buffer, VkImage image, int width, int height, VkImageAspectFlags aspect, VkCommandBuffer cmd) const
{
	bool createCmd = cmd == nullptr;

	if (createCmd)
		cmd = BeginSingleTimeCommand();

	VkBufferImageCopy region =
	{
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource =
		{
			.aspectMask = aspect,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
		.imageOffset = { .x = 0, .y = 0, .z = 0 },
		.imageExtent = { .width = (uint32_t)width, .height = (uint32_t)height, .depth = 1 },
	};

	vkCmdCopyBufferToImage(cmd, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	if (createCmd)
		EndSingleTimeCommand(cmd);
}

void Vulkan::CopyImageToBuffer(VkImage image, VkBuffer buffer, int width, int height, VkImageAspectFlags aspect, VkCommandBuffer cmd) const
{
	bool createCmd = cmd == nullptr;

	if (createCmd)
		cmd = BeginSingleTimeCommand();

	VkBufferImageCopy region =
	{
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource =
		{
			.aspectMask = aspect,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
		.imageOffset = { .x = 0, .y = 0, .z = 0 },
		.imageExtent = { .width = (uint32_t)width, .height = (uint32_t)height, .depth = 1 },
	};

	vkCmdCopyImageToBuffer(cmd, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buffer, 1, &region);

	if (createCmd)
		EndSingleTimeCommand(cmd);
}

void Vulkan::CreateTexture(void* data, int width, int height, VkImageType type, VkFormat format, VkImage* image, VkImageView* imageView, VkDeviceMemory* memory, VkSampler* sampler) const
{
	static std::unordered_map<VkFormat, VkDeviceSize> strideMap =
	{
		{ VK_FORMAT_R8G8B8A8_UNORM, 4 },
		{ VK_FORMAT_B8G8R8A8_UNORM, 4 },
	};

	assert(strideMap.contains(format));

	VkDeviceSize stride = strideMap[format];
	VkDeviceSize size = (VkDeviceSize)width * (VkDeviceSize)height * stride;

	VkBuffer staggingBuffer;
	VkDeviceMemory staggingMemory;

	CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &staggingBuffer);
	CreateBufferMemory(staggingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &staggingMemory);

	void* staggingLocation;
	vkMapMemory(device, staggingMemory, 0, size, 0, &staggingLocation);
	memcpy(staggingLocation, data, size);

	CreateImage(width, height, type, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, format, image);
	CreateImageMemory(*image, memory);

	TransitionImageLayout(*image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(staggingBuffer, *image, width, height);
	TransitionImageLayout(*image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	CreateImageView(*image, format, VK_IMAGE_ASPECT_COLOR_BIT, imageView);
	CreateSampler(sampler);

	vkUnmapMemory(device, staggingMemory);
	vkDestroyBuffer(device, staggingBuffer, allocator);
	vkFreeMemory(device, staggingMemory, allocator);
}

void Vulkan::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect, VkImageView* imageView) const
{
	VkImageViewCreateInfo createInfo =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = format,
		.components =
		{
			.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.a = VK_COMPONENT_SWIZZLE_IDENTITY
		},
		.subresourceRange =
		{
			.aspectMask = aspect,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
	};

	vkCreateImageView(device, &createInfo, allocator, imageView);
}

void Vulkan::CreateSampler(VkSampler* sampler) const
{
	VkSamplerCreateInfo createInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.mipLodBias = 0.0f,
		.anisotropyEnable = false,
		.maxAnisotropy = 1.0f,
		.compareEnable = false,
		.compareOp = VK_COMPARE_OP_ALWAYS,
		.minLod = 0.0f,
		.maxLod = 0.0f,
		.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
		.unnormalizedCoordinates = false
	};

	vkCreateSampler(device, &createInfo, allocator, sampler);
}

void Vulkan::TransitionImageLayout(
	VkImage image,

	VkImageLayout from,
	VkAccessFlags sourceAccess,
	VkPipelineStageFlags sourceStage,

	VkImageLayout to,
	VkAccessFlags destinationAccess,
	VkPipelineStageFlags destinationStage,

	VkCommandBuffer cmd
) const
{
	bool createCmd = cmd == nullptr;

	if (createCmd)
		cmd = BeginSingleTimeCommand();

	VkImageMemoryBarrier barrier =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.srcAccessMask = sourceAccess,
		.dstAccessMask = destinationAccess,
		.oldLayout = from,
		.newLayout = to,
		.srcQueueFamilyIndex = ~0u,
		.dstQueueFamilyIndex = ~0u,
		.image = image,
		.subresourceRange =
		{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		},
	};

	vkCmdPipelineBarrier(cmd, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	if (createCmd)
		EndSingleTimeCommand(cmd);
}

void Vulkan::TransitionImageLayout(VkImage image, VkImageLayout from, VkImageLayout to, VkCommandBuffer cmd) const
{
	VkAccessFlags sourceAccess, destinationAccess;
	VkPipelineStageFlags sourceStage, destinationStage;

	if (from == VK_IMAGE_LAYOUT_UNDEFINED && to == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		sourceAccess = 0;
		destinationAccess = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (from == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && to == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		sourceAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
		destinationAccess = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (from == VK_IMAGE_LAYOUT_UNDEFINED && to == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		sourceAccess = 0;
		destinationAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else if (from == VK_IMAGE_LAYOUT_UNDEFINED && to == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		sourceAccess = 0;
		destinationAccess = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (from == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && to == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		sourceAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		destinationAccess = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (from == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && to == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		sourceAccess = VK_ACCESS_SHADER_READ_BIT;
		destinationAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else if (from == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR && to == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		sourceAccess = VK_ACCESS_NONE;
		destinationAccess = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (from == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR && to == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		sourceAccess = VK_ACCESS_NONE;
		destinationAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else if (from == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && to == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		sourceAccess = VK_ACCESS_TRANSFER_READ_BIT;
		destinationAccess = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (from == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && to == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		sourceAccess = VK_ACCESS_SHADER_READ_BIT;
		destinationAccess = VK_ACCESS_TRANSFER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else
		assert("Unsupported layer transition from.");

	TransitionImageLayout(image, from, sourceAccess, sourceStage, to, destinationAccess, destinationStage, cmd);
}

VkCommandBuffer Vulkan::BeginSingleTimeCommand() const
{
	VkCommandBuffer cmd;

	VkCommandBufferAllocateInfo allocateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};

	vkAllocateCommandBuffers(device, &allocateInfo, &cmd);

	VkCommandBufferBeginInfo beginInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};

	vkBeginCommandBuffer(cmd, &beginInfo);

	return cmd;
}

void Vulkan::EndSingleTimeCommand(VkCommandBuffer cmd) const
{
	vkEndCommandBuffer(cmd);

	VkSubmitInfo submitInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = nullptr,
		.commandBufferCount = 1,
		.pCommandBuffers = &cmd,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = nullptr,
	};

	vkQueueSubmit(computeQueue, 1, &submitInfo, nullptr);
	vkQueueWaitIdle(computeQueue);

	vkFreeCommandBuffers(device, commandPool, 1, &cmd);
}

void Vulkan::SetDebugMessageCallback(const decltype(debugMessageCallback)& callback)
{
	debugMessageCallback = callback;
}

Vulkan::Vulkan()
{
	Initialize();
}

Vulkan::~Vulkan()
{
	vkDestroyCommandPool(device, commandPool, allocator);
	vkDestroyDevice(device, allocator);
	vkDestroyDebugUtilsMessengerEXT(instance, debugUtilsMessenger, allocator);
	vkDestroyInstance(instance, allocator);
}
