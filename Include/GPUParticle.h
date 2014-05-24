#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

struct GPUParticleS {
	GPUParticleS() : v3Position(1), v3Velocity(1), fLifeTime(1), fLifeSpan(0),fRotation(1) {}

    glm::vec3	v3Position;
    glm::vec3	v3Velocity;
    float		fLifeTime;
    float		fLifeSpan;
    float		fRotation;
};

class GPUParticle {
//-----------------------------
public:
//CONSTRUCTORS
	GPUParticle(unsigned int MAXPARTICLES);
	~GPUParticle();
//METHODS
	void	setPosition			(const glm::vec3 a_Vec);
	void	setRange			(const glm::vec3 a_Vec);
	void	setVelocity			(const glm::vec3 a_Vec);
	void	setVelocityJitterMax(const glm::vec3 a_Vec);
	void	setAcceleration		(const glm::vec3 a_Vec);
	void	setSizeStart		(const glm::vec2 a_Vec);
	void	setSizeEnd			(const glm::vec2 a_Vec);
	void	setColourStart		(const glm::vec4 a_Vec);
	void	setColourEnd		(const glm::vec4 a_Vec);
	void	setLifeSpanStart	(const float a_Float);
	void	setLifeSpanEnd		(const float a_Float);
	void	setRotationStepMax	(const float a_Degree);
	void	setTextureID		(const unsigned int a_TextureID);

	//right now this just does a draw call, It will run the shaderUpdate function for given time
	void	PreWarm(float f_Time);
	//run the particle system
	void	draw(const glm::mat4& a_cameraTransform, const glm::mat4& a_projection);
//-----------------------------
protected:
//FUNCTIONS
	void	createBuffers();
	void	createUpdateShader();
	void	createDrawShader();
	void	shaderUpdate(float time = 0.0f, float delta = 0.0f);
	void	shaderDraw(const glm::mat4& a_cameraTransform, const glm::mat4& a_projection);
//DEFAULT PARTICLE VALS
    glm::vec3	v3Position;
	glm::vec3	v3Range;
    glm::vec3	v3Velocity,v3VelocityJitter;
    glm::vec3	v3Acceleration;

    glm::vec2	v2SizeStart			,v2SizeEnd;
    glm::vec4	v4ColourStart		,v4ColourEnd;

    float		fLifeSpanMin		,fLifeSpanMax;
    float		fRotationStepMax;
//-----------------------------
private:
	//EMITTER VARS
	unsigned int	uiMaxParticles;
	GPUParticleS*	gpParticles;
	unsigned int	uiTexID;

	unsigned int	uivao[2]		,uivbo[2];
	unsigned int	uiDrawShader	,uiUpdateShader;
	unsigned int	uiActiveBuffer	,uiOtherBuffer;
};

