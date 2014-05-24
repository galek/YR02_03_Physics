#pragma once
#include <SOIL.h>

class Texture{
public:
	Texture(const char *filename,int force_channels = SOIL_LOAD_AUTO);
	~Texture(void);
protected:
	unsigned int m_texture;
public:
	unsigned int ID(){return m_texture;}
};

class CubeMap{
public:
	CubeMap(const char *filename,const char *extension);
	~CubeMap(void);
protected:
	unsigned int m_texture;
public:
	unsigned int ID(){return m_texture;}
};