#pragma once

#include <vector>

#include <glm\ext.hpp>
#include "Shape.hpp"

typedef std::vector<Shape*> ShapesVector;

enum ActorType { STATIC, DYNAMIC };

//======================================================================================

class Actor {
public:
	float _Mass;
	float _InvMass;
	glm::vec3 _Velocity;

	glm::mat4 _Transform;
	ShapesVector _Shapes;
	ActorType _Type;

public:
	Actor(glm::vec3 Position);
	virtual ~Actor() { }

	virtual void Update();
	virtual void Draw();

	virtual void ApplyForce(glm::vec3 Force) { }
	void AddShape(Shape *shape);
};

//======================================================================================

class ActorStatic : public Actor {
public:
	ActorStatic(glm::vec3 Position);
	virtual ~ActorStatic() { }

	virtual void Update() { }
};

//======================================================================================

class ActorDynamic : public Actor {
	bool _Gravity;

public:
	ActorDynamic(glm::vec3 Position, float Mass, glm::vec3 Velocity);
	virtual ~ActorDynamic() { }

	virtual void Update();

	virtual void ApplyForce(glm::vec3 Force);
};

