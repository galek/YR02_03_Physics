#include "Actor.hpp"
#include <limits>

#include "Constants.hpp"
#include "Shapes.hpp"

#include <glm\glm.hpp>

//!-- Actor

Actor::Actor(glm::vec3 Position) {
	_Mass = 1.0f;
	_InvMass = 1.0f / _Mass;
	_Velocity = glm::vec3(0);
	_Transform[3].xyz = Position; 
}

void Actor::Update() {

}

void Actor::Draw() {
	for (auto shape : _Shapes) {
		shape->Draw(_Transform[3].xyz);
	}
}

void Actor::AddShape(Shape *shape) {
	shape->_Parent = this;
	_Shapes.push_back(shape);
}

//!-- Actor Static

ActorStatic::ActorStatic(glm::vec3 Position) : Actor(Position) {
	_Mass = FLT_MAX;
	_Mass = 0.0f;
	_Velocity = glm::vec3(0);
	 _Type = STATIC; 
}

ActorDynamic::ActorDynamic(glm::vec3 Position, float Mass, glm::vec3 Velocity) : Actor(Position) {
	_Gravity = true;
	_Mass = Mass;
	_Velocity = Velocity;
	_Type = DYNAMIC;
}

//!-- Actor Dynamic

void ActorDynamic::Update() {

	//	Apply Gravity
	if (_Gravity) ApplyForce(glm::vec3(0, DIYPHYSICS::GRAVITY, 0) * _Mass * DIYPHYSICS::TIMESTEP);

	//	Update Position
	_Transform[3].xyz += _Velocity * DIYPHYSICS::TIMESTEP;
}

void ActorDynamic::ApplyForce(glm::vec3 Force) {
	_Velocity += Force/_Mass;
}
