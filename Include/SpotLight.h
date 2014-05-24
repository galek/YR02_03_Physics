#pragma once

#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class SpotLight {
public:
	glm::vec3 Position;
	glm::vec4 Colour;
	float fDistance;

	SpotLight(glm::vec3 a_Position,float a_fDistance,glm::vec4 a_Colour = glm::vec4(1,1,1,1)){Position = a_Position;fDistance = a_fDistance;Colour = a_Colour;}
	~SpotLight(void){};

	void Update(GLFWwindow* a_window, float a_deltaTime);
	void Draw();
};

