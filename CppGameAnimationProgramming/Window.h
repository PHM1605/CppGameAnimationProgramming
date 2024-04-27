#pragma once
#include <string>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>


class Window {
public:
	bool init(unsigned int width, unsigned int height, std::string title);
	void mainLoop();
	void cleanup();
	bool initVulkan();
private:
	GLFWwindow* mWindow = nullptr;
	std::string mApplicationName;
	VkInstance mInstance{};
	VkSurfaceKHR mSurface{};

	void handleWindowCloseEvents();
	void handleKeyEvents(int key, int scancode, int action, int mods);
	void handleMouseButtonEvents(int button, int action, int mods);
};