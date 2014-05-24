#include "Billboard.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

Billboard::Billboard(glm::vec3 Position,glm::vec2 Size){
	Create(Position,Size);
}

void Billboard::Create(glm::vec3 Position,glm::vec2 Size){
	position = Position;
	m_vertexData[0].position = glm::vec4(-Size.x/2,0	 ,0,1); m_vertexData[0].colour = glm::vec4(1); m_vertexData[0].texCoord = glm::vec2(0,1);
	m_vertexData[1].position = glm::vec4( Size.x/2,0	 ,0,1); m_vertexData[1].colour = glm::vec4(1); m_vertexData[1].texCoord = glm::vec2(1,1);
	m_vertexData[2].position = glm::vec4( Size.x/2,Size.y,0,1); m_vertexData[2].colour = glm::vec4(1); m_vertexData[2].texCoord = glm::vec2(1,0);
	m_vertexData[3].position = glm::vec4(-Size.x/2,Size.y,0,1); m_vertexData[3].colour = glm::vec4(1); m_vertexData[3].texCoord = glm::vec2(0,0);

	unsigned int ind[6] = {0,1,2,0,2,3};

	// create opengl buffers
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);	

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(BasicVertex), m_vertexData, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), ind , GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::PositionOffset);
	glEnableVertexAttribArray(1); // colour
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::ColourOffset);
	glEnableVertexAttribArray(2); // UV
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::TexCoordOffset);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Billboard::Update(Shader* a_Shader,glm::mat4 a_Cam){
	// create billboard transform
	glm::vec3 zAxis = glm::normalize( a_Cam[3].xyz - position);//FORWARD
	glm::vec3 xAxis = glm::cross( glm::vec3(a_Cam[1].xyz), zAxis );//RIGHT
	glm::vec3 yAxis = glm::cross( zAxis, xAxis );//UP
	glm::mat4 billboard(glm::vec4(xAxis,0),glm::vec4(yAxis,0),glm::vec4(zAxis,0),glm::vec4(position,1));
	a_Shader->SetUniform("Model","m4fv",1,false,glm::value_ptr(billboard));
}

void Billboard::Draw(){
	// draw particles
	glBindVertexArray( m_vao );
	glDrawElements(GL_TRIANGLES, 6 , GL_UNSIGNED_INT, 0);
}