#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vkb/VkBootstrap.h>
#include <vma/vk_mem_alloc.h>

struct VkVertex {
	glm::vec3 position;
	glm::vec2 uv;
};

struct VkMesh {
	std::vector<VkVertex> vertices;
};

struct VkRenderData {
	VmaAllocator rdAllocator;
	vkb::Instance rdVkbInstance{};
	vkb::Device rdVkbDevice{};
	vkb::Swapchain rdVkbSwapchain{};
	std::vector<VkImage> rdSwapchainImages;
	std::vector<VkImageView> rdSwapchainImageViews;
	std::vector<VkFramebuffer> rdFramebuffers;
	// Queues
	VkQueue rdGraphicsQueue = VK_NULL_HANDLE;
	VkQueue rdPresentQueue = VK_NULL_HANDLE;
	// Depth
	VkImage rdDepthImage = VK_NULL_HANDLE;
	VkImageView rdDepthImageView = VK_NULL_HANDLE;
	VkFormat rdDepthFormat;
	VmaAllocation rdDepthImageAlloc = VK_NULL_HANDLE;
	// Renderpass
	VkRenderPass rdRenderpass = VK_NULL_HANDLE;
	// Pipeline and Pipeline layout
	VkPipelineLayout rdPipelineLayout = VK_NULL_HANDLE;
	VkPipeline rdPipeline = VK_NULL_HANDLE;
	// Command pool and Command buffer
	VkCommandPool rdCommandPool = VK_NULL_HANDLE;
	VkCommandBuffer rdCommandBuffer = VK_NULL_HANDLE;
	// Semaphore
	VkSemaphore rdPresentSemaphore = VK_NULL_HANDLE;
	VkSemaphore rdRenderSemaphore = VK_NULL_HANDLE;
	// Fence
	VkFence rdRenderFence = VK_NULL_HANDLE;
	// Textures
	VkImage rdTextureImage = VK_NULL_HANDLE;
	VkImageView rdTextureImageView = VK_NULL_HANDLE;
	VkSampler rdTextureSampler = VK_NULL_HANDLE;
	VmaAllocation rdTextureImageAlloc = VK_NULL_HANDLE;
	// Descriptor
	VkDescriptorPool rdDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout rdTextureLayout = VK_NULL_HANDLE;
	VkDescriptorSet rdDescriptorSet = VK_NULL_HANDLE;
};