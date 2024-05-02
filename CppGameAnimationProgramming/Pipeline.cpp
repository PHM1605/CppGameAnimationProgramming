#include <vector>
#include <vkb/VkBootstrap.h>
#include "Pipeline.h"
#include "Logger.h"
#include "Shader.h"

bool Pipeline::init(VkRenderData& renderData, std::string vertexShaderFilename, std::string fragmentShaderFilename) {
	// Pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &renderData.rdTextureLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	if (vkCreatePipelineLayout(renderData.rdVkbDevice.device, &pipelineLayoutInfo, nullptr, &renderData.rdPipelineLayout) != VK_SUCCESS) {
		Logger::log(1, "%s error: could not create pipeline layout\n", __FUNCTION__);
		return false;
	}

	/* shader */
	VkShaderModule vertexModule = Shader::loadShader(renderData.rdVkbDevice.device, vertexShaderFilename);
	VkShaderModule fragmentModule = Shader::loadShader(renderData.rdVkbDevice.device, fragmentShaderFilename);
	if (vertexModule == VK_NULL_HANDLE || fragmentModule == VK_NULL_HANDLE) {
		Logger::log(1, "%s error: could not load shaders\n", __FUNCTION__);
		return false;
	}

	VkPipelineShaderStageCreateInfo vertexStageInfo{};
	vertexStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexStageInfo.module = vertexModule;
	vertexStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragmentStageInfo{};
	fragmentStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentStageInfo.module = fragmentModule;
	fragmentStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStagesInfo[] = { vertexStageInfo, fragmentStageInfo };

	// Input description
	VkVertexInputBindingDescription mainBinding{};
	mainBinding.binding = 0;
	mainBinding.stride = sizeof(VkVertex);
	mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription positionAttribute{};
	positionAttribute.binding = 0;
	positionAttribute.location = 0;
	positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
	positionAttribute.offset = offsetof(VkVertex, position);

	VkVertexInputAttributeDescription uvAttribute{};
	uvAttribute.binding = 0;
	uvAttribute.location = 1;
	uvAttribute.format = VK_FORMAT_R32G32_SFLOAT;
	uvAttribute.offset = offsetof(VkVertex, uv);

	VkVertexInputAttributeDescription attributes[] = { positionAttribute, uvAttribute };

	// Vertex input info
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &mainBinding;
	vertexInputInfo.vertexAttributeDescriptionCount = 2;
	vertexInputInfo.pVertexAttributeDescriptions = attributes;

	// Input assembly
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	// Viewport
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(renderData.rdVkbSwapchain.extent.width);
	viewport.height = static_cast<float>(renderData.rdVkbSwapchain.extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{};
	scissor.offset = { 0,0 };
	scissor.extent = renderData.rdVkbSwapchain.extent;
	
	VkPipelineViewportStateCreateInfo viewportStateInfo{};
	viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateInfo.viewportCount = 1;
	viewportStateInfo.pViewports = &viewport;
	viewportStateInfo.scissorCount = 1;
	viewportStateInfo.pScissors = &scissor;


	// Pipeline info
	VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = 2;
	pipelineCreateInfo.pStages = shaderStagesInfo;
	pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineCreateInfo.pViewportState = &viewportStateInfo;
	pipelineCreateInfo.pRasterizationState = &rasterizerInfo;
	pipelineCreateInfo.pMultisampleState = &multisamplingInfo;
	pipelineCreateInfo.pColorBlendState = &colorBlendingInfo;
	pipelineCreateInfo.pDepthStencilState = &depthStencilInfo;
	pipelineCreateInfo.pDynamicState = &dynStatesInfo;
	pipelineCreateInfo.layout = renderData.rdPipelineLayout;
	pipelineCreateInfo.renderPass = renderData.rdRenderPass;
	pipelineCreateInfo.subpass = 0;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

	// Create pipeline
	if (vkCreateGraphicsPipelines(renderData.rdVkbDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &renderData.rdPipeline) != VK_SUCCESS) {
		Logger::log(1, "%s error: could not create rendering pipeline\n", __FUNCTION__);
		vkDestroyPipelineLayout(renderData.rdVkbDevice.device, renderData.rdPipelineLayout, nullptr);
		return false;
	}

	// Destroy shader modules after pipeline has been created
	vkDestroyShaderModule(renderData.rdVkbDevice.device, fragmentModule, nullptr);
	vkDestroyShaderModule(renderData.rdVkbDevice.device, vertexModule, nullptr);
	return true;
}

void Pipeline::cleanup(VkRenderData& renderData) {
	vkDestroyPipeline(renderData.rdVkbDevice.device, renderData.rdPipeline, nullptr);
	vkDestroyPipelineLayout(renderData.rdVkbDevice.device, renderData.rdPipelineLayout, nullptr);
}