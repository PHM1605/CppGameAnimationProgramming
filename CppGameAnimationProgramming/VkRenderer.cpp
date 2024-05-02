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