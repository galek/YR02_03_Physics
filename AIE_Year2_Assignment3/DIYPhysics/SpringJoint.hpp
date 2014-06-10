#pragma once

#include "Actor.hpp"

class SpringJoint {
protected:
	Actor *_Actors[2];
	float _Coefficient;
	float _Damping;
	float _Length;

public:
	SpringJoint(Actor *Start, Actor *End, float Coefficient, float Damping, float Length);
	virtual ~SpringJoint() { }

	void Update();
	void Draw();
};

