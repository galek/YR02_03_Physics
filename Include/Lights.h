#pragma once

#include <glm/ext.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Shader.h>

class Lights{
public:
	Lights(int i_maxLights = 5);
	~Lights(){};

	int AddDirectional(glm::vec3 a_Direction,glm::vec4 a_Colour = glm::vec4(1,1,1,1));
	int AddPoint(glm::vec3 a_Position,float a_fDistance,glm::vec4 a_Colour = glm::vec4(1,1,1,1));
	int AddSpot(glm::vec3 a_Position,glm::vec3 a_Direction,float a_fDistance,float f_Angle,glm::vec4 a_Colour = glm::vec4(1,1,1,1));

	void DirectionalMove(int ID,glm::vec3 v3_Pos);
	void DirectionalColour(int ID,glm::vec4 v4_Colour);

	void PointMove(int ID,glm::vec3 v3_Pos);
	void PointColour(int ID,glm::vec4 v4_Colour);
	void PointDistance(int ID,float f_Distance);

	void SpotMove(int ID,glm::vec3 v3_Pos);
	void SpotColour(int ID,glm::vec4 v4_Colour);
	void SpotDistance(int ID,float f_Distance);
	void SpotAngle(int ID,float f_Angle);
	void SpotDirection(int ID,glm::vec3 v3_Direction);

	void Clear();
	void SetAmbient(glm::vec3 v3_Ambient);
	void SetSpecular(glm::vec3 v3_Specular);
	void UpdateUniforms(Shader* s_LightShader);
	void Draw();

protected:
	int imaxLights;

	int iCurrentDirectional;
	int iCurrentPoint;
	int iCurrentSpot;

	float *fpDirectionalPosition;
	float *fpDirectionalColour;

	float *fpPointPosition;
	float *fpPointColour;
	float *fPointDistance;

	float *fpSpotPosition;
	float *fpSpotColour;
	float *fSpotDistance;
	float *fSpotAngle;
	float *fSpotDirection;

	float fAmbient[3];
	float fSpecular[3];

};

class DirectionalLight { 
public: 
	glm::vec3 Direction;
	glm::vec4 Colour;

	DirectionalLight(glm::vec3 a_Direction,glm::vec4 a_Colour = glm::vec4(1,1,1,1)){
		Direction = a_Direction;
		Colour = a_Colour;
	}
	DirectionalLight(void){};
	~DirectionalLight(void){};

	void Update(GLFWwindow* a_window, float a_deltaTime);
	void Draw();
};

class PointLight {
public:
	glm::vec3 Position;
	glm::vec4 Colour;
	float fDistance;

	PointLight(glm::vec3 a_Position,float a_fDistance,glm::vec4 a_Colour = glm::vec4(1,1,1,1)){
		Position = a_Position;
		fDistance = a_fDistance;
		Colour = a_Colour;
	}
	PointLight(void){};
	~PointLight(void){};

	void Update(GLFWwindow* a_window, float a_deltaTime);
	void Draw();
};

class SpotLight {
public:
	glm::vec3 Position,Direction;
	glm::vec4 Colour;
	float fDistance,fAngle;

	SpotLight(glm::vec3 a_Position,glm::vec3 a_Direction,float a_fDistance,float f_Angle,glm::vec4 a_Colour = glm::vec4(1,1,1,1)){
		Direction = a_Direction;
		Position = a_Position;
		fDistance = a_fDistance;
		Colour = a_Colour;
		fAngle = f_Angle;
	}
	SpotLight(void){};
	~SpotLight(void){};

	void Update(GLFWwindow* a_window, float a_deltaTime);
	void Draw();
};