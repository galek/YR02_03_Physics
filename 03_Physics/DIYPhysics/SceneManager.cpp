#include "SceneManager.hpp"
#include "Constants.hpp"
#include "Gizmos.h"
#include "Utilities.h"

#include <glm/ext.hpp>
#include <GLFW/glfw3.h>

bool update = true;

SceneManager::SceneManager() {
	_TimeStepLast = 0.0f;
}

void SceneManager::Update() { 
	if (Utility::getTotalTime() < _TimeStepLast + DIYPHYSICS::TIMESTEP){
		return;
	}
	_TimeStepLast = Utility::getTotalTime();

	if (glfwGetKey(glfwGetCurrentContext(),GLFW_KEY_F1) == GLFW_PRESS){
		update = !update;
	}

	if (!update){
		return;
	}

	//	Update all Actors
	_Bucket.clear();
	for (auto actor : _Actors) {
		actor.second->Update();
		_Bucket.push_back(actor.second);
	}

	//	Check Collisions on All Actors
	for (ActorList::iterator A = _Bucket.begin(); A != _Bucket.end(); A++) {
		for (ActorList::iterator B = A + 1; B != _Bucket.end(); B++) {
			for (auto AA : (*A)->_Shapes) {
				for (auto BB : (*B)->_Shapes) {
					Data data;
					if (Collision::Shape_Shape(AA, BB, data)) {
						Collide::Shape_Shape(AA, BB, data);
						//update = false;
					}
				}
			}
		}
	}
}

void SceneManager::Draw() { 
	for (auto actor : _Actors) {
		actor.second->Draw();
	}
}

int SceneManager::AddActor(Actor *actor) { 
	int ID = _CurrentID;
	_Actors[ID] = actor;
	_CurrentID++;
	return ID;
}
void SceneManager::RemoveActor(int ID) {
	if (_Actors.find(ID) == _Actors.end()) { return; }

	_Actors.erase(ID);
	if (_Actors.size() == 0){ _CurrentID = 0; }
}