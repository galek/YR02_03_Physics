#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <stdio.h>
#include <glm/ext.hpp>
#include <SOIL.h>

#include "Utilities.h"

static double s_prevTime = 0;
static float s_totalTime = 0;
static float s_deltaTime = 0;
static float fVelocity = 0;
static bool bCanJump;
static bool bGravMove,bToggle;

unsigned int Utility::Width = 1280;
unsigned int Utility::Height = 720;

void Utility::resetTimer()
{
	s_prevTime = glfwGetTime();
	s_totalTime = 0;
	s_deltaTime = 0;
}

float Utility::tickTimer()
{
	double currentTime = glfwGetTime();
	s_deltaTime = (float)(currentTime - s_prevTime);
	s_totalTime += s_deltaTime;
	s_prevTime = currentTime;
	return s_deltaTime;
}

float Utility::getDeltaTime()
{
	return s_deltaTime;
}

float Utility::getTotalTime()
{
	return s_totalTime;
}

void Utility::getCursorPos(int &xx, int &yy){
	double x,y;
	glfwGetCursorPos(glfwGetCurrentContext(),&x,&y);
	xx = (int)x; 
	yy = (int)y;
}

void Utility::ConvertScreenToView(float &x, float &y){
	x /= Width * 0.5f;
	x -= 1.0f;
	y /= Height * 0.5f;
	y -= 1.0f;
	y *= -1;
}

void Utility::ConvertScreenToViewSize(float &x, float &y){
	x /= Width;
	y /= Height;
}

glm::vec3 Utility::RayCast(glm::mat4 &view,glm::mat4 &proj){
	int xx,yy;
	getCursorPos(xx,yy);
	float x = (2.0f * xx) / Width - 1.0f;
	float y = 1.0f - (2.0f * yy) / Height;

	glm::vec4 ray_clip = glm::vec4(x,y,-1.0f,1.0f);
	glm::vec4 ray_eye = (glm::inverse(proj) * ray_clip);
	ray_eye = glm::vec4 (ray_eye.xy, -1.0, 0.0);
	glm::vec3 ray_wor = (glm::inverse (view) * ray_eye).xyz;
	return glm::normalize(ray_wor);
}

// builds a simple 2 triangle quad with a position, colour and texture coordinates
void Utility::build3DPlane(float a_size, unsigned int& a_vao, unsigned int& a_vbo, unsigned int& a_ibo, const glm::vec4& a_colour /* = glm::vec4(1,1,1,1) */ )
{
	float halfSize = a_size * 0.5f;

	// create mesh
	BasicVertex vertices[4];
	vertices[0].position = glm::vec4(-halfSize,0,-halfSize,1);
	vertices[0].colour = a_colour;
	vertices[0].texCoord = glm::vec2(0,0);
	vertices[1].position = glm::vec4(halfSize,0,-halfSize,1);
	vertices[1].colour = a_colour;
	vertices[1].texCoord = glm::vec2(1,0);
	vertices[2].position = glm::vec4(halfSize,0,halfSize,1);
	vertices[2].colour = a_colour;
	vertices[2].texCoord = glm::vec2(1,1);
	vertices[3].position = glm::vec4(-halfSize,0,halfSize,1);
	vertices[3].colour = a_colour;
	vertices[3].texCoord = glm::vec2(0,1);

	unsigned int indices[6] = {
		3,1,0,
		3,2,1 
	};

	// create and bind buffers to a vertex array object
	glGenBuffers(1, &a_vbo);
	glGenBuffers(1, &a_ibo);
	glGenVertexArrays(1, &a_vao);

	glBindVertexArray(a_vao);
	glBindBuffer(GL_ARRAY_BUFFER, a_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, a_ibo);

	glBufferData(GL_ARRAY_BUFFER, 4* sizeof(BasicVertex), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::PositionOffset);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::ColourOffset);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::TexCoordOffset);

	// unbind vertex array
	glBindVertexArray(0);
}

// loads a shader from a file and creates it for the specified stage
unsigned int Utility::loadShader(const char* a_filename, unsigned int a_type)
{
	int success = GL_FALSE;

	unsigned char* source = fileToBuffer(a_filename);
	unsigned int handle = glCreateShader(a_type);

	glShaderSource(handle, 1, (const char**)&source, 0);
	glCompileShader(handle);

	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;		
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetShaderInfoLog(handle, infoLogLength, 0, infoLog);
		printf("Error: Failed to compile shader!\n");
		printf("%s",infoLog);
		printf("\n");
		delete[] infoLog;
		return 0;
	}

	return handle;
}

// creates a shader program, links the specified shader stages to it, and binds the specified input/output attributes
unsigned int Utility::createProgram(unsigned int a_vertexShader, unsigned int a_controlShader, unsigned int a_evaluationShader, unsigned int a_geometryShader, unsigned int a_fragmentShader,
								  unsigned int a_inputAttributeCount /* = 0 */, const char** a_inputAttributes /* = nullptr */,
								  unsigned int a_outputAttributeCount /* = 0 */, const char** a_outputAttributes /* = nullptr */)
{
	int success = GL_FALSE;

	// create a shader program and attach the shaders to it
	unsigned int handle = glCreateProgram();
	glAttachShader(handle, a_vertexShader);
	glAttachShader(handle, a_controlShader);
	glAttachShader(handle, a_evaluationShader);
	glAttachShader(handle, a_geometryShader);
	glAttachShader(handle, a_fragmentShader);

	// specify vertex input attributes
	for ( unsigned int i = 0 ; i < a_inputAttributeCount ; ++i )
		glBindAttribLocation(handle, i, a_inputAttributes[i]);

	// specify fragment shader outputs
	for ( unsigned int i = 0 ; i < a_outputAttributeCount ; ++i )
		glBindFragDataLocation(handle, i, a_outputAttributes[i]);

	// link the program together and log errors
	glLinkProgram(handle);

	glGetProgramiv(handle, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;		
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetShaderInfoLog(handle, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s",infoLog);
		printf("\n");
		delete[] infoLog;
		return 0;
	}

	return handle;
}

unsigned char* Utility::fileToBuffer(const char* a_sPath)
{
	// open file for text reading
	FILE* pFile = fopen(a_sPath,"rb");
	if (pFile == nullptr)
	{
		printf("Error: Unable to open file '%s' for reading!\n",a_sPath);
		return nullptr;
	}

	// get number of bytes in file
	fseek(pFile, 0, SEEK_END);
	unsigned int uiLength = (unsigned int)ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	// allocate buffer and read file contents
	unsigned char* acBuffer = new unsigned char[uiLength + 1];
	memset(acBuffer,0,uiLength + 1);
	fread(acBuffer, sizeof(unsigned char), uiLength, pFile);

	fclose(pFile);
	return acBuffer;
}

//Below has doubled up code.... needs proper wrapping

void Utility::freeMovement(glm::mat4& a_transform, float a_deltaTime, float a_speed, const glm::vec3& a_up /* = glm::vec3(0,1,0) */)
{
	GLFWwindow* window = glfwGetCurrentContext();

	// Get the camera's forward, right, up, and location vectors
	glm::vec4 vForward = a_transform[2];
	glm::vec4 vRight = a_transform[0];
	glm::vec4 vUp = a_transform[1];
	glm::vec4 vTranslation = a_transform[3];

	float frameSpeed = glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? a_deltaTime * a_speed * 2 : a_deltaTime * a_speed;	

	// Translate camera
	if (glfwGetKey(window,'W') == GLFW_PRESS)
	{
		vTranslation -= vForward * frameSpeed;
	}else if (glfwGetKey(window,'S') == GLFW_PRESS)
	{
		vTranslation += vForward * frameSpeed;
	}if (glfwGetKey(window,'D') == GLFW_PRESS)
	{
		vTranslation += vRight * frameSpeed;
	}else if (glfwGetKey(window,'A') == GLFW_PRESS)
	{
		vTranslation -= vRight * frameSpeed;
	}
	if (glfwGetKey(window,GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		vTranslation += vUp * frameSpeed;
	}else if (glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		vTranslation -= vUp * frameSpeed;
	}

	a_transform[3] = vTranslation;

	// check for camera rotation

	double siPrevMouseX = 1280/2;
	double siPrevMouseY = 720/2;

	if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_2)){
		if (!bToggle){
			bToggle = true;
			bGravMove = !bGravMove;
		}
		if (bGravMove){
			glfwSetCursorPos(window,siPrevMouseX,siPrevMouseY);
			glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_HIDDEN);
		} else {
			glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
		}
	}else{
		bToggle = false;
	}

	if (bGravMove){
		double mouseX = 0;
		double mouseY = 0;

		glfwGetCursorPos(window,&mouseX,&mouseY);
		glfwSetCursorPos(window,siPrevMouseX,siPrevMouseY);

		double iDeltaX = mouseX - siPrevMouseX;
		double iDeltaY = mouseY - siPrevMouseY;

		glm::mat4 mMat;
		
		// pitch
		if (iDeltaY != 0){
			mMat = glm::axisAngleMatrix( vRight.xyz(), (float)-iDeltaY / 150.0f );
			vRight = mMat * vRight;
			vUp = mMat * vUp;
			vForward = mMat * vForward;
		}

		// yaw
		if (iDeltaX != 0){
			mMat = glm::axisAngleMatrix( a_up, (float)-iDeltaX / 150.0f );
			vRight = mMat * vRight;
			vUp = mMat * vUp;
			vForward = mMat * vForward;
		}

		a_transform[0] = vRight;
		a_transform[1] = vUp;
		a_transform[2] = vForward;
	}
}

void Utility::GravMovement(glm::mat4& a_transform, float a_deltaTime, float a_speed, float floorPos, const glm::vec3& a_up /* = glm::vec3(0,1,0) */) {
	GLFWwindow* window = glfwGetCurrentContext();

	// Get the camera's forward, right, up, and location vectors
	glm::vec4 vForward = a_transform[2];
	glm::vec4 vRight = a_transform[0];
	glm::vec4 vUp = a_transform[1];
	glm::vec4 vTranslation = a_transform[3];

	float frameSpeed = glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? a_deltaTime * a_speed * 2 : a_deltaTime * a_speed;	

	// Translate camera
	glm::vec3 HitLocation;
	if (glfwGetKey(window,'W') == GLFW_PRESS){
		vTranslation.xz -= vForward.xz * frameSpeed;
	}else if (glfwGetKey(window,'S') == GLFW_PRESS){
		vTranslation.xz += vForward.xz * frameSpeed;
	}

	if (glfwGetKey(window,'D') == GLFW_PRESS){
		vTranslation.xz += vRight.xz * frameSpeed;
	}else if (glfwGetKey(window,'A') == GLFW_PRESS){
		vTranslation.xz -= vRight.xz * frameSpeed;
	}

	a_transform[3] = vTranslation;

	fVelocity -= a_deltaTime * a_speed * 0.5f;

	if (a_transform[3].y <= floorPos + 1.5f){
		a_transform[3].y = floorPos + 1.5f;
		bCanJump = true;
		fVelocity = 0.0f;
	}

	if (glfwGetKey(window,GLFW_KEY_SPACE) && bCanJump) {
		bCanJump = false;
		fVelocity = 300.0f * a_deltaTime * a_speed;
	}

	a_transform[3].y += fVelocity * a_deltaTime;

	// check for camera rotation
	double siPrevMouseY = 720/2;
	double siPrevMouseX = 1280/2;

	if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_2)){
		if (!bToggle){
			bToggle = true;
			bGravMove = !bGravMove;
		}
		if (bGravMove){
			glfwSetCursorPos(window,siPrevMouseX,siPrevMouseY);
			glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_HIDDEN);
		} else {
			glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
		}
	}else{
		bToggle = false;
	}

	if (bGravMove){
		double mouseX = 0;
		double mouseY = 0;

		glfwGetCursorPos(window,&mouseX,&mouseY);
		glfwSetCursorPos(window,siPrevMouseX,siPrevMouseY);

		double iDeltaX = mouseX - siPrevMouseX;
		double iDeltaY = mouseY - siPrevMouseY;

		glm::mat4 mMat;
		
		// pitch
		if (iDeltaY != 0){
			mMat = glm::axisAngleMatrix( vRight.xyz(), (float)-iDeltaY / 150.0f );
			vRight = mMat * vRight;
			vUp = mMat * vUp;
			vForward = mMat * vForward;
		}

		// yaw
		if (iDeltaX != 0){
			mMat = glm::axisAngleMatrix( a_up, (float)-iDeltaX / 150.0f );
			vRight = mMat * vRight;
			vUp = mMat * vUp;
			vForward = mMat * vForward;
		}

		a_transform[0] = vRight;
		a_transform[1] = vUp;
		a_transform[2] = vForward;
	}
}
	
void Utility::CollMapMove(glm::mat4& a_transform, float a_deltaTime, float a_speed, CollisionMap *c_Map, const glm::vec3& a_up /* = glm::vec3(0,1,0) */) {
	GLFWwindow* window = glfwGetCurrentContext();

	// Get the camera's forward, right, up, and location vectors
	glm::vec4 vForward = a_transform[2];
	glm::vec4 vRight = a_transform[0];
	glm::vec4 vUp = a_transform[1];
	glm::vec4 vTranslation = a_transform[3];

	float frameSpeed = glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? a_deltaTime * a_speed * 2 : a_deltaTime * a_speed;	

	//HitChecking
	glm::vec3 HitDirection,HitLoop;
	c_Map->CheckMap((glm::vec3)vTranslation.xyz - (vForward.xyz * frameSpeed * 2.0f),HitDirection);
	c_Map->CheckMap((glm::vec3)vTranslation.xyz + (vForward.xyz * frameSpeed * 2.0f),HitLoop);HitDirection+=HitLoop;
	c_Map->CheckMap((glm::vec3)vTranslation.xyz - (vRight.xyz	* frameSpeed * 2.0f),HitLoop);HitDirection+=HitLoop;
	c_Map->CheckMap((glm::vec3)vTranslation.xyz + (vRight.xyz	* frameSpeed * 2.0f),HitLoop);HitDirection+=HitLoop;
	c_Map->CheckMap((glm::vec3)vTranslation.xyz - (vUp.xyz		* frameSpeed * 2.0f),HitLoop);HitDirection+=HitLoop;
	c_Map->CheckMap((glm::vec3)vTranslation.xyz + (vUp.xyz		* frameSpeed * 2.0f),HitLoop);HitDirection+=HitLoop;

	//Forward/Backward
	if (glfwGetKey(window,'W') == GLFW_PRESS && HitDirection.z >= 0){
		vTranslation.xz -= vForward.xz * frameSpeed;
	}else if (glfwGetKey(window,'S') == GLFW_PRESS && HitDirection.z <= 0){
		vTranslation.xz += vForward.xz * frameSpeed;
	}
	//Left/Right
	if (glfwGetKey(window,'A') == GLFW_PRESS && HitDirection.x >= 0){
		vTranslation.xz -= vRight.xz * frameSpeed;
	}else if (glfwGetKey(window,'D') == GLFW_PRESS && HitDirection.x <= 0){
		vTranslation.xz += vRight.xz * frameSpeed;
	} 
	//Gravity/Jumping
	a_transform[3] = vTranslation;

	fVelocity -= a_deltaTime * a_speed * 0.25f;

	if (a_transform[3].y <= 0 + 1.5f){
		a_transform[3].y = 0 + 1.5f;
		bCanJump = true;
		fVelocity = 0.0f;
	}

	if (glfwGetKey(window,GLFW_KEY_SPACE) && bCanJump) {
		bCanJump = false;
		fVelocity = 500.0f * a_deltaTime * a_speed;
	}

	a_transform[3].y += fVelocity * a_deltaTime;

	// check for camera rotation
	double siPrevMouseY = 720/2;
	double siPrevMouseX = 1280/2;

	if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_2)){
		if (!bToggle){
			bToggle = true;
			bGravMove = !bGravMove;
		}
		if (bGravMove){
			glfwSetCursorPos(window,siPrevMouseX,siPrevMouseY);
			glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_HIDDEN);
		} else {
			glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
		}
	}else{
		bToggle = false;
	}

	if (bGravMove){
		double mouseX = 0;
		double mouseY = 0;

		glfwGetCursorPos(window,&mouseX,&mouseY);
		glfwSetCursorPos(window,siPrevMouseX,siPrevMouseY);

		double iDeltaX = mouseX - siPrevMouseX;
		double iDeltaY = mouseY - siPrevMouseY;

		glm::mat4 mMat;
		
		// pitch
		if (iDeltaY != 0){
			mMat = glm::axisAngleMatrix( vRight.xyz(), (float)-iDeltaY / 150.0f );
			vRight = mMat * vRight;
			vUp = mMat * vUp;
			vForward = mMat * vForward;
		}

		// yaw
		if (iDeltaX != 0){
			mMat = glm::axisAngleMatrix( a_up, (float)-iDeltaX / 150.0f );
			vRight = mMat * vRight;
			vUp = mMat * vUp;
			vForward = mMat * vForward;
		}

		a_transform[0] = vRight;
		a_transform[1] = vUp;
		a_transform[2] = vForward;
	}
}


