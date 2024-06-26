#include <fstream>
#include <vector>
#include "Shader.h"
#include "Logger.h"
#define _CRT_SECURE_NO_WARNINGS
/*
bool Shader::loadShaders(std::string vertexShaderFileName, std::string fragmentShaderFileName) {
	Logger::log(1, "%s: loading vertex shader '%s' and fragment shader '%s'\n", __FUNCTION__, vertexShaderFileName.c_str(), fragmentShaderFileName.c_str());
	if (!createShaderProgram(vertexShaderFileName, fragmentShaderFileName)) {
		Logger::log(1, "%s error: shader program creation failed\n", __FUNCTION__);
		return false;
	}
	return true;	
}

void Shader::use() {
	glUseProgram(mShaderProgram);
}

GLuint Shader::loadShader(std::string shaderFileName, GLuint shaderType) {
	std::string shaderAsText = loadFileToString(shaderFileName);
	Logger::log(4, "%s: loaded shader file '%s', size %i\n", __FUNCTION__, shaderFileName.c_str(), shaderAsText.size());
	const char* shaderSource = shaderAsText.c_str();
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, (const GLchar**)&shaderSource, 0);
	glCompileShader(shader);
	if (!checkCompileStats(shaderFileName, shader)) {
		Logger::log(1, "%s error: compiling shader '%s' failed\n", __FUNCTION__, shaderFileName.c_str());
		return 0;
	}
	Logger::log(1, "%s: shader %#x loaded and compiled\n", __FUNCTION__, shader);
	return shader;
}

bool Shader::createShaderProgram(std::string vertexShaderFileName, std::string fragmentShaderFileName) {
	GLuint vertexShader = loadShader(vertexShaderFileName, GL_VERTEX_SHADER);
	if (!vertexShader) {
		return false;
	}
	GLuint fragmentShader = loadShader(fragmentShaderFileName, GL_FRAGMENT_SHADER);
	if (!fragmentShader) {
		return false;
	}
	mShaderProgram = glCreateProgram();
	glAttachShader(mShaderProgram, vertexShader);
	glAttachShader(mShaderProgram, fragmentShader);
	glLinkProgram(mShaderProgram);

	// Checking
	if (!checkLinkStats(vertexShaderFileName, fragmentShaderFileName, mShaderProgram)) {
		Logger::log(1, "%s error: program linking from vertex shader '%s' / fragment shader '%s' failed\n", __FUNCTION__, vertexShaderFileName.c_str(), fragmentShaderFileName.c_str());
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return false;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	Logger::log(1, "%s: shader program %#x successfully compiled from vertex shader '%s' and fragment shader '%s'\n", __FUNCTION__, mShaderProgram, vertexShaderFileName.c_str(), fragmentShaderFileName.c_str());
	return true;
}

void Shader::cleanup() {
	glDeleteProgram(mShaderProgram);
}

bool Shader::checkCompileStats(std::string shaderFileName, GLuint shader) {
	GLint isShaderCompiled;
	int logMessageLength;
	std::vector<char> shaderLog;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isShaderCompiled);
	if (!isShaderCompiled) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logMessageLength);
		shaderLog = std::vector<char>(logMessageLength + 1);
		glGetShaderInfoLog(shader, logMessageLength, &logMessageLength, shaderLog.data());
		shaderLog.at(logMessageLength) = '\0';
		Logger::log(1, "%s error: shader compile of shader '%s' failed\n", __FUNCTION__, shaderFileName.c_str());
		Logger::log(1, "%s compile log:\n%s\n", __FUNCTION__, shaderLog.data());
		return false;
	}
	return true;
}

bool Shader::checkLinkStats(std::string vertexShaderFileName, std::string fragmentShaderFileName, GLuint shaderProgram) {
	GLint isProgramLinked;
	int logMessageLength;
	std::vector<char> programLog;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isProgramLinked);
	if (!isProgramLinked) {
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logMessageLength);
		programLog = std::vector<char>(logMessageLength + 1);
		glGetProgramInfoLog(shaderProgram, logMessageLength, &logMessageLength, programLog.data());
		programLog.at(logMessageLength) = '\0';
		Logger::log(1, "%s error: program linking of shaders '%s' and '%s' failed\n", __FUNCTION__, vertexShaderFileName.c_str(), fragmentShaderFileName.c_str());
		Logger::log(1, "%s compile log:\n%s\n", __FUNCTION__, programLog.data());
		return false;
	}
	return true;
}
*/

VkShaderModule Shader::loadShader(VkDevice device, std::string shaderFileName) {
	std::string shaderAsText = loadFileToString(shaderFileName);
	VkShaderModuleCreateInfo shaderCreateInfo{};
	shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderCreateInfo.codeSize = shaderAsText.size();
	shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderAsText.c_str());
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &shaderCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		Logger::log(1, "%s: could not load shader'%s'\n", __FUNCTION__, shaderFileName.c_str());
		return VK_NULL_HANDLE;
	}
	return shaderModule;
}

std::string Shader::loadFileToString(std::string fileName) {
	std::ifstream inFile(fileName);
	std::string str;
	// Read shader file
	if (inFile.is_open()) {
		str.clear();
		inFile.seekg(0, std::ios::end);
		str.reserve(inFile.tellg());
		inFile.seekg(0, std::ios::beg);
		str.assign(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>());
		inFile.close();
	}
	else {
		Logger::log(1, "%s error: could not open file %s\n", __FUNCTION__, fileName.c_str());
		return std::string();
	}

	if (inFile.bad() || inFile.fail()) {
		Logger::log(1, "%s error: error while reading file %s\n", __FUNCTION__, fileName.c_str());
		inFile.close();
		return std::string();
	}

	// Closing when good
	inFile.close();
	Logger::log(1, "%s: file %s successfully read to string\n", __FUNCTION__, fileName.c_str());
	return str;
}