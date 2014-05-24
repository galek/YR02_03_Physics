#pragma once

#include <glm/glm.hpp>

struct Particle {
	glm::vec3	position;
	glm::vec3	velocity;
	glm::vec4	colour;
	float		size;
	float		lifetime;
	float		lifespan;
	float		rotation;
};

#include <list>
#include "BasicVertex.h"

class ParticleEmitter {
public:
	ParticleEmitter(void);
	~ParticleEmitter(void);
	void initalise(	unsigned int a_maxParticles, unsigned int a_emitRate, 
					float a_lifetimeMin, float a_lifetimeMax, 
					float a_velocityMin, float a_velocityMax, 
					float a_startSize, float a_endSize, 
					const glm::vec4& a_startColour, const glm::vec4& a_endColour);

	void update(float a_deltaTime, const glm::mat4& a_cameraTransform);
	void draw();
protected:
	//Particles
	std::list<Particle*> m_aliveParticles;
	std::list<Particle*> m_deadParticles;
	Particle* m_particles;
	//OpenGL
	unsigned int m_vao,m_vbo,m_ibo;
	BasicVertex* m_vertexData;
	unsigned int* m_indexData;
	//Emitter Values
	glm::vec3	m_position;
	glm::vec4	m_startColour,m_endColour;

	float		m_emitTimer,m_emitRate;
	float		m_lifespanMin,m_lifespanMax;
	float		m_velocityMin,m_velocityMax;
	float		m_startSize,m_endSize;

	float		m_rotatestep;
private:
	void emit();
};

