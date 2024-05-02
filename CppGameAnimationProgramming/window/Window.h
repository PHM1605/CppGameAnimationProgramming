#pragma once
#include <string>
#include <memory>
//#include <glad/glad.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

//#include "OGLRenderer.h"
#include "VkRenderer.h"
#include "Model.h"

class Window {
public:
	bool init(unsigned int width, unsigned int height, std::string title);
	void mainLoop();
	void cleanup();
	//bool initVulkan();
private:
	GLFWwindow* mWindow = nullptr;
	std::unique_ptr<VkRenderer> mRenderer;
	std::unique_ptr<Model> mModel;

	//std::string mApplicationName;
	//VkInstance mInstance{};
	//VkSurfaceKHR mSurface{};

	//std::unique_ptr<OGLRenderer> mRenderer;
	//void handleWindowCloseEvents();
	//void handleKeyEvents(int key, int scancode, int action, int mods);
	//void handleMouseButtonEvents(int button, int action, int mods);
};