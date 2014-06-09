#pragma once 

#include <map>
#include "Collision.hpp"
#include "Actor.hpp"

typedef std::map<int, Actor*> ActorMap;
typedef std::vector<Actor*> ActorList;

class SceneManager {
public:
	int _CurrentID;

	float _TimeStepLast;

	ActorMap _Actors;
	ActorList _Bucket;

	SceneManager();
	virtual ~SceneManager() { }

	void Update();
	void Draw();

	void Collide(Actor *A, Actor *B, Data data);

	int AddActor(Actor *actor);
	void RemoveActor(int ID); 
};