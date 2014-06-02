#pragma once

#include <unordered_map>
#include <glm\glm.hpp>
#include <Gizmos.h>
#include <vector>
#include "Scene.hpp"
#include "Actor.hpp"

namespace Osiris {
	//f_TimeStep = How many times to update per second
	Scene::Scene(float f_TimeStep, glm::ivec3 iv3_GridSize, glm::vec3 v3_ClipBounds){
		fGravity = -9.8f;
		iCurrent = 0;
		fTimeStep = 1/f_TimeStep;
		fTime = 0.0f;
		bNext = false;
		sGrid = new Spatial(iv3_GridSize);
		v3ClipBounds = v3_ClipBounds;
	}
	Scene::~Scene(){
		delete sGrid;
		for (auto actor : umiaActors){
			delete actor.second;
		}
		umiaActors.clear();
	}

	Spatial*			Scene::getSpatial		()							{return sGrid;}
	const glm::vec3&	Scene::getClipBounds	()							{return v3ClipBounds;}

	const float			Scene::getGravity		()							{return fGravity;}
	void				Scene::setGravity		(const float& f_Gravity)	{fGravity = f_Gravity;}

	int Scene::addActor(Actor* a_Actor){
		umiaActors[iCurrent] = a_Actor;
		a_Actor->setScene(this);
		a_Actor->update(fTimeStep);
		iCurrent++;
		return iCurrent - 1;
	}

	void Scene::removeActor(int i_ID){
		if (umiaActors.find(iCurrent) != umiaActors.end()){
			umiaActors.erase(iCurrent);
			if (umiaActors.size() == 0){
				iCurrent = 0;
			}
		}
	}

	void Scene::update(float f_deltaTime){
		fTime -= f_deltaTime;
		if (bNext){
			bNext = false;
			GridList grid = sGrid->getBuckets();
			for (auto bucket : grid){
				bucket->update(fTimeStep);
			}
		}
		if (fTime <= 0.0f){
			fTime = fTimeStep;
			bNext = true;
			sGrid->clearEmpty();
			sGrid->clearElements();
			for (auto actor : umiaActors){
				actor.second->spatial();
			}
			for (std::unordered_map<int,Actor*>::iterator child = umiaActors.begin(); child != umiaActors.end();){
				if (child->second->toRemove()){
					delete child->second;
					child = umiaActors.erase(child);
					if (umiaActors.size() == 0){
						iCurrent = 0;
					}
				}else{
					child++;
				}
			}
		}
	}

	void Scene::clearActors(){
		for (auto actor : umiaActors){
			delete actor.second;
		}
		umiaActors.clear();
		sGrid->clear();
		iCurrent = 0;
	}

	void Scene::clearGrid(){
		sGrid->clear();
	}

	void Scene::draw(){
		for (auto actor : umiaActors){
			actor.second->draw();
		}
	}
}

