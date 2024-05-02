#pragma once
#include <vector>
#include <glm/glm.hpp>
//#include "OGLRenderData.h"
#include "VkRenderData.h"

class Model {
public:
	void init();
	//OGLMesh getVertexData();
	VkMesh getVertexData();
private:
	//OGLMesh mVertexData;
	VkMesh mVertexData;
};