#include "Plane.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Plane::Plane(glm::vec3 pos,glm::vec2 size, glm::vec2 subd ,bool b1to1,float fScale ,glm::vec4 v4_Colour){
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_IBO);
	glGenBuffers(1, &m_VBO);

	if (subd.x == 0 || subd.y == 0){
		return;//we cant draw nothing!
	}

	//we add one to our local values as its done by vertex points not by actual squares
	m_rows		= (unsigned int)(subd.x + 1);
	m_columns	= (unsigned int)(subd.y + 1);

	//Setup our verticies
	aoVertices = new BasicVertex[ m_rows * m_columns ];
	for ( unsigned int r = 0 ; r < m_rows ; ++r ){
		for ( unsigned int c = 0 ; c < m_columns ; ++c ){
		aoVertices[ r * m_columns + c ].position = glm::vec4( (((float)c/(m_rows-1))*size.x) + pos.x, pos.y, (((float)r/(m_columns-1))*size.y) + pos.z, 1);
		//Create some arbitrary colour based off something 
		//that might not be related to tiling a texture
		aoVertices[ r * m_columns + c ].colour = v4_Colour;
		aoVertices[ r * m_columns + c ].normal = glm::vec4(0,1,0,1);
		//Texture
		if (b1to1){
			aoVertices[ r * m_columns + c ].texCoord = glm::vec2(r*fScale,c*fScale);
		}else{
			aoVertices[ r * m_columns + c ].texCoord = glm::vec2( (r/(float)m_rows)*2 ,(c/(float)m_columns)*2 );
			}
		aoVertices[ r * m_columns + c ].texCoord2 = glm::vec2( (r/(float)m_rows)*2 ,(c/(float)m_columns)*2 );
		}
	}

	// defining index count based off quad count (2 triangles per quad)
	unsigned int* auiIndices = new unsigned int[ (m_rows - 1) * (m_columns - 1) * 6 ];
	unsigned int index = 0;unsigned intindex = 0;
	for (unsigned int r=0;r<(m_rows -1);++r){
		for ( unsigned int c = 0 ; c < (m_columns - 1) ; ++c ){
		// triangle 1
		auiIndices[ index++ ] = (r + 1) * m_columns + (c + 1);
		auiIndices[ index++ ] = r * m_columns + c;
		auiIndices[ index++ ] = (r + 1) * m_columns + c;
		// triangle 2
		auiIndices[ index++ ] = r * m_columns + (c + 1);
		auiIndices[ index++ ] = r * m_columns + c;
		auiIndices[ index++ ] = (r + 1) * m_columns + (c + 1);
		}
	}

	//Add the following line to generate a VertexArrayObject
	glBindVertexArray( m_VAO );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (m_rows - 1) * (m_columns - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);

	// create and bind buffers to a vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (m_rows * m_columns) * sizeof(BasicVertex), aoVertices, GL_STATIC_DRAW);

	//where to send information to what part of the shader
	//Vertex
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::PositionOffset);
	//Colour/normals(depending on shader implementation)/needs proper wrappings
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::ColourOffset);
	//Normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::NormalOffset);
	//TexCoord1
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::TexCoordOffset);
	//TexCoord2
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::TexCoord2Offset);

	//Set eveything back to nothing
	glBindVertexArray(0);
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	//dont need CPU sided verts anymore, delete them
	delete[] aoVertices;
}

void Plane::Draw(){
	//bind the VAO for this set of information and draw it
	glBindVertexArray(m_VAO);
	//dynamically get the amound of indicies
	unsigned int indexCount = (m_rows - 1) * (m_columns - 1) * 6;
	//Draw using triangles
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

void Plane::DrawTess(){
	glBindVertexArray(m_VAO);
	glPatchParameteri(GL_PATCH_VERTICES, 3);	
	unsigned int indexCount = (m_rows - 1) * (m_columns - 1) * 6;
	glDrawElements(GL_PATCHES, indexCount, GL_UNSIGNED_INT, nullptr);
}