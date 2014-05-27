#pragma once

#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>
#include <unordered_map>

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

class PhysXScene {
public:
	PhysXScene(float f_TicksPerSecond);
	~PhysXScene();	
	
	void update();
	void draw();

	void AddPlane	(char* name, physx::PxActorType::Enum aType, const float& f_density														, const glm::vec3& v3_Transform = glm::vec3(0), const physx::PxQuat& px_Quaternion = physx::PxQuat(0,0,0,0), const glm::vec3& v3_Direction = glm::vec3(0), const float& f_Power = 0.0f);
	void AddBox		(char* name, physx::PxActorType::Enum aType, const float& f_density, const glm::vec3& v3_Dimentions						, const glm::vec3& v3_Transform = glm::vec3(0), const physx::PxQuat& px_Quaternion = physx::PxQuat(0,0,0,0), const glm::vec3& v3_Direction = glm::vec3(0), const float& f_Power = 0.0f);
	void AddSphere	(char* name, physx::PxActorType::Enum aType, const float& f_density, const float& f_Radius								, const glm::vec3& v3_Transform = glm::vec3(0), const physx::PxQuat& px_Quaternion = physx::PxQuat(0,0,0,0), const glm::vec3& v3_Direction = glm::vec3(0), const float& f_Power = 0.0f);
	void AddCapsule (char* name, physx::PxActorType::Enum aType, const float& f_density, const float& f_Radius, const float& f_HalfHeight	, const glm::vec3& v3_Transform = glm::vec3(0), const physx::PxQuat& px_Quaternion = physx::PxQuat(0,0,0,0), const glm::vec3& v3_Direction = glm::vec3(0), const float& f_Power = 0.0f);
	
	void linkFixed		(physx::PxRigidActor* px_Actor1, physx::PxTransform pxt_Transform1, physx::PxRigidActor* px_Actor2, physx::PxTransform pxt_Transform2);
	void linkDistance	(physx::PxRigidActor* px_Actor1, physx::PxTransform pxt_Transform1, physx::PxRigidActor* px_Actor2, physx::PxTransform pxt_Transform2, float f_Lower, float f_Upper, float f_Spring = 0.0f, float f_Damping = 0.0f);
	void linkSherical	(physx::PxRigidActor* px_Actor1, physx::PxTransform pxt_Transform1, physx::PxRigidActor* px_Actor2, physx::PxTransform pxt_Transform2, float f_YLimit, float f_Zlimit, float f_ContactDistance);
	void linkRevolute	(physx::PxRigidActor* px_Actor1, physx::PxTransform pxt_Transform1, physx::PxRigidActor* px_Actor2, physx::PxTransform pxt_Transform2, float f_Lower, float f_Upper, float f_ContactDistance);
	void linkPrismatic	(physx::PxRigidActor* px_Actor1, physx::PxTransform pxt_Transform1, physx::PxRigidActor* px_Actor2, physx::PxTransform pxt_Transform2, float f_Lower, float f_Upper, float f_ContactDistance);

	// Get an actor from its Name
	physx::PxRigidActor* getActor(char* name){
		unsigned long long l = getHash(std::string(name));
		if (g_PhysXActors.find(l) != g_PhysXActors.end()){
			return g_PhysXActors[l];
		}
		return nullptr;
	}
	// Get an actor from its Hash
	physx::PxRigidActor* getActor(long long ll_Hash){
		if (g_PhysXActors.find(ll_Hash) != g_PhysXActors.end()){
			return g_PhysXActors[ll_Hash];
		}
		return nullptr;
	}

	// Get an Joint from its actor pair
	physx::PxJoint* getJoint(physx::PxRigidActor* px_Actor1,physx::PxRigidActor* px_Actor2){
		std::string name = px_Actor1->getName();
		name.append(px_Actor2->getName());
		unsigned long long l = getHash(name);
		if (g_PhysXJoints.find(l) != g_PhysXJoints.end()){
			return g_PhysXJoints[l];
		}
		return nullptr;
	}

	void controlActor(float a_deltaTime, const glm::mat4& m_camera, physx::PxRigidActor* actor,float f_Force);
	void snapActor(physx::PxRigidActor* actor,glm::vec3 v3_Position);

	// Get a long long key from a String key
	unsigned long long getHash(std::string key){
		unsigned long long hash = 0;
		for (unsigned int i = 0; i < key.length(); i++){
			hash = 31*hash + (key[i] + 127);
		}
		return hash;
	}

private:
	float fTicksPerSecond;
	physx::PxFoundation* g_PhysicsFoundation;
	physx::PxPhysics* g_Physics;
	physx::PxScene* g_PhysicsScene;
	physx::PxDefaultErrorCallback gDefaultErrorCallback;
	physx::PxDefaultAllocator gDefaultAllocatorCallback;
	physx::PxSimulationFilterShader gDefaultFilterShader;
	physx::PxMaterial* g_PhysicsMaterial;
	physx::PxCooking* g_PhysicsCooker;
	std::unordered_map<unsigned long long,physx::PxRigidActor*> g_PhysXActors;
	std::unordered_map<unsigned long long,physx::PxJoint*> g_PhysXJoints;
};
