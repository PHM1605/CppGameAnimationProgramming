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
}