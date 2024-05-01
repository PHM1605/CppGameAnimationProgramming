#include "Model.h"

void Model::init() {
	mVertexData.vertices.resize(6);
	mVertexData.vertices[0].position = glm::vec3(-0.5f, -0.5f, 0.5f);
	mVertexData.vertices[1].position = glm::vec3(0.5f, 0.5f, 0.5f);
	mVertexData.vertices[2].position = glm::vec3(-0.5f, 0.5f, 0.5f);
	mVertexData.vertices[3].position = glm::vec3(-0.5f, -0.5f, 0.5f);
	mVertexData.vertices[4].position = glm::vec3(0.5f, -0.5f, 0.5f);
	mVertexData.vertices[5].position = glm::vec3(0.5f, 0.5f, 0.5f);
	mVertexData.vertices[0].uv = glm::vec2(0.0f, 0.0f);
	mVertexData.vertices[1].uv = glm::vec2(1.0f, 1.0f);
	mVertexData.vertices[2].uv = glm::vec2(0.0f, 1.0f);
	mVertexData.vertices[3].uv = glm::vec2(0.0f, 0.0f);
	mVertexData.vertices[4].uv = glm::vec2(1.0f, 0.0f);
	mVertexData.vertices[5].uv = glm::vec2(1.0f, 1.0f);
}

OGLMesh Model::getVertexData() {
	return mVertexData;
}