#pragma once

#include <unordered_map>
#include <glm\glm.hpp>
#include <Gizmos.h>
#include <vector>
#include "Spatial.hpp"

namespace Osiris {
	class Scene {
	public:
		// Scene Constructor
		// f_TimeStep		= How many times to update per second.
		// iv3_GridSize		= Size of each Bucket in the Spatial Grid.		
		// v3_ClipBounds	= How far away from 0,0,0 the outer bounds are. Anything outside these bounds will be destroyed.
		Scene(float f_TimeStep, glm::ivec3 iv3_GridSize, glm::vec3 v3_ClipBounds);
		~Scene();

		Spatial*			getSpatial		();
		const glm::vec3&	getClipBounds	();

		const float			getGravity		();
		void				setGravity		(const float& f_Gravity);

		void				clearActors		();
		void				clearGrid		();

		int					addActor(Actor* a_Actor);
		void				removeActor(int i_ID);
		void				update(float f_deltaTime);
		void				draw();

	private:
		Spatial *sGrid;
		glm::vec3 v3ClipBounds;
		std::unordered_map<int,Actor*> umiaActors;
		bool	bNext;
		float	fGravity;
		float	fTime;
		float	fTimeStep;
		int		iCurrent;
	};
}

