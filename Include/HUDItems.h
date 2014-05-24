#pragma once

#include <glm/glm.hpp> //use of vec2,vec3,vec4 

#include <BasicVertex.h>
#include <Texture.h>
#include <Shader.h>

#include <map>
#include <vector>

#define MAX_CHARS 1024

class HUDTexture {
public:
	//here for standardised reasons, not used or needed
	HUDTexture(glm::vec2 pos,glm::vec2 size);
	HUDTexture(void);
	~HUDTexture(void){};
	//Draw our plane
	void Draw();
protected:
	unsigned int m_vao,m_vbo,m_ibo;
	BasicVertex aoVertices[4];
	void Create(glm::vec2 pos = glm::vec2(0,0),glm::vec2 size = glm::vec2(1,1));
};

class Button {
public:
	Button(glm::vec2 pos,glm::vec2 size){Create(pos,size);}	
	Button(void){Create();}
	~Button(void){};

	void Draw();

	bool Update();
protected:
	unsigned int m_vao,m_vbo,m_ibo;
	BasicVertex aoVertices[4];
	void Create(glm::vec2 pos = glm::vec2(0,0),glm::vec2 size = glm::vec2(1,1));
};

class BitFont{
private:
	typedef struct sCharacter{
		int	id;
		int	width,height;
		int	xoffset,yoffset;
		int	xadvance;
		int	page;
		int	channel;
		float UV[4];
	};
//--------------------------- Info
	char* face;
	char* SpritePath;
	int size,charset,unicode,stretchH,padding[4],spacing[2],pageID,offset;
	int bold,italic,smooth,aa,outline;
//--------------------------- Common
	int lineheight,Base,Pages,Packed,TexW,TexH,charid;
	glm::vec4 Colour;
//--------------------------- Chars
	char iBuff[100];//100 char limit
	Texture *FontTex;
//-- Drawing Stuff
	BasicVertex bvVertices[4];

	sCharacter vChars[256];
	BasicVertex lVerticesToDraw[MAX_CHARS * 4];
	unsigned int lIndexs[MAX_CHARS * 6];

	unsigned int iCount,iCount2;
	unsigned int uiVBO,uiVAO,uiIBO;
	unsigned int ind[6];
public:
	BitFont(char* cs_Path);
	void SizeSet(int i_size){size = i_size;}
	int SizeGet(){return size;}
	void SetColour(glm::vec4 v4_Colour){Colour = v4_Colour;}

	int StrLen(char* string);

	void AddString(char* cs_String,float x, float y,float depth,bool center);
	void AddInt(int value,float x, float y,float depth,bool center);
	void AddFloat(float value,float x, float y,float depth,bool center);

	void Update();

	void Clear();
	void Draw(Shader* s_Shader);
};
