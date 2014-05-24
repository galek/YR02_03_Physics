#pragma once

#include "BasicVertex.h"
#include "Shader.h"

class Billboard {
public:
	Billboard(glm::vec3 Position,glm::vec2 Size);
	Billboard(){}
	~Billboard(){}
	void Update(Shader *a_Shader,glm::mat4 a_cameraTransform);
	void Create(glm::vec3 Position,glm::vec2 Size);
	void Draw();

	glm::vec3 position;
private:
	//OpenGL
	BasicVertex m_vertexData[4];
	unsigned int m_vao,m_vbo,m_ibo;
};

