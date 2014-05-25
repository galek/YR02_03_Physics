#pragma once

#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>
#include <vector>

class myAllocator: public physx::PxAllocatorCallback {
public:
	virtual ~myAllocator() {}
	virtual void* allocate(size_t size, const char* typeName, const char* filename, int line){
		void* pointer = _aligned_malloc(size,16);
		return pointer;
	}
	virtual void deallocate(void* ptr){
		_aligned_free(ptr);
	}
};

class PhysXScene
{
public:
	PhysXScene(void);
	~PhysXScene(void);	
	
	void update();
	void draw();

	void AddPlane	(char* name, physx::PxActorType::Enum aType, const float& f_density														, const glm::vec3& v3_Transform = glm::vec3(0), const physx::PxQuat& px_Quaternion = physx::PxQuat(0,0,0,0), const glm::vec3& v3_Direction = glm::vec3(0), const float& f_Power = 0.0f);
	void AddBox		(char* name, physx::PxActorType::Enum aType, const float& f_density, const glm::vec3& v3_Dimentions						, const glm::vec3& v3_Transform = glm::vec3(0), const physx::PxQuat& px_Quaternion = physx::PxQuat(0,0,0,0), const glm::vec3& v3_Direction = glm::vec3(0), const float& f_Power = 0.0f);
	void AddSphere	(char* name, physx::PxActorType::Enum aType, const float& f_density, const float& f_Radius								, const glm::vec3& v3_Transform = glm::vec3(0), const physx::PxQuat& px_Quaternion = physx::PxQuat(0,0,0,0), const glm::vec3& v3_Direction = glm::vec3(0), const float& f_Power = 0.0f);
	void AddCapsule (char* name, physx::PxActorType::Enum aType, const float& f_density, const float& f_Radius, const float& f_HalfHeight	, const glm::vec3& v3_Transform = glm::vec3(0), const physx::PxQuat& px_Quaternion = physx::PxQuat(0,0,0,0), const glm::vec3& v3_Direction = glm::vec3(0), const float& f_Power = 0.0f);
								 
	physx::PxRigidActor* getActor(char* name){
		for (auto actor : g_PhysXActors) {
			if (actor->getName() == name){
				return actor;
			}
		}
	}

	void controlActor(float a_deltaTime, physx::PxRigidActor* actor,float f_Force);
	void snapActor(physx::PxRigidActor* actor,glm::vec3 v3_Position);

private:
	physx::PxFoundation* g_PhysicsFoundation;
	physx::PxPhysics* g_Physics;
	physx::PxScene* g_PhysicsScene;
	physx::PxDefaultErrorCallback gDefaultErrorCallback;
	physx::PxDefaultAllocator gDefaultAllocatorCallback;
	physx::PxSimulationFilterShader gDefaultFilterShader;
	physx::PxMaterial* g_PhysicsMaterial;
	physx::PxCooking* g_PhysicsCooker;
	std::vector<physx::PxRigidActor*> g_PhysXActors;
};

