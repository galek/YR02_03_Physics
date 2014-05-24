#ifndef __Plane_H_
#define __Plane_H_

#include <glm/glm.hpp> //use of vec2,vec3,vec4 
#include "BasicVertex.h"

class Plane{
public:
	//here for standardised reasons, not used or needed
	Plane();
	Plane(glm::vec3 pos,glm::vec2 size,glm::vec2 subd ,bool b1to1 = false ,float fScale = 1.0f,glm::vec4 v4_Colour = glm::vec4(1));
	~Plane(void){};
	//Draw our plane
	void Draw();
	void DrawTess();
protected:
	unsigned int m_rows,m_columns;
	unsigned int m_VAO,m_VBO,m_IBO;
	BasicVertex* aoVertices;
};

#endif