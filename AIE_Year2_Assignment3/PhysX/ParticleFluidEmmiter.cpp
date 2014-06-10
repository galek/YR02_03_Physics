#include "ParticleFluidEmmiter.h"
#include <iostream>
#include<vector>
#include "Gizmos.h"
#include <GL/glew.h>

//constructor
ParticleFluidEmitter::ParticleFluidEmitter(int _maxParticles, PxVec3 _position,PxParticleFluid* _pf,float _releaseDelay){
	releaseDelay = _releaseDelay;
	maxParticles = _maxParticles;  //maximum number of particles our emitter can handle
	//allocate an array
	activeParticles = new FluidParticle[maxParticles]; //array of particle structs
	time = 0; //time system has been running
	respawnTime = 0; //time for next respawn
	position = _position;
	pf =_pf; //pointer to the physX particle system
	particleMaxAge = 8; //maximum time in seconds that a particle can live for
	//initialize the buffer
	for(int index=0;index<maxParticles;index++)
	{
		activeParticles[index].active = false;
	}
}

//destructure
ParticleFluidEmitter::~ParticleFluidEmitter()
{
	//remove all the active particles
	delete activeParticles;
}

//find the next free particle, mark it as used and return it's index.  If it can't allocate a particle: returns minus one
int ParticleFluidEmitter::getNextFreeParticle()
{
	//find particle, this is a very inefficient way to do this.  A better way would be to keep a list of free particles so we can quickly find the first free one
	for(int index=0;index<maxParticles;index++)
	{
		//when we find a particle which is free
		if(!activeParticles[index].active)
		{
			activeParticles[index].active = true; //mark it as not free
			activeParticles[index].maxTime = time+particleMaxAge;  //record when the particle was created so we know when to remove it
			return index;
		}
	}
	return -1; //returns minus if a particle was not allocated
}


//releast a particle from the system using it's index to ID it
void ParticleFluidEmitter::releaseParticle(int index)
{
	if(index >= 0 && index < maxParticles)
		activeParticles[index].active = false;
}

//returns true if a particle age is greater than it's maximum allowed age
bool ParticleFluidEmitter::tooOld(int index)
{
	if(index >= 0 && index < maxParticles && time > activeParticles[index].maxTime)
		return true;
	return false;
}

//add particle to PhysX System
bool ParticleFluidEmitter::addPhysXParticle(int particleIndex)
{
	//reserve space for data
	PxParticleCreationData particleCreationData;
	//set up the data
	particleCreationData.numParticles = 1;  //spawn one particle at a time,  this is inefficient and we could improve this by passing in the list of particles.
	//set up the buffers
	PxU32 myIndexBuffer[] = {particleIndex};
	PxVec3 startPos = position;
	PxVec3 startVel(0,-200,0);
	//randomize starting velocity.
	startPos.x += (rand()%1000 - 500)/2000.0f;
	startPos.z += (rand()%1000 - 500)/2000.0f;

	//we can change starting position tos get different emitter shapes
	PxVec3 myPositionBuffer[] = {startPos};
	PxVec3 myVelocityBuffer[] =  {startVel};

	particleCreationData.indexBuffer = PxStrideIterator<const PxU32>(myIndexBuffer);
	particleCreationData.positionBuffer = PxStrideIterator<const PxVec3>(myPositionBuffer);
	particleCreationData.velocityBuffer = PxStrideIterator<const PxVec3>(myVelocityBuffer);
	// create particles in *PxParticleSystem* ps
	return pf->createParticles(particleCreationData);
}

//updateParticle
void ParticleFluidEmitter::upDate(float delta)
{
	//tick the emitter
	time += delta;
	respawnTime += delta;
	int numberSpawn = 0;
	//if respawn time is greater than our release delay then we spawn at least one particle so work out how many to spawn
	if(respawnTime > releaseDelay)
	{
		numberSpawn = (int)(respawnTime/releaseDelay);
		respawnTime -= (numberSpawn * releaseDelay);
	}
	// spawn the required number of particles 
	for(int count = 0;count < numberSpawn; count++)
	{
		//get the next free particle
		int particleIndex = getNextFreeParticle();
		if(particleIndex >= 0)
		{
			//if we got a particle ID then spawn it
			addPhysXParticle(particleIndex);
		}
	}
	//check to see if we need to release particles because they are either too old or have hit the particle sink
	//lock the particle buffer so we can work on it and get a pointer to read data
	PxParticleReadData* rd = pf->lockParticleReadData();
	// access particle data from PxParticleReadData was OK
	if (rd)
	{
		vector<PxU32> particlesToRemove; //we need to build a list of particles to remove so we can do it all in one go
		particlesToRemove.reserve(maxParticles/10);
		PxStrideIterator<const PxParticleFlags> flagsIt(rd->flagsBuffer);
		PxStrideIterator<const PxVec3> positionIt(rd->positionBuffer);

		for (unsigned i = 0; i < rd->validParticleRange; ++i, ++flagsIt, ++positionIt)
		{
			if (*flagsIt & PxParticleFlag::eVALID)
				{
					//if particle is either too old or has hit the sink then mark it for removal.  We can't remove it here because we buffer is locked
					if (*flagsIt & PxParticleFlag::eCOLLISION_WITH_DRAIN)
					{
						//mark our local copy of the particle free
						releaseParticle(i);
						//add to our list of particles to remove
						particlesToRemove.push_back(i);
					}
				}
		}
		// return ownership of the buffers back to the SDK
		rd->unlock();
		//if we have particles to release then pass the particles to remove to PhysX so it can release them
		if(particlesToRemove.size()>0)
		{
			//create a buffer of particle indicies which we are going to remove
			PxStrideIterator<const PxU32> indexBuffer(&particlesToRemove[0]);
			//free particles from the physics system
			pf->releaseParticles(particlesToRemove.size(), indexBuffer);
		}
	}
}

//simple routine to render our particles
void ParticleFluidEmitter::renderParticles()
{
	// lock SDK buffers of *PxParticleSystem* ps for reading
	PxParticleFluidReadData * fd = pf->lockParticleFluidReadData();
	// access particle data from PxParticleReadData
	float minX =1000,maxX = -1000,minZ = 1000,maxZ = -1000,minY = 1000,maxY = -1000;
	if (fd){
		PxStrideIterator<const PxParticleFlags> flagsIt(fd->flagsBuffer);
		PxStrideIterator<const PxVec3> positionIt(fd->positionBuffer);
		PxStrideIterator<const PxF32> densityIt(fd->densityBuffer);
		for (unsigned i = 0; i < fd->validParticleRange; ++i, ++flagsIt, ++positionIt,++densityIt){
			if (*flagsIt & PxParticleFlag::eVALID){
				//density tells us how many neighbours a particle has.  
				//If it has a density of 0 it has no neighbours, 1 is maximum neighbouts
				//we can use this to decide if the particle is seperate or part of a larger body of fluid

				glm::vec4 finalc(0,0,1,0.5f);
				float size = 0.8f;

				const PxF32 *val = densityIt.ptr();
				if (val != nullptr){
					glm::vec4 cWater(0,0,1,0.5f);
					glm::vec4 cWash(1.0f);
					finalc = glm::mix(cWash,cWater,(float)(*densityIt.ptr()));
				}else{
					printf("NULL density\n");
				}

				glm::vec3 pos(positionIt->x,positionIt->y,positionIt->z);

				Gizmos::addTri(pos + glm::vec3(0,-size,-size),pos + glm::vec3(0,-size, size),pos + glm::vec3(0, size, size),finalc);
				Gizmos::addTri(pos + glm::vec3(0,-size,-size),pos + glm::vec3(0, size, size),pos + glm::vec3(0, size,-size),finalc);
				Gizmos::addTri(pos + glm::vec3(0,-size,-size),pos + glm::vec3(0, size, size),pos + glm::vec3(0,-size, size),finalc);
				Gizmos::addTri(pos + glm::vec3(0,-size,-size),pos + glm::vec3(0, size,-size),pos + glm::vec3(0, size, size),finalc);

				Gizmos::addTri(pos + glm::vec3(-size,-size,0),pos + glm::vec3( size,-size,0),pos + glm::vec3( size, size,0),finalc);
				Gizmos::addTri(pos + glm::vec3(-size,-size,0),pos + glm::vec3( size, size,0),pos + glm::vec3(-size, size,0),finalc);
				Gizmos::addTri(pos + glm::vec3(-size,-size,0),pos + glm::vec3( size, size,0),pos + glm::vec3( size,-size,0),finalc);
				Gizmos::addTri(pos + glm::vec3(-size,-size,0),pos + glm::vec3(-size, size,0),pos + glm::vec3( size, size,0),finalc);

				Gizmos::addTri(pos + glm::vec3(-size,0,-size),pos + glm::vec3( size,0,-size),pos + glm::vec3( size,0, size),finalc);
				Gizmos::addTri(pos + glm::vec3(-size,0,-size),pos + glm::vec3( size,0, size),pos + glm::vec3(-size,0, size),finalc);
				Gizmos::addTri(pos + glm::vec3(-size,0,-size),pos + glm::vec3( size,0, size),pos + glm::vec3( size,0,-size),finalc);
				Gizmos::addTri(pos + glm::vec3(-size,0,-size),pos + glm::vec3(-size,0, size),pos + glm::vec3(-size,0, size),finalc);
			}
		}
		// return ownership of the buffers back to the SDK
		fd->unlock();
	}
}

