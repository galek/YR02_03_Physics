#include "SpotLight.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Gizmos.h"

void SpotLight::Update(GLFWwindow* a_window, float a_deltaTime){
	if (glfwGetKey(a_window,GLFW_KEY_LEFT) == GLFW_PRESS){ 
		Position.x -= 10.0f* a_deltaTime;
	}if (glfwGetKey(a_window,GLFW_KEY_RIGHT) == GLFW_PRESS){ 
		Position.x += 10.0f* a_deltaTime;
	}

	if (glfwGetKey(a_window,GLFW_KEY_UP) == GLFW_PRESS){ 
		if (glfwGetKey(a_window,GLFW_KEY_RIGHT_SHIFT)){
		Position.y += 10.0f* a_deltaTime;
		}else{
		Position.z -= 10.0f* a_deltaTime;
		}
	}else if (glfwGetKey(a_window,GLFW_KEY_DOWN) == GLFW_PRESS){ 
		if (glfwGetKey(a_window,GLFW_KEY_RIGHT_SHIFT)){
		Position.y -= 10.0f* a_deltaTime;
		}else{
		Position.z += 10.0f* a_deltaTime;
		}
	}
}

void SpotLight::Draw(){
	Gizmos::addSphere(Position,12,12,0.1f,Colour);
}