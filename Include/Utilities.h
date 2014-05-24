#ifndef __UTILITIES_H_
#define __UTILITIES_H_

#include <glm/glm.hpp>
#include "BasicVertex.h"
#include "CollisionMap.h"

/*
// a basic vertex structure supporting position, colour and texture coordinate
struct BasicVertex
{
	glm::vec4 position;
	glm::vec4 colour;
	glm::vec2 texCoord;
};
*/

// a utility class with static helper methods
class Utility
{
public:
	static unsigned int Width,Height;

	// utilities for timing; Get() updates timers and returns time since last Get call
	static void		resetTimer();
	static float	tickTimer();
	static float	getDeltaTime();
	static float	getTotalTime();
	static void		getCursorPos(int &xx, int &yy);
	static void		ConvertScreenToView(float &x, float &y);
	static void		ConvertScreenToViewSize(float &x, float &y);
	static glm::vec3 RayCast(glm::mat4 &view, glm::mat4 &proj);

	// utility for mouse / keyboard movement of a matrix transform (suitable for camera)
	static void		freeMovement(glm::mat4& a_transform, float a_deltaTime, float a_speed, const glm::vec3& a_up = glm::vec3(0,1,0));
	static void		GravMovement(glm::mat4& a_transform, float a_deltaTime, float a_speed, float floorPos = 0.0f,const glm::vec3& a_up = glm::vec3(0,1,0));
	static void		CollMapMove (glm::mat4& a_transform, float a_deltaTime, float a_speed, CollisionMap *c_Map, const glm::vec3& a_up = glm::vec3(0,1,0));

	// loads a shader from a file and creates it for the specified stage
	static unsigned int	loadShader(const char* a_filename, unsigned int a_type);

	// creates a shader program, links the specified shader stages to it, and binds the specified input/output attributes if they are used
	static unsigned int	createProgram(unsigned int a_vertexShader, unsigned int a_controlShader, unsigned int a_evaluationShader, unsigned int a_geometryShader, unsigned int a_fragmentShader,
									  unsigned int a_inputAttributeCount = 0, const char** a_inputAttributes = nullptr,
									  unsigned int a_outputAttributeCount = 0, const char** a_outputAttributes = nullptr);

	// helper function for loading shader code into memory
	static unsigned char*	fileToBuffer(const char* a_szPath);

	// builds a textured plane out of 2 triangles and fills in the vertex array object, vertex buffer object, and index buffer object
	static void		build3DPlane(float a_size, unsigned int& a_vao, unsigned int& a_vbo, unsigned int& a_ibo, const glm::vec4& a_colour = glm::vec4(1,1,1,1));

};

#endif // __UTILITIES_H_