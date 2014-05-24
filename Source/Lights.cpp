#include "Lights.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Gizmos.h"

Lights::Lights(int i_maxLights){
	imaxLights = i_maxLights;

	fpDirectionalPosition = new float[imaxLights * 3];
	fpDirectionalColour = new float[imaxLights * 4];

	fpPointPosition = new float[imaxLights * 3];
	fpPointColour = new float[imaxLights * 4];
	fPointDistance = new float[imaxLights];

	fpSpotPosition = new float[imaxLights * 3];
	fpSpotColour = new float[imaxLights * 4];
	fSpotDistance = new float[imaxLights];
	fSpotAngle = new float[imaxLights];
	fSpotDirection = new float[imaxLights * 3];

	fAmbient[0]		= fAmbient[1]	 = fAmbient[2]	= 0.0f;
	fSpecular[0]	= fSpecular[1]	 = fSpecular[2] = 0.0f;

	iCurrentDirectional = 0;
	iCurrentPoint = 0;
	iCurrentSpot = 0;
}

int Lights::AddDirectional(glm::vec3 a_Direction,glm::vec4 a_Colour){
	fpDirectionalPosition[(iCurrentDirectional * 3) + 0] = a_Direction.x;
	fpDirectionalPosition[(iCurrentDirectional * 3) + 1] = a_Direction.y;
	fpDirectionalPosition[(iCurrentDirectional * 3) + 2] = a_Direction.z;

	fpDirectionalColour[(iCurrentDirectional * 4) + 0] = a_Colour.x;
	fpDirectionalColour[(iCurrentDirectional * 4) + 1] = a_Colour.y;
	fpDirectionalColour[(iCurrentDirectional * 4) + 2] = a_Colour.z;
	fpDirectionalColour[(iCurrentDirectional * 4) + 3] = a_Colour.w;

	iCurrentDirectional++;
	return (iCurrentDirectional - 1);
}
int Lights::AddPoint(glm::vec3 a_Position,float a_fDistance,glm::vec4 a_Colour ){
	fpPointPosition[(iCurrentPoint * 3) + 0] = a_Position.x;
	fpPointPosition[(iCurrentPoint * 3) + 1] = a_Position.y;
	fpPointPosition[(iCurrentPoint * 3) + 2] = a_Position.z;

	fPointDistance[iCurrentPoint] = a_fDistance;

	fpPointColour[(iCurrentPoint * 4) + 0] = a_Colour.x;
	fpPointColour[(iCurrentPoint * 4) + 1] = a_Colour.y;
	fpPointColour[(iCurrentPoint * 4) + 2] = a_Colour.z;
	fpPointColour[(iCurrentPoint * 4) + 3] = a_Colour.w;

	iCurrentPoint++;
	return (iCurrentPoint - 1);
}
int Lights::AddSpot(glm::vec3 a_Position,glm::vec3 a_Direction,float a_fDistance,float f_Angle,glm::vec4 a_Colour){
	fpSpotPosition[(iCurrentSpot * 3) + 0] = a_Position.x;
	fpSpotPosition[(iCurrentSpot * 3) + 1] = a_Position.y;
	fpSpotPosition[(iCurrentSpot * 3) + 2] = a_Position.z;
	
	fSpotDirection[(iCurrentSpot * 3) + 0] = a_Direction.x;
	fSpotDirection[(iCurrentSpot * 3) + 1] = a_Direction.y;
	fSpotDirection[(iCurrentSpot * 3) + 2] = a_Direction.z;

	fSpotDistance[iCurrentSpot] = a_fDistance;

	fSpotAngle[iCurrentSpot] = f_Angle;

	fpSpotColour[(iCurrentSpot * 4) + 0] = a_Colour.x;
	fpSpotColour[(iCurrentSpot * 4) + 1] = a_Colour.y;
	fpSpotColour[(iCurrentSpot * 4) + 2] = a_Colour.z;
	fpSpotColour[(iCurrentSpot * 4) + 3] = a_Colour.w;

	iCurrentSpot++;
	return (iCurrentSpot - 1);
}

void Lights::DirectionalMove(int ID,glm::vec3 v3_Pos){
	fpDirectionalPosition[(ID * 3) + 0] = v3_Pos.x;
	fpDirectionalPosition[(ID * 3) + 1] = v3_Pos.y;
	fpDirectionalPosition[(ID * 3) + 2] = v3_Pos.z;
}
void Lights::DirectionalColour(int ID,glm::vec4 v4_Colour){
	fpDirectionalColour[(ID * 4) + 0] = v4_Colour.x;
	fpDirectionalColour[(ID * 4) + 1] = v4_Colour.y;
	fpDirectionalColour[(ID * 4) + 2] = v4_Colour.z;
	fpDirectionalColour[(ID * 4) + 3] = v4_Colour.w;
}

void Lights::PointMove(int ID,glm::vec3 v3_Pos){
	fpPointPosition[(ID * 3) + 0] = v3_Pos.x;
	fpPointPosition[(ID * 3) + 1] = v3_Pos.y;
	fpPointPosition[(ID * 3) + 2] = v3_Pos.z;
}
void Lights::PointColour(int ID,glm::vec4 v4_Colour){
	fpPointColour[(ID * 4) + 0] = v4_Colour.x;
	fpPointColour[(ID * 4) + 1] = v4_Colour.y;
	fpPointColour[(ID * 4) + 2] = v4_Colour.z;
	fpPointColour[(ID * 4) + 3] = v4_Colour.w;
}
void Lights::PointDistance(int ID,float f_Distance){
	fPointDistance[ID] = f_Distance;
}

void Lights::SpotMove(int ID,glm::vec3 v3_Pos){
	fpSpotPosition[(ID * 3) + 0] = v3_Pos.x;
	fpSpotPosition[(ID * 3) + 1] = v3_Pos.y;
	fpSpotPosition[(ID * 3) + 2] = v3_Pos.z;
}
void Lights::SpotColour(int ID,glm::vec4 v4_Colour){
	fpSpotColour[(ID * 4) + 0] = v4_Colour.x;
	fpSpotColour[(ID * 4) + 1] = v4_Colour.y;
	fpSpotColour[(ID * 4) + 2] = v4_Colour.z;
	fpSpotColour[(ID * 4) + 3] = v4_Colour.w;
}
void Lights::SpotDistance(int ID,float f_Distance){
	fSpotDistance[ID] = f_Distance;
}
void Lights::SpotAngle(int ID,float f_Angle){
	fSpotAngle[ID] = f_Angle;
}
void Lights::SpotDirection(int ID,glm::vec3 v3_Direction){
	fSpotDirection[(ID * 3) + 0] = v3_Direction.x;
	fSpotDirection[(ID * 3) + 1] = v3_Direction.y;
	fSpotDirection[(ID * 3) + 2] = v3_Direction.z;
}

void Lights::Clear(){
	delete fpDirectionalPosition;
	fpDirectionalPosition = new float[imaxLights * 3];
	delete fpDirectionalColour;
	fpDirectionalColour = new float[imaxLights * 4];

	delete fpPointPosition;
	fpPointPosition = new float[imaxLights * 3];
	delete fpPointColour;
	fpPointColour = new float[imaxLights * 4];
	delete fPointDistance;
	fPointDistance = new float[imaxLights];

	delete fpSpotPosition;
	fpSpotPosition = new float[imaxLights * 3];
	delete fpSpotColour;
	fpSpotColour = new float[imaxLights * 4];
	delete fSpotDistance;
	fSpotDistance = new float[imaxLights];
	delete fSpotAngle;
	fSpotAngle = new float[imaxLights];
	delete fSpotDirection;
	fSpotDirection = new float[imaxLights * 3];
}
void Lights::SetAmbient(glm::vec3 v3_Ambient){
	fAmbient[0] = v3_Ambient.x;
	fAmbient[1] = v3_Ambient.y;
	fAmbient[2] = v3_Ambient.z;
}
void Lights::SetSpecular(glm::vec3 v3_Specular){
	fSpecular[0] = v3_Specular.x;
	fSpecular[1] = v3_Specular.y;
	fSpecular[2] = v3_Specular.z;

}
void Lights::UpdateUniforms(Shader* s_LightShader){

	s_LightShader->SetUniform("LightAmbient","3fv",1,&glm::vec3(fAmbient[0],fAmbient[1],fAmbient[2]));
	s_LightShader->SetUniform("LightSpecular","3fv",1,&glm::vec3(fSpecular[0],fSpecular[1],fSpecular[2]));

	//push values to uniforms, some of this is done with straight OGL, needs to be wrapped properly

	if (iCurrentDirectional > 0){
		//std::cout << "Pushing Directional Lights with count of " << iCurrentDirectional << '\n';
		unsigned int iLoc = glGetUniformLocation(s_LightShader->id,"DirectionalLightDirection");
		glUniform3fv(iLoc,iCurrentDirectional,&fpDirectionalPosition[0]);
		iLoc = glGetUniformLocation(s_LightShader->id,"DirectionalLightColour");
		glUniform4fv(iLoc,iCurrentDirectional,&fpDirectionalColour[0]);

		s_LightShader->SetUniform("DirectionalLightCount","1i",1,iCurrentDirectional);
	}

	if (iCurrentPoint > 0){
		//std::cout << "Pushing Point Lights with count of " << iCurrentPoint << '\n';
		unsigned int iLoc = glGetUniformLocation(s_LightShader->id,"PointLightDistance");
		glUniform1fv(iLoc,iCurrentPoint,&fPointDistance[0]);
		iLoc = glGetUniformLocation(s_LightShader->id,"PointLightColour");
		glUniform4fv(iLoc,iCurrentPoint,&fpPointColour[0]);
		iLoc = glGetUniformLocation(s_LightShader->id,"PointLightPosition");
		glUniform3fv(iLoc,iCurrentPoint,&fpPointPosition[0]);

		s_LightShader->SetUniform("PointLightCount","1i",1,iCurrentPoint);
	}

	if (iCurrentSpot > 0){
		//std::cout << "Pushing Spot Lights with count of " << iCurrentSpot << '\n';
		unsigned int iLoc = glGetUniformLocation(s_LightShader->id,"SpotLightPosition");
		glUniform3fv(iLoc,iCurrentSpot,&fpSpotPosition[0]);
		iLoc = glGetUniformLocation(s_LightShader->id,"SpotLightDirection");
		glUniform3fv(iLoc,iCurrentSpot,&fSpotDirection[0]);
		iLoc = glGetUniformLocation(s_LightShader->id,"SpotLightAngle");
		glUniform1fv(iLoc,iCurrentSpot,&fSpotAngle[0]);
		iLoc = glGetUniformLocation(s_LightShader->id,"SpotLightDistance");
		glUniform1fv(iLoc,iCurrentSpot,&fSpotDistance[0]);
		iLoc = glGetUniformLocation(s_LightShader->id,"SpotLightColour");
		glUniform4fv(iLoc,iCurrentSpot,&fpSpotColour[0]);

		s_LightShader->SetUniform("SpotLightCount","1i",1,iCurrentSpot);
	}
}
void Lights::Draw(){
	if (iCurrentDirectional > 0){
		for(int i = 0;i < iCurrentDirectional; i++){

			glm::vec3 Direction(fpDirectionalPosition[(i*3)+0],fpDirectionalPosition[(i*3)+1],fpDirectionalPosition[(i*3)+2]);
			glm::vec4 Colour(fpDirectionalColour[(i*4)+0],fpDirectionalColour[(i*4)+1],fpDirectionalColour[(i*4)+2],fpDirectionalColour[(i*4)+3]);

			Gizmos::addSphere(glm::vec3(0,100,0),12,12,0.1f,Colour);
			Gizmos::addLine( glm::vec3(0,100,0), glm::vec3(0,100,0) + (Direction * -8.0f), Colour,glm::vec4(0,0,0,1) );
		}
	}
	if (iCurrentPoint > 0){
		for(int i = 0;i < iCurrentPoint; i++){

			glm::vec3 Position(fpPointPosition[(i*3)+0],fpPointPosition[(i*3)+1],fpPointPosition[(i*3)+2]);
			glm::vec4 Colour(fpPointColour[(i*4)+0],fpPointColour[(i*4)+1],fpPointColour[(i*4)+2],fpPointColour[(i*4)+3]);

			Gizmos::addSphere(Position,12,12,0.1f,Colour);
		}
	}
	if (iCurrentSpot > 0){
		for(int i = 0;i < iCurrentSpot; i++){

			glm::vec3 Position(fpSpotPosition[(i*3)+0],fpSpotPosition[(i*3)+1],fpSpotPosition[(i*3)+2]);
			glm::vec4 Colour(fpSpotColour[(i*4)+0],fpSpotColour[(i*4)+1],fpSpotColour[(i*4)+2],fpSpotColour[(i*4)+3]);
			glm::vec3 Direction(fSpotDirection[(i*4)+0],fSpotDirection[(i*4)+1],fSpotDirection[(i*4)+2]);

			Gizmos::addSphere(Position,12,12,0.1f,Colour);
			Gizmos::addLine( Position, Position + (Direction), Colour,glm::vec4(0,0,0,0) );
		}
	}
}

// Static Lights

void PointLight::Update(GLFWwindow* a_window, float a_deltaTime){
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
void PointLight::Draw(){
	Gizmos::addSphere(Position,12,12,0.1f,Colour);
}

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
	Gizmos::addLine( Position, Position + (Direction), Colour,glm::vec4(0,0,0,0) );
}

void DirectionalLight::Update(GLFWwindow* a_window, float a_deltaTime){
	if (glfwGetKey(a_window,GLFW_KEY_LEFT) == GLFW_PRESS){ 
		Direction = glm::rotate(Direction,-1.0f* a_deltaTime,glm::vec3(1,0,0));
	}if (glfwGetKey(a_window,GLFW_KEY_RIGHT) == GLFW_PRESS){ 
		Direction = glm::rotate(Direction,1.0f* a_deltaTime,glm::vec3(1,0,0));
	}
	if (glfwGetKey(a_window,GLFW_KEY_UP) == GLFW_PRESS){ 
		if (glfwGetKey(a_window,GLFW_KEY_RIGHT_SHIFT)){
		Direction = glm::rotate(Direction,1.0f* a_deltaTime,glm::vec3(0,1,0));
		}else{
		Direction = glm::rotate(Direction,-1.0f* a_deltaTime,glm::vec3(0,0,1));
		}
	}else if (glfwGetKey(a_window,GLFW_KEY_DOWN) == GLFW_PRESS){ 
		if (glfwGetKey(a_window,GLFW_KEY_RIGHT_SHIFT)){
		Direction = glm::rotate(Direction,-1.0f* a_deltaTime,glm::vec3(0,1,0));
		}else{
		Direction = glm::rotate(Direction,1.0f* a_deltaTime,glm::vec3(0,0,1));
		}
	}
}
void DirectionalLight::Draw(){
	Gizmos::addSphere(glm::vec3(0,5,0),12,12,0.1f,Colour);
	Gizmos::addLine( glm::vec3(0,5,0), glm::vec3(0,5,0) + (Direction), Colour,glm::vec4(0,0,0,0) );
	Gizmos::addSphere(glm::vec3(0,5,0) + (Direction),12,12,0.1f,glm::vec4(0,0,0,0));
}