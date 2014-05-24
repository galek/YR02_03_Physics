#pragma once

#include <glm/ext.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

struct CollisionBox{
	CollisionBox(glm::vec3 v3_Origin,glm::vec3 v3_Extents){Origin = v3_Origin;Extents = v3_Extents;}
	glm::vec3 Origin;//Negative Position
	glm::vec3 Extents;//Positive Position
};

class Bucket{
public:
	Bucket(){};
	std::vector<CollisionBox> Objects;
};

class CollisionMap {
public:
	CollisionMap(int i_mapsize, int i_bucketCount);
	~CollisionMap(void);
	void AddBox(glm::vec3 v3_P1,glm::vec3 v3_P2);
	bool CheckMap(glm::vec3 v3_SelfPosition,glm::vec3& HitPos);
	void DrawMap();
	void GetLocation(glm::vec3 v3_SelfPosition,glm::vec2& v2_Pos);
protected:
	int iMapSize,iBucketSize,iBucketCount;
	Bucket **bBuckets;
};

