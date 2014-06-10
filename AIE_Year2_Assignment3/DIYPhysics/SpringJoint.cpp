#include "SpringJoint.hpp"
#include "Gizmos.h"

SpringJoint::SpringJoint(Actor *Start, Actor *End, float Coefficient, float Damping, float Length) {
	_Actors[0] = Start;
	_Actors[1] = End;
	_Coefficient = Coefficient;
	_Damping = Damping;
	_Length = Length;
}

void SpringJoint::Update() {

	if (_Actors[0] == nullptr || _Actors[1] == nullptr) { return; }

	glm::vec3 delta = _Actors[0]->_Transform[3].xyz() - _Actors[1]->_Transform[3].xyz();
	glm::vec3 normal = glm::normalize(delta);
	float length = glm::length(delta);

	glm::vec3 reletaveVelocity = _Actors[1]->_Velocity - _Actors[0]->_Velocity;
	glm::vec3 forceVector = normal * (length - _Length) * _Coefficient;
	forceVector -= _Damping * reletaveVelocity;

	//	Static Checks
	if (_Actors[0]->_Type ==  ActorType::DYNAMIC && _Actors[1]->_Type ==  ActorType::STATIC) {
		_Actors[0]->ApplyForce(-forceVector * 2.0f);
	} else if (_Actors[0]->_Type ==  ActorType::STATIC && _Actors[1]->_Type ==  ActorType::DYNAMIC) {
		_Actors[1]->ApplyForce( forceVector * 2.0f);
	} else {
		_Actors[0]->ApplyForce(-forceVector);
		_Actors[1]->ApplyForce( forceVector);
	}
}

void SpringJoint::Draw() {
	if (_Actors[0] == nullptr || _Actors[1] == nullptr)
		return;
	Gizmos::addLine(_Actors[0]->_Transform[3].xyz(), _Actors[1]->_Transform[3].xyz(), glm::vec4(1));
}
