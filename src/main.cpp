#include <print>
#include <memory>
#include <utility>
#include <cstring>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "Image.h"
#include "Vulkan.h"

struct LoadedFilter
{
	int index;
	size_t parametersSize = 0;
	std::shared_ptr<void> parameters = nullptr;
};

struct LoadedImage
{
	const Vulkan& vk;
	Image image;
	VkBuffer bufferFrom, bufferTo;
	VkDeviceMemory memoryFrom, memoryTo;
	std::vector<LoadedFilter> filters;

	inline LoadedImage(const Vulkan& vk, const char* path) : vk(vk), image(path) {}

	inline ~LoadedImage()
	{
		vk.Destroy(memoryFrom);
		vk.Destroy(bufferFrom);
		vk.Destroy(memoryTo);
		vk.Destroy(bufferTo);
	}
};

void runImages(const Vulkan& vk, const std::span<LoadedImage>& images)
{
	VkCommandBuffer cmd = vk.commandBuffer;

	VkCommandBufferBeginInfo beginInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	};

	vkResetCommandBuffer(cmd, 0);
	vkBeginCommandBuffer(cmd, &beginInfo);

	for (auto& loadedImage : images)
	{
		for (const auto& loadedFilter : loadedImage.filters)
		{
			VkDescriptorBufferInfo bufferInfos[] =
			{
				{
					.buffer = loadedImage.bufferFrom,
					.offset = 0,
					.range = loadedImage.image.GetWidth() * loadedImage.image.GetHeight() * sizeof(Color),
				},
				{
					.buffer = loadedImage.bufferTo,
					.offset = 0,
					.range = loadedImage.image.GetWidth() * loadedImage.image.GetHeight() * sizeof(Color),
				},
			};

			VkWriteDescriptorSet descriptorWrites[] =
			{
				{
					.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					.dstSet = 0,
					.dstBinding = 0,
					.dstArrayElement = 0,
					.descriptorCount = 1,
					.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
					.pImageInfo = nullptr,
					.pBufferInfo = &bufferInfos[0],
					.pTexelBufferView = nullptr,
				},
				{
					.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					.dstSet = 0,
					.dstBinding = 1,
					.dstArrayElement = 0,
					.descriptorCount = 1,
					.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
					.pImageInfo = nullptr,
					.pBufferInfo = &bufferInfos[1],
					.pTexelBufferView = nullptr,
				},
			};

			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, vk.pipelines[loadedFilter.index]);
			vk.vkCmdPushDescriptorSetKHR(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, vk.pipelineLayout, 0, 2, descriptorWrites);

			if (loadedFilter.parametersSize > 0)
				vkCmdPushConstants(cmd, vk.pipelineLayout, VK_SHADER_STAGE_ALL, 0, loadedFilter.parametersSize, loadedFilter.parameters.get());

			vkCmdDispatch(cmd, loadedImage.image.GetWidth(), loadedImage.image.GetHeight(), 1);

			VkBufferMemoryBarrier barrier =
			{
			    .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
			    .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
			    .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
			    .buffer = loadedImage.bufferTo,
			    .offset = 0,
			    .size = VK_WHOLE_SIZE,
			};

			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 1, &barrier, 0, nullptr);

			std::swap(loadedImage.bufferFrom, loadedImage.bufferTo);
			std::swap(loadedImage.memoryFrom, loadedImage.memoryTo);
		}
	}

	vkEndCommandBuffer(cmd);

	// TODO: semaphores and fences

	VkSubmitInfo submitInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &cmd,
	};

	vkQueueSubmit(vk.computeQueue, 1, &submitInfo, nullptr);
}

LoadedImage loadImage(const Vulkan& vk, const char* path)
{
	LoadedImage image(vk, path);
	VkDeviceSize size = image.image.GetPixels().size_bytes();

	vk.CreateStorageBuffer(image.image.GetPixels().data(), size, &image.bufferFrom, &image.memoryFrom);

	vk.CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, &image.bufferTo);
	vk.CreateBufferMemory(image.bufferTo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &image.memoryTo);

	return image;
}

void readBack(LoadedImage& image)
{
	void* data = image.image.GetPixels().data();
	VkDeviceSize size = image.image.GetPixels().size_bytes();

	VkBuffer staggingBuffer;
	VkDeviceMemory staggingMemory;

	image.vk.CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT, &staggingBuffer);
	image.vk.CreateBufferMemory(staggingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staggingMemory);

	VkBufferMemoryBarrier barrier =
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
		.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.buffer = image.bufferFrom,
		.offset = 0,
		.size = VK_WHOLE_SIZE,
	};

	VkCommandBuffer cmd = image.vk.BeginSingleTimeCommand();
	vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 1, &barrier, 0, nullptr);
	image.vk.EndSingleTimeCommand(cmd);

	image.vk.CopyBuffer(image.bufferFrom, staggingBuffer, size);

	memcpy(data, image.vk.Map(staggingMemory, size), size);

	image.vk.Unmap(staggingMemory);
	image.vk.Destroy(staggingMemory);
	image.vk.Destroy(staggingBuffer);
}

int main(int argc, char** argv)
{
	Vulkan vk;
	vk.SetDebugMessageCallback([](VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagBitsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data)
		{
			std::println("{} {}", data->pMessageIdName, data->pMessage);
		}
	);

	LoadedImage image = loadImage(vk, "test.png");
	image.filters.push_back({ .index = 0 });

	runImages(vk, std::span(&image, 1));
	readBack(image);

	image.image.Save("test-out.png");
}
