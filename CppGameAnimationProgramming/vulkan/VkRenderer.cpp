#include <cstring>
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
#include "VkRenderer.h"
#include "Logger.h"

VkRenderer::VkRenderer(GLFWwindow* window) {
	mWindow = window;
}

bool VkRenderer::init(unsigned int width, unsigned int height) {
	if (!mWindow) {
		Logger::log(1, "%s error: invalid GLFWwindow handle\n", __FUNCTION__);
		return false;
	}
	if (!deviceInit()) {
		return false;
	}
	if (!initVma()) {
		return false;
	}
	if (!getQueue()) {
		return false;
	}
	if (!createSwapchain()) {
		return false;
	}
	// Needs data from Swapchain
	if (!createDepthBuffer()) {
		return false;
	}
	if (!createCommandPool()) {
		return false;
	}
	if (!createCommandBuffer()) {
		return false;
	}
	// Needs command pool
	if (!loadTexture()) {
		return false;
	}
	if (!createRenderPass()) {
		return false;
	}
	if (!createPipeline()) {
		return false;
	}
	if (!createFramebuffer()) {
		return false;
	}
	if (!createSyncObjects()) {
		return false;
	}

	Logger::log(1, "%s: Vulkan renderer initialized to %ix%i\n", __FUNCTION__, width, height);
	return true;
}

bool VkRenderer::deviceInit() {
	// Build instance with VkBootstrap
	vkb::InstanceBuilder instBuild;
	auto instRet = instBuild.use_default_debug_messenger().request_validation_layers().build();
	if (!instRet) {
		Logger::log(1, "%s error: could not build vkb instance\n", __FUNCTION__);
		return false;
	}
	mRenderData.rdVkbInstance = instRet.value();

	// Create surface
	VkResult result = VK_ERROR_UNKNOWN;
	result = glfwCreateWindowSurface(mRenderData.rdVkbInstance, mWindow, nullptr, &mSurface);
	if (result != VK_SUCCESS) {
		Logger::log(1, "%s error: Could not create Vulkan surface\n", __FUNCTION__);
		return false;
	}

	// Device
	vkb::PhysicalDeviceSelector physicalDevSel{ mRenderData.rdVkbInstance };
	auto physicalDevSelRet = physicalDevSel.set_surface(mSurface).select();
	if (!physicalDevSelRet) {
		Logger::log(1, "%s error: could not get physical devices\n", __FUNCTION__);
		return false;
	}
	mPhysDevice = physicalDevSelRet.value();
	Logger::log(1, "%s: found physical device '%s'\n", __FUNCTION__, mPhysDevice.name.c_str());

	// Device builder
	vkb::DeviceBuilder devBuilder{ mPhysDevice };
	auto devBuilderRet = devBuilder.build();
	if (!devBuilderRet) {
		Logger::log(1, "%s error: could not get devices\n", __FUNCTION__);
		return false;
	}
	mRenderData.rdVkbDevice = devBuilderRet.value();
	return true;
}

bool VkRenderer::getQueue() {
	// Graph queue
	auto graphQueueRet = mRenderData.rdVkbDevice.get_queue(vkb::QueueType::graphics);
	if (!graphQueueRet.has_value()) {
		Logger::log(1, "%s error: could not get graphics queue\n", __FUNCTION__);
		return false;
	}
	mRenderData.rdGraphicsQueue = graphQueueRet.value();
	// Present queue
	auto presentQueueRet = mRenderData.rdVkbDevice.get_queue(vkb::QueueType::present);
	if (!presentQueueRet.has_value()) {
		Logger::log(1, "%s error: could not get present queue\n", __FUNCTION__);
		return false;
	}
	mRenderData.rdPresentQueue = presentQueueRet.value();
	return true;
}

bool VkRenderer::createDepthBuffer() {
	VkExtent3D depthImageExtent = {
		mRenderData.rdVkbSwapchain.extent.width, 
		mRenderData.rdVkbSwapchain.extent.height,
		1
	};
	// Image Depth
	mRenderData.rdDepthFormat = VK_FORMAT_D32_SFLOAT;
	VkImageCreateInfo depthImageInfo{};
	depthImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	depthImageInfo.imageType = VK_IMAGE_TYPE_2D;
	depthImageInfo.format = mRenderData.rdDepthFormat;
	depthImageInfo.extent = depthImageExtent;
	depthImageInfo.mipLevels = 1;
	depthImageInfo.arrayLayers = 1;
	depthImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	depthImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	depthImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	VmaAllocationCreateInfo depthAllocInfo{};
	depthAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	depthAllocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if (vmaCreateImage(mRenderData.rdAllocator, &depthImageInfo, &depthAllocInfo, &mRenderData.rdDepthImage, &mRenderData.rdDepthImageAlloc, nullptr) != VK_SUCCESS) {
		Logger::log(1, "%s error: could not allocate depth buffer memory\n", __FUNCTION__);
		return false;
	}

	// ImageView Depth
	VkImageViewCreateInfo depthImageViewinfo{};
	depthImageViewinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	depthImageViewinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	depthImageViewinfo.image = mRenderData.rdDepthImage;
	depthImageViewinfo.format = mRenderData.rdDepthFormat;
	depthImageViewinfo.subresourceRange.baseMipLevel = 0;
	depthImageViewinfo.subresourceRange.levelCount = 1;
	depthImageViewinfo.subresourceRange.baseArrayLayer = 0;
	depthImageViewinfo.subresourceRange.layerCount = 1;
	depthImageViewinfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	if (vkCreateImageView(mRenderData.rdVkbDevice.device, &depthImageViewinfo, nullptr, &mRenderData.rdDepthImageView) != VK_SUCCESS) {
		Logger::log(1, "%s error: could not create depth buffer image view\n", __FUNCTION__);
		return false;
	}

	return true;
}

bool VkRenderer::createSwapchain() {
	vkb::SwapchainBuilder swapChainBuild{ mRenderData.rdVkbDevice };
	auto swapChainBuildRet = swapChainBuild.set_old_swapchain(mRenderData.rdVkbSwapchain).set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR).build();
	if (!swapChainBuildRet) {
		Logger::log(1, "%s error: could not init swapchain\n", __FUNCTION__);
		return false;
	}
	vkb::destroy_swapchain(mRenderData.rdVkbSwapchain);
	mRenderData.rdVkbSwapchain = swapChainBuildRet.value();
	return true;
}

bool VkRenderer::recreateSwapchain() {
	return true;
}

bool VkRenderer::createRenderPass() {
	return true;
}

bool VkRenderer::createPipeline() {
	return true;
}

bool VkRenderer::createFramebuffer() {
	return true;
}

bool VkRenderer::createCommandPool() {
	return true;
}

bool VkRenderer::createCommandBuffer() {
	return true;
}

bool VkRenderer::createSyncObjects() {
	return true;
}

bool VkRenderer::loadTexture() {
	return true;
}

bool VkRenderer::initVma() {
	VmaAllocatorCreateInfo allocatorInfo{};
	allocatorInfo.physicalDevice = mPhysDevice.physical_device;
	allocatorInfo.device = mRenderData.rdVkbDevice.device;
	allocatorInfo.instance = mRenderData.rdVkbInstance.instance;
	if (vmaCreateAllocator(&allocatorInfo, &mRenderData.rdAllocator) != VK_SUCCESS) {
		Logger::log(1, "%s error: could not init VMA\n", __FUNCTION__);
		return false;
	}
	return true;
}

void VkRenderer::setSize(unsigned int width, unsigned int height) {
	Logger::log(1, "%s: resized window to %ix%i\n", __FUNCTION__, width, height);
}

bool VkRenderer::uploadData(VkMesh vertexData) {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = vertexData.vertices.size() * sizeof(VkVertex);
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	VmaAllocationCreateInfo vmaAllocInfo{};
	vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
	if (vmaCreateBuffer(mRenderData.rdAllocator, &bufferInfo, &vmaAllocInfo, &mVertexBuffer, &mVertexBufferAlloc, nullptr) != VK_SUCCESS) {
		Logger::log(1, "%s error: could not allocate vertex buffer via VMA\n", __FUNCTION__);
		return false;
	}

	void* data;
	vmaMapMemory(mRenderData.rdAllocator, mVertexBufferAlloc, &data);
	std::memcpy(data, vertexData.vertices.data(), vertexData.vertices.size() * sizeof(VkVertex));
	vmaUnmapMemory(mRenderData.rdAllocator, mVertexBufferAlloc);
	mTriangleCount = vertexData.vertices.size() / 3;
	return true;
}

bool VkRenderer::draw() {
	return true;
}

void VkRenderer::cleanup() {

}