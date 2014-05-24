#pragma once

#include <glm/glm.hpp>

// a basic vertex structure supporting position, colour and texture coordinate
struct BasicVertex
{
	enum Offsets
	{
		PositionOffset	= 0,
		ColourOffset	= PositionOffset + sizeof(glm::vec4),
		NormalOffset	= ColourOffset + sizeof(glm::vec4),
		TexCoordOffset	= NormalOffset + sizeof(glm::vec4),	
		TexCoord2Offset	= TexCoordOffset + sizeof(glm::vec2),			
	};

	BasicVertex(){
		position = glm::vec4(0);
		colour = glm::vec4(0);
		normal = glm::vec4(0);
		texCoord = glm::vec2(0);
		texCoord2 = glm::vec2(0);
	}

	BasicVertex(glm::vec4 p,glm::vec4 c,glm::vec4 n,glm::vec2 tc,glm::vec2 tc2){
		position =p;
		colour = c;
		normal = n;
		texCoord = tc;
		texCoord2 = tc2;
	}

	glm::vec4 position;
	glm::vec4 colour;
	glm::vec4 normal;
	glm::vec2 texCoord;
	glm::vec2 texCoord2;
};
