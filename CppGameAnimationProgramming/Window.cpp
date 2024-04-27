#include "Window.h"
#include "Logger.h"
#include <vector>
#include <stdexcept>
#include <iostream>

bool Window::init(unsigned int width, unsigned int height, std::string title) {
	
	if (!glfwInit()) {
		Logger::log(1, "%s: glfwInit() error\n", __FUNCTION__);
		return false;
	}

	if (!glfwVulkanSupported()) {
		glfwTerminate();
		Logger::log(1, "%s: Vulkan is not supported\n", __FUNCTION__);
		return false;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	mApplicationName = title;
	mWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!mWindow) {
		Logger::log(1, "%s: Could not create window\n", __FUNCTION__);
		glfwTerminate();
		return false;
	}

	// Save user pointer
	glfwSetWindowUserPointer(mWindow, this);

	// ... to use the callbacks
	glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* win) {
		auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
		thisWindow->handleWindowCloseEvents();
		});

	glfwSetKeyCallback(mWindow, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
		auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
		thisWindow->handleKeyEvents(key, scancode, action, mods);
		});

	glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* win, int button, int action, int mods) {
		auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
		thisWindow->handleMouseButtonEvents(button, action, mods);
		});
	
	// glfwMakeContextCurrent(mWindow); // Use OpenGL
	if (!initVulkan()) {
		Logger::log(1, "%s: Could not init Vulkan\n", __FUNCTION__);
		glfwTerminate();
		return false;
	}
	Logger::log(1, "%s: Window successfully initialized\n", __FUNCTION__);
	return true;
}

bool Window::initVulkan() {
	VkResult result = VK_ERROR_UNKNOWN;
	
	VkApplicationInfo mAppInfo{};
	mAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	mAppInfo.pApplicationName = "Hello Triangle";
	mAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	mAppInfo.pEngineName = "No Engine";
	mAppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	mAppInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo mCreateInfo{};
	mCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	mCreateInfo.pApplicationInfo = &mAppInfo;

	// Extensions
	uint32_t extensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
	if (extensionCount == 0) {
		Logger::log(1, "%s error: no Vulkan extensions found, need at least 'VK_KHR_SURFACE'\n", __FUNCTION__);
		return false;
	}
	Logger::log(1, "%s: Found %u Vulkan extensions\n", __FUNCTION__, extensionCount);
	for (int i = 0; i < extensionCount; ++i) {
		Logger::log(1, "%s: %s\n", __FUNCTION__, std::string(glfwExtensions[i]).c_str());
	}

	mCreateInfo.enabledExtensionCount = extensionCount;
	mCreateInfo.ppEnabledExtensionNames = glfwExtensions;
	mCreateInfo.enabledLayerCount = 0;

	result = vkCreateInstance(&mCreateInfo, nullptr, &mInstance);
	if (result != VK_SUCCESS) {
		Logger::log(1, "%s: Could not create Vulkan instance (%i)\n", __FUNCTION__, result);
		return false;
	}

	// count physical device
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, nullptr);
	if (physicalDeviceCount == 0) {
		Logger::log(1, "%s: No Vulkan capable GPU found\n", __FUNCTION__);
		return false;
	}
	// fill with data about GPU
	std::vector<VkPhysicalDevice> devices;
	vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, devices.data());

	// create surface
	result = glfwCreateWindowSurface(mInstance, mWindow, nullptr, &mSurface);
	if (result != VK_SUCCESS) {
		Logger::log(1, "%s: Could not create Vulkan surface\n", __FUNCTION__);
		return false;
	}
	return true;
}

void Window::handleWindowCloseEvents() {
	Logger::log(1, "%s: Window got close event...bye!\n", __FUNCTION__);
}

void Window::handleKeyEvents(int key, int scancode, int action, int mods) {
	std::string actionName;
	switch (action) {
	case GLFW_PRESS:
		actionName = "pressed";
		break;
	case GLFW_RELEASE:
		actionName = "released";
		break;
	case GLFW_REPEAT:
		actionName = "repeated";
		break;
	default:
		actionName = "invalid";
		break;
	}
	const char* keyName = glfwGetKeyName(key, 0);
	Logger::log(1, "%s: key %s (key %i, scancode %i) %s\n", __FUNCTION__, keyName, key, scancode, actionName.c_str());
}

void Window::handleMouseButtonEvents(int button, int action, int mods) {
	std::string actionName;
	switch (action) {
	case GLFW_PRESS:
		actionName = "pressed";
		break;
	case GLFW_RELEASE:
		actionName = "released";
		break;
	default:
		actionName = "invalid";
		break;
	}

	std::string mouseButtonName;
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		mouseButtonName = "left";
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		mouseButtonName = "middle";
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		mouseButtonName = "right";
		break;
	default:
		mouseButtonName = "other";
		break;
	}
	Logger::log(1, "%s: %s mouse button (%i) %s\n", __FUNCTION__, mouseButtonName.c_str(), button, actionName.c_str());
}

void Window::mainLoop() {
	glfwSwapInterval(1);
	float color = 0.0f;
	while (!glfwWindowShouldClose(mWindow)) {
		color >= 1.0f ? color = 0.0f : color += 0.01f;
		glClearColor(color, color, color, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}
}

void Window::cleanup() {
	Logger::log(1, "%s: Terminating Window\n", __FUNCTION__);
	vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
	vkDestroyInstance(mInstance, nullptr);
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

