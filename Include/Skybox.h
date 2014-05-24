#pragma once

#include "BasicVertex.h"
#include <Shader.h>
#include <Texture.h>
#include <glm\ext.hpp>

class Skybox
{
public:
	Skybox(float size,float fHeightOff = 0.0f,bool Inverted = true);
	~Skybox(void);
	void Draw(CubeMap* a_Texture,Shader* a_Shader,const glm::mat4 &VP);

	glm::vec3 position;
	glm::mat4 Model;
private:
	//OpenGL
	unsigned int m_vao,m_vbo,m_ibo;
};

