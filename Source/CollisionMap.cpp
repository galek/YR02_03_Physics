#include "CollisionMap.h"
#include <Gizmos.h>

CollisionMap::CollisionMap(int i_mapsize, int i_bucketCount){
	iMapSize = i_mapsize;
	iBucketCount = i_bucketCount;
	iBucketSize = iMapSize/iBucketCount;
	//dynamic 2D array of buckets
	bBuckets = new Bucket*[iBucketCount];
	for (int z = 0; z < iBucketCount; z++){
		bBuckets[z] = new Bucket[iBucketCount];
	}
}

CollisionMap::~CollisionMap(void){
	for (int z = 0; z < iMapSize; z++){
		delete bBuckets[z];
	}
	delete bBuckets;
}

void CollisionMap::AddBox(glm::vec3 v3_P1,glm::vec3 v3_P2){
	glm::ivec2 BucketPos = glm::ivec2((v3_P1.x + (iMapSize/2))/iBucketSize,(v3_P1.z + (iMapSize/2))/iBucketSize);
	BucketPos = glm::clamp(BucketPos,0,iBucketCount-1);
	bBuckets[BucketPos.x][BucketPos.y].Objects.push_back(CollisionBox(v3_P1,v3_P2));
}

bool CollisionMap::CheckMap(glm::vec3 v3_SelfPosition,glm::vec3& HitPos){
	glm::ivec2 BucketPos = glm::ivec2((v3_SelfPosition.x + (iMapSize/2))/iBucketSize,(v3_SelfPosition.z + (iMapSize/2))/iBucketSize);
	BucketPos = glm::clamp(BucketPos,0,iBucketCount-1);
	for (unsigned int i = 0; i < bBuckets[BucketPos.x][BucketPos.y].Objects.size(); i++){
		if (v3_SelfPosition.x > bBuckets[BucketPos.x][BucketPos.y].Objects[i].Origin.x - bBuckets[BucketPos.x][BucketPos.y].Objects[i].Extents.x && 
			v3_SelfPosition.y > bBuckets[BucketPos.x][BucketPos.y].Objects[i].Origin.y - bBuckets[BucketPos.x][BucketPos.y].Objects[i].Extents.y && 
			v3_SelfPosition.z > bBuckets[BucketPos.x][BucketPos.y].Objects[i].Origin.z - bBuckets[BucketPos.x][BucketPos.y].Objects[i].Extents.z && 
			v3_SelfPosition.x < bBuckets[BucketPos.x][BucketPos.y].Objects[i].Origin.x + bBuckets[BucketPos.x][BucketPos.y].Objects[i].Extents.x && 
			v3_SelfPosition.y < bBuckets[BucketPos.x][BucketPos.y].Objects[i].Origin.y + bBuckets[BucketPos.x][BucketPos.y].Objects[i].Extents.y && 
			v3_SelfPosition.z < bBuckets[BucketPos.x][BucketPos.y].Objects[i].Origin.z + bBuckets[BucketPos.x][BucketPos.y].Objects[i].Extents.z) {
			HitPos = glm::normalize(bBuckets[BucketPos.x][BucketPos.y].Objects[i].Origin - v3_SelfPosition);
			return false;
		}
	}
	HitPos = glm::vec3(0);
	return true;
}

void CollisionMap::DrawMap(){
	for (int y = -iBucketCount/2; y < iBucketCount/2; y++){
		for (int x = -iBucketCount/2; x < iBucketCount/2; x++){
			Gizmos::addAABB(glm::vec3((iBucketSize * x) + (iBucketSize/2),iBucketSize/2,(iBucketSize * y) + (iBucketSize/2)),glm::vec3(iBucketSize,iBucketSize/2,iBucketSize),glm::vec4(0,1,0,0.2f));
			for (unsigned int i = 0; i < bBuckets[x+(iBucketCount/2)][y+(iBucketCount/2)].Objects.size(); i++){
				Gizmos::addAABBFilled(bBuckets[x+(iBucketCount/2)][y+(iBucketCount/2)].Objects[i].Origin,bBuckets[x+(iBucketCount/2)][y+(iBucketCount/2)].Objects[i].Extents,glm::vec4(1,0,0,0.2f));
			}
		}
	}
}

void CollisionMap::GetLocation(glm::vec3 v3_SelfPosition,glm::vec2& v2_Pos){
	v2_Pos = glm::ivec2((v3_SelfPosition.x + (iMapSize/2))/iBucketSize,(v3_SelfPosition.z + (iMapSize/2))/iBucketSize);
}
