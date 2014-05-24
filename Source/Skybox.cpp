#include "Skybox.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

Skybox::Skybox(float size,float f_HeightOff,bool Inverted) {
	Model[3].y = f_HeightOff;
	// Genorate our cube
	// cube vertices for vertex buffer object
	float cube_vertices[] = {
	//	x,	  y,	z
	-size,  size,  size,
	-size, -size,  size,
	 size, -size,  size,
	 size,  size,  size,
	-size,  size, -size,
	-size, -size, -size,
	 size, -size, -size,
	 size,  size, -size,
	};

	unsigned short *cube_indices;
	if (!Inverted){
		unsigned short cube_inds[] = {
			0, 1, 2,	0, 2, 3,
			3, 2, 6,	3, 6, 7,
			7, 6, 5,	7, 5, 4,
			4, 5, 1,	4, 1, 0,
			0, 3, 7,	0, 7, 4,
			1, 6, 2,	1, 5, 6,
		}; cube_indices = cube_inds;
	}else{
		unsigned short cube_inds[] = {
			0, 2, 1,	0, 3, 2, 
			3, 6, 2,	3, 7, 6, 
			7, 5, 6,	7, 4, 5, 
			4, 1, 5,	4, 0, 1, 
			0, 7, 3,	0, 4, 7, 
			1, 2, 6,	1, 6, 5, 
		}; cube_indices = cube_inds;
	}

	// OPENGL: genorate the VBO, IBO and VAO
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);
	glGenVertexArrays(1, &m_vao);

	// send the data
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned short), cube_indices, GL_STATIC_DRAW);

	// setup vertex attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
}


Skybox::~Skybox(void) {

}

void Skybox::Draw(CubeMap* a_Texture,Shader* a_Shader,const glm::mat4 &VP){
	// draw the skybox
	glUseProgram( a_Shader->id );

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, a_Texture->ID());
	
	glUniformMatrix4fv( glGetUniformLocation(a_Shader->id, "ModelViewProjection"), 1, false, glm::value_ptr(VP * Model) );
	glUniform1i( glGetUniformLocation(a_Shader->id, "CubeMap"), 1);

	glBindVertexArray( m_vao );
	glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_SHORT, 0);
}