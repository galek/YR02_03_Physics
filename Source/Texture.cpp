#include "Texture.h"
#include <iostream>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>

Texture::Texture(const char *filename,int force_channels){
// loads a texture and returns just the OpenGL texture handle
	m_texture = SOIL_load_OGL_texture(filename,force_channels, SOIL_CREATE_NEW_ID, SOIL_FLAG_TEXTURE_REPEATS );
	glBindTexture( GL_TEXTURE_2D, m_texture);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	if (m_texture == 0){
		std::cout << "Failed to load texture : " << filename << std::endl;
	}
}

Texture::~Texture(void){}

CubeMap::CubeMap(const char *filename,const char *extension){
	// cube mapping must be enabled
	glEnable(GL_TEXTURE_CUBE_MAP);

	// genorate an  OpenGL texture and bind it as a GL_TEXTURE_CUBE_MAP
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
	
	// set some texture parameters...
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	// temporary width and height variables
	int width, height;
	
	// get the pixels for each of face of the cube and send the data to the graphics card
	std::string sPath = filename;
	sPath.append("top");
	sPath.append(extension);
	unsigned char *pixels_top		= SOIL_load_image(sPath.c_str(), &width, &height, NULL, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels_top );
	sPath = filename;
	sPath.append("bottom");
	sPath.append(extension);
	unsigned char *pixels_bottom	= SOIL_load_image(sPath.c_str(), &width, &height, NULL, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels_bottom );
	sPath = filename;
	sPath.append("north");
	sPath.append(extension);
	unsigned char *pixels_north		= SOIL_load_image(sPath.c_str(), &width, &height, NULL, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels_north );
	sPath = filename;
	sPath.append("south");
	sPath.append(extension);
	unsigned char *pixels_south		= SOIL_load_image(sPath.c_str(), &width, &height, NULL, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels_south );
	sPath = filename;
	sPath.append("east");
	sPath.append(extension);
	unsigned char *pixels_east		= SOIL_load_image(sPath.c_str(), &width, &height, NULL, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels_east );
	sPath = filename;
	sPath.append("west");
	sPath.append(extension);
	unsigned char *pixels_west		= SOIL_load_image(sPath.c_str(), &width, &height, NULL, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels_west );
	
	// delete the origional image data
	SOIL_free_image_data(pixels_top);
	SOIL_free_image_data(pixels_bottom);
	SOIL_free_image_data(pixels_north);
	SOIL_free_image_data(pixels_south);
	SOIL_free_image_data(pixels_east);
	SOIL_free_image_data(pixels_west);

	if (m_texture == 0){
		std::cout << "Failed to load texture : " << filename << std::endl;
	}
}

CubeMap::~CubeMap(void){}
