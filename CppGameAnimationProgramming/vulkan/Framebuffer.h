#pragma once
/*
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Framebuffer {
public:
	bool init(unsigned int width, unsigned int height);
	bool resize(unsigned int newWidth, unsigned int newHeight);
	void bind();
	void unbind();
	void drawToScreen();
	void cleanup();
private:
	unsigned int mBufferWidth = 640;
	unsigned int mBufferHeight = 480;
	GLuint mBuffer = 0;
	GLuint mColorTex = 0;
	GLuint mDepthBuffer = 0;
	bool checkComplete();
};
*/

#include <vector>
#include <vulkan/vulkan.h>
#include "VkRenderData.h"

class FrameBuffer {
public:
	static bool init(VkRenderData& renderData);
	static bool cleanup(VkRenderData& renderData);
};