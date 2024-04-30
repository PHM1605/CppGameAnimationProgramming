#include "VertexBuffer.h"

void VertexBuffer::init() {
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVertexVBO);
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OGLVertex), (void*)offsetof(OGLVertex, position));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(OGLVertex), (void*)offsetof(OGLVertex, uv));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void VertexBuffer::cleanup() {
	glDeleteBuffers(1, &mVertexVBO);
	glDeleteVertexArrays(1, &mVAO);
}

