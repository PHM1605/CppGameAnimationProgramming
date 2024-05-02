#pragma once
#include <vulkan/vulkan.h>
#include "VkRenderData.h"

class SyncObjects {
public:
	static bool init(VkRenderData& renderData);
	static bool cleanup(VkRenderData& renderData);
};