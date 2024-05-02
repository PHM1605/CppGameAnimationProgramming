#include "Renderpass.h"
#include "Logger.h"
#include <vkb/VkBootstrap.h>

bool Renderpass::init(VkRenderData& renderData) {

}

void Renderpass::cleanup(VkRenderData& renderData) {
	vkDestroyRenderPass(renderData.rdVkbDevice.device, renderData.rdRenderpass, nullptr);
}