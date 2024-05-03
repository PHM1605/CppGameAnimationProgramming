#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cstring>
#include "CommandBuffer.h"
#include "Texture.h"
#include <Logger.h>

bool Texture::loadTexture(VkRenderData& renderData, std::string textureFilename) {
	// Texture data
	int texWidth;
	int texHeight;
	int numberOfChannels;
	unsigned char* textureData = stbi_load(textureFilename.c_str(), &texWidth, &texHeight, &numberOfChannels, STBI_rgb_alpha);
	if (!textureData) {
		Logger::log(1, "%s error: could not load file '%s'\n", __FUNCTION__, textureFilename.c_str());
		stbi_image_free(textureData);
		return false;
	}

	// Image
	VkDeviceSize imageSize = texWidth * texHeight * 4;
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = static_cast<uint32_t>(texWidth);
	imageInfo.extent.height = static_cast<uint32_t>(texHeight);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageInfo.tiling = VK_IMAGE_TILING_LINEAR;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	VmaAllocationCreateInfo imageAllocInfo{};
	imageAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	if (vmaCreateImage(renderData.rdAllocator, &imageInfo, &imageAllocInfo, &renderData.rdTextureImage, &renderData.rdTextureImageAlloc, nullptr) != VK_SUCCESS) {
		Logger::log(1, "%s error: could not allocate texture image via VMA\n", __FUNCTION__);
		return false;
	}

	// Staging buffer
	VkBufferCreateInfo stagingBufferInfo{};
	stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufferInfo.size = imageSize;
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VkBuffer stagingBuffer;
	VmaAllocation stagingBufferAlloc;
	VmaAllocationCreateInfo stagingAllocInfo{};
	stagingAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	if (vmaCreateBuffer(renderData.rdAllocator, &stagingBufferInfo, &stagingAllocInfo, &stagingBuffer, &stagingBufferAlloc, nullptr) != VK_SUCCESS) {
		Logger::log(1, "%s error: could not allocate texture staging buffer via VMA\n", __FUNCTION__);
		return false;
	}

	// Map texture data to stagingBufferAlloc
	void* data;
	vmaMapMemory(renderData.rdAllocator, stagingBufferAlloc, &data);
	std::memcpy(data, textureData, static_cast<uint32_t>(imageSize));
	vmaUnmapMemory(renderData.rdAllocator, stagingBufferAlloc);
	stbi_image_free(textureData);

}

void Texture::cleanup(VkRenderData& renderData) {

}