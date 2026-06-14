#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <filesystem>
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
	InitializeCommandBuffer();
	InitializeShaderModules();
	InitializeDescriptorSetLayout();
	InitializeDescriptorPool();
	InitialzieDescriptorSet();
	InitializePipelineLayout();
	InitializePipelines();

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

VkShaderModule Vulkan::CreateShaderModule(const std::string& path) const
{
	std::ifstream file(path, std::ios::binary);

	assert(file.is_open());

	file.seekg(0, file.end);
	size_t size = file.tellg();
	file.seekg(0, file.beg);

	auto code = std::make_unique<uint8_t[]>(size);
	file.read((char*)code.get(), size);
	file.close();

	VkShaderModuleCreateInfo shaderModuleCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = size,
		.pCode = (const uint32_t*)code.get(),
	};

	VkShaderModule shaderModule;
	vkCreateShaderModule(device, &shaderModuleCreateInfo, allocator, &shaderModule);

	return shaderModule;
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
	vkCmdPushDescriptorSetKHR = (PFN_vkCmdPushDescriptorSetKHR)vkGetInstanceProcAddr(instance, "vkCmdPushDescriptorSetKHR");
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
	extensions.push_back("VK_KHR_push_descriptor");

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

void Vulkan::InitializeCommandBuffer()
{
	VkCommandBufferAllocateInfo commandBufferAllocateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};

	vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);
}

void Vulkan::InitializeShaderModules()
{
	for (const auto& x : std::filesystem::directory_iterator("build/shaders"))
		shaderModules.push_back(CreateShaderModule(x.path().string()));
}

void Vulkan::InitializeDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding layoutBindings[] =
	{
		{
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
			.pImmutableSamplers = nullptr,
		},
		{
			.binding = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
			.pImmutableSamplers = nullptr,
		},
	};

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.flags  = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR,
		.bindingCount = sizeof(layoutBindings) / sizeof(layoutBindings[0]),
		.pBindings = layoutBindings,
	};

	vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, allocator, &descriptorSetLayout);
}

void Vulkan::InitializeDescriptorPool()
{
	VkDescriptorPoolSize poolSize =
	{
		.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		.descriptorCount = 2,
	};

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		.maxSets = 1,
		.poolSizeCount = 1,
		.pPoolSizes = &poolSize,
	};

	vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, allocator, &descriptorPool);
}

void Vulkan::InitialzieDescriptorSet()
{
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = descriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &descriptorSetLayout,
	};

	vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet);
}

void Vulkan::InitializePipelineLayout()
{
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 1,
		.pSetLayouts = &descriptorSetLayout,
	};

	vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, allocator, &pipelineLayout);
}

void Vulkan::InitializePipelines()
{
	std::vector<VkPipelineShaderStageCreateInfo> stages;
	stages.reserve(shaderModules.size());

	for (auto& module : shaderModules)
	{
		stages.push_back(
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.stage = VK_SHADER_STAGE_COMPUTE_BIT,
				.module = module,
				.pName = "main",
			}
		);
	}

	std::vector<VkComputePipelineCreateInfo> createInfos;
	createInfos.reserve(stages.size());

	for (auto& stage : stages)
	{
		createInfos.push_back(
			{
				.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
				.stage = stage,
				.layout = pipelineLayout,
			}
		);
	}

	pipelines.resize(createInfos.size());
	vkCreateComputePipelines(device, nullptr, createInfos.size(), createInfos.data(), allocator, pipelines.data());
}

void Vulkan::CreateStorageBuffer(const void* data, VkDeviceSize size, VkBuffer* buffer, VkDeviceMemory* memory) const
{
	VkBuffer staggingBuffer;
	VkDeviceMemory staggingMemory;

	CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &staggingBuffer);
	CreateBufferMemory(staggingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staggingMemory);

	memcpy(Map(staggingMemory, size), data, size);

	CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, buffer);
	CreateBufferMemory(*buffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memory);

	CopyBuffer(staggingBuffer, *buffer, size);

	Unmap(staggingMemory);
	vkFreeMemory(device, staggingMemory, allocator);
	vkDestroyBuffer(device, staggingBuffer, allocator);
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
	for (auto& x : pipelines)
		vkDestroyPipeline(device, x, allocator);

	vkDestroyPipelineLayout(device, pipelineLayout, allocator);
	vkFreeDescriptorSets(device, descriptorPool, 1, &descriptorSet);
	vkDestroyDescriptorPool(device, descriptorPool, allocator);
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, allocator);

	for (auto& x : shaderModules)
		vkDestroyShaderModule(device, x, allocator);

	// vkUnmapMemory(device, globalUniformsMemory);
	// vkFreeMemory(device, globalUniformsMemory, allocator);
	// vkDestroyBuffer(device, globalUniformsBuffer, allocator);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	vkDestroyCommandPool(device, commandPool, allocator);
	vkDestroyDevice(device, allocator);
	vkDestroyDebugUtilsMessengerEXT(instance, debugUtilsMessenger, allocator);
	vkDestroyInstance(instance, allocator);
}
