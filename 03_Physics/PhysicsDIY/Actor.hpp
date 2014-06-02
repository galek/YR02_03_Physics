#pragma once

#include <unordered_map>
#include <glm\glm.hpp>
#include <Gizmos.h>
#include <vector>
#include "Base.hpp"

namespace Osiris{

	class Shape;
	class Scene;

	enum ActorType { STATIC, DYNAMIC };
	#define ACTORTYPE(type) ((type == ActorType::STATIC) ? "STATIC" : "DYNAMIC")
	typedef std::vector<Shape*> ShapeVector;

	class Actor : public _Base {
	public:
		Actor();
		virtual ~Actor();

		const bool&			getActive();
		void				setActive(bool bActive);

		const bool&			toRemove();

		Scene*				getScene();
		void				setScene(Scene* s_Scene);

		ShapeVector&		getShapes();

		const ActorType		getType();

		void addShape(Shape* a_Shape);

		virtual void update(float f_timeStepInterval);
		virtual void spatial() = 0;
		virtual void draw();


	protected:
		void calculateAABB();
		ActorType aType;
		ShapeVector vShapes;
		Scene *parentScene;
		bool bActive;
		bool bRemoveMe;
	};

	class ActorStatic : public Actor {
	public:
		ActorStatic();
		virtual ~ActorStatic();
		virtual void spatial();
		virtual void update(float f_timeStepInterval);

	};

	class ActorDynamic : public Actor {
	public:
		ActorDynamic();
		virtual ~ActorDynamic();

		const glm::vec3&	getDamping	();
		void				setDamping	(const glm::vec3& v3_Damping);

		const glm::vec3&	getVelocity	();
		void				setVelocity	(const glm::vec3& v3_Velocity);
		void				addForce	(const glm::vec3& v3_Velocity);

		virtual void spatial();
		virtual void update(float f_timeStepInterval);
	protected:
		glm::vec3 v3Velocity;
		glm::vec3 v3Damping;
	};
}

