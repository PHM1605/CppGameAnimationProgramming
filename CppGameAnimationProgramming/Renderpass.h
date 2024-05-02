#pragma once
#include <vulkan/vulkan.h>
#include "VkRenderData.h"

class RenderPass {
public:
	static bool init(VkRenderData& renderData);
	static void cleanup(VkRenderData& renderData);
};