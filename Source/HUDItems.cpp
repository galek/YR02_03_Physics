#include "HUDItems.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <TXML/tinyxml.h>
#include <Utilities.h>

//!--- 2D Texture

HUDTexture::HUDTexture(glm::vec2 pos,glm::vec2 size){
	Create(pos,size);
}

HUDTexture::HUDTexture(void) {
	Create();
}

void HUDTexture::Create(glm::vec2 pos,glm::vec2 size){
	glm::mat4 proj = glm::ortho<float>(0,Utility::Width,Utility::Height,0);

	aoVertices[0].position = proj * (glm::vec4(pos,0.0f,0.0f) + glm::vec4(-size.x/2, size.y/2,0.0f,1.0f)); aoVertices[0].colour = glm::vec4(1.0f); aoVertices[0].texCoord = glm::vec2(0.0f,0.0f);
	aoVertices[1].position = proj * (glm::vec4(pos,0.0f,0.0f) + glm::vec4( size.x/2, size.y/2,0.0f,1.0f)); aoVertices[1].colour = glm::vec4(1.0f); aoVertices[1].texCoord = glm::vec2(1.0f,0.0f);
	aoVertices[2].position = proj * (glm::vec4(pos,0.0f,0.0f) + glm::vec4( size.x/2,-size.y/2,0.0f,1.0f)); aoVertices[2].colour = glm::vec4(1.0f); aoVertices[2].texCoord = glm::vec2(1.0f,1.0f);
	aoVertices[3].position = proj * (glm::vec4(pos,0.0f,0.0f) + glm::vec4(-size.x/2,-size.y/2,0.0f,1.0f)); aoVertices[3].colour = glm::vec4(1.0f); aoVertices[3].texCoord = glm::vec2(0.0f,1.0f);

	unsigned int ind[6] = {0,1,2,0,2,3};

	// create opengl buffers
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);	

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(BasicVertex), aoVertices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), ind , GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::PositionOffset);
	glEnableVertexAttribArray(1); // colour
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::ColourOffset);
	glEnableVertexAttribArray(2); // UV
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::TexCoordOffset);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void HUDTexture::Draw(){
	// draw particles
	glBindVertexArray( m_vao );
	glDrawElements(GL_TRIANGLES, 6 , GL_UNSIGNED_INT, 0);
}
	
//!--- BUTTONS
void Button::Create(glm::vec2 pos,glm::vec2 size){
	glm::mat4 proj = glm::ortho<float>(0,Utility::Width,Utility::Height,0);

	aoVertices[0].position = proj * (glm::vec4(pos,0.0f,0.0f) + glm::vec4(-size.x/2, size.y/2,0.0f,1.0f)); aoVertices[0].colour = glm::vec4(1.0f); aoVertices[0].texCoord = glm::vec2(0.0f,1.0f);
	aoVertices[1].position = proj * (glm::vec4(pos,0.0f,0.0f) + glm::vec4( size.x/2, size.y/2,0.0f,1.0f)); aoVertices[1].colour = glm::vec4(1.0f); aoVertices[1].texCoord = glm::vec2(1.0f,1.0f);
	aoVertices[2].position = proj * (glm::vec4(pos,0.0f,0.0f) + glm::vec4( size.x/2,-size.y/2,0.0f,1.0f)); aoVertices[2].colour = glm::vec4(1.0f); aoVertices[2].texCoord = glm::vec2(1.0f,0.0f);
	aoVertices[3].position = proj * (glm::vec4(pos,0.0f,0.0f) + glm::vec4(-size.x/2,-size.y/2,0.0f,1.0f)); aoVertices[3].colour = glm::vec4(1.0f); aoVertices[3].texCoord = glm::vec2(0.0f,0.0f);

	unsigned int ind[6] = {0,1,2,0,2,3};

	// create opengl buffers
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);	

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(BasicVertex), aoVertices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), ind , GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::PositionOffset);
	glEnableVertexAttribArray(1); // colour
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::ColourOffset);
	glEnableVertexAttribArray(2); // UV
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::TexCoordOffset);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Button::Draw() {
	// draw particles
	glBindVertexArray( m_vao );
	glDrawElements(GL_TRIANGLES, 6 , GL_UNSIGNED_INT, 0);
}

bool Button::Update() {
	GLFWwindow* window = glfwGetCurrentContext();
	int ix,iy;
	Utility::getCursorPos(ix,iy);
	float fx = ix,fy = iy;
	Utility::ConvertScreenToView(fx,fy);

	if (fx < aoVertices[1].position.x && 
		fx > aoVertices[3].position.x && 
		fy > aoVertices[1].position.y && 
		fy < aoVertices[3].position.y ){
		if (aoVertices[0].colour != glm::vec4(1,0,0,1)){
			aoVertices[0].colour = aoVertices[1].colour = aoVertices[2].colour = aoVertices[3].colour = glm::vec4(0.6,0.6,0.8,1);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(BasicVertex), &aoVertices);
		}
		if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_1) == GLFW_PRESS){
			return true;
		}
		return false;
	}	
	if (aoVertices[0].colour != glm::vec4(1,1,1,0.5f)){
		aoVertices[0].colour = 	aoVertices[1].colour = aoVertices[2].colour = aoVertices[3].colour = glm::vec4(1,1,1,1.0f);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(BasicVertex), &aoVertices);
	}
	return false;
}

//!--- BITFONTS

BitFont::BitFont(char* cs_Path) {

	iCount = iCount2 = 0;

	ind[0] = 0;	
	ind[1] = 1;	
	ind[2] = 2;	
	ind[3] = 0;	
	ind[4] = 2;	
	ind[5] = 3;

	// create opengl buffers
	glGenVertexArrays(1, &uiVAO);
	glBindVertexArray(uiVAO);

	glGenBuffers(1, &uiVBO);
	glGenBuffers(1, &uiIBO);	

	glBindBuffer(GL_ARRAY_BUFFER, uiVBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * MAX_CHARS * sizeof(BasicVertex), nullptr, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uiIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * MAX_CHARS * sizeof(unsigned int), nullptr , GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0); // position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::PositionOffset);
	glEnableVertexAttribArray(1); // colour
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::ColourOffset);
	glEnableVertexAttribArray(2); // UV
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*) + BasicVertex::TexCoordOffset);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Load the XML
	//printf("------------- Loading XML file of path : '%s' \n",Path);
	TiXmlDocument doc(cs_Path);
    if (doc.LoadFile()){
	//Check its a font
		TiXmlNode* xFont = doc.FirstChild("font");
		if (!xFont){
			printf("Could not find 'font' Node. \n");
			return;
		};
	//Check it has info
		TiXmlNode* xInfo = xFont->FirstChild("info");
		if (!xInfo){
			printf("Could not find 'info' Node. \n");
			return;
		}
		TiXmlElement * xInfoElement = xInfo->ToElement();
	//Define all the INFO variables
		face = (char*)xInfoElement->Attribute("face");
		xInfoElement->QueryIntAttribute("size",&size);
		xInfoElement->QueryIntAttribute("charset",&charset);
		xInfoElement->QueryIntAttribute("unicode",&unicode);
		xInfoElement->QueryIntAttribute("stretchH",&stretchH);
		char* temppadd = (char*)xInfoElement->Attribute("padding");
		for (int i = 0; i < (int)strlen(temppadd); i+=2){
			padding[i] = (int)temppadd[i] - '0';
		}
		char* tempspac = (char*)xInfoElement->Attribute("spacing");
		for (int i = 0; i < (int)strlen(tempspac); i+=2){
			spacing[i] = (int)tempspac[i] - '0';
		}
		xInfoElement->QueryIntAttribute("bold",&bold);
		xInfoElement->QueryIntAttribute("italic",&italic);
		xInfoElement->QueryIntAttribute("smooth",&smooth);
		xInfoElement->QueryIntAttribute("aa",&aa);
		xInfoElement->QueryIntAttribute("outline",&outline);
	//Check it has common
		TiXmlNode* xCommon = xFont->FirstChild("common");
		if (!xCommon){
			printf("Could not find 'common' Node. \n");
			return;
		}
	//Define all the COMMON vars
		TiXmlElement * xCommonElement = xCommon->ToElement();
		xCommonElement->QueryIntAttribute("lineHeight",&lineheight);
		xCommonElement->QueryIntAttribute("base"      ,&Base);
		xCommonElement->QueryIntAttribute("scaleW"    ,&TexW);
		xCommonElement->QueryIntAttribute("scaleH"    ,&TexH);
		xCommonElement->QueryIntAttribute("pages"     ,&Pages);
		xCommonElement->QueryIntAttribute("packed"    ,&Packed);
		//xCommonElement->QueryFloatAttribute("alphaChnl" ,&Colour.a);
		//xCommonElement->QueryFloatAttribute("redChnl"   ,&Colour.r);
		//xCommonElement->QueryFloatAttribute("greenChnl" ,&Colour.g);
		//xCommonElement->QueryFloatAttribute("blueChnl"  ,&Colour.b);
		Colour = glm::vec4(1.0f);
	//Check it has pages
		TiXmlNode* xPages = xFont->FirstChild("pages");
		if (!xFont){
			printf("Could not find 'pages' Node. \n");
			return;
		}
		TiXmlElement * xPage = xPages->FirstChildElement();
		xPage->QueryIntAttribute("id",&pageID);
		SpritePath = (char*)xPage->Attribute("file");
	//Create Font
		std::string tStr;
		tStr.append(cs_Path);

		int found = (int)tStr.find_last_of('/');
		tStr = tStr.substr(0,found+1);
		tStr.append(SpritePath);

		////////////////////////LOAD TEXTURE HERE
		FontTex = new Texture( tStr.c_str() );

	//Check it has chars
		TiXmlNode* xChars = xFont->FirstChild("chars");
		if (!xFont){
			printf("Could not find 'chars' Node. \n");
			return;
		}
		TiXmlElement * xCharsElement = xChars->ToElement();
		TiXmlElement * XChar = xCharsElement->FirstChildElement();
	//Loop through the chars, assigning them to the Array
		int x,y;
		do {
			XChar->QueryIntAttribute("id",&charid);
			vChars[charid].id = charid;
			XChar->QueryIntAttribute("x",&x);
			XChar->QueryIntAttribute("y",&y);
			XChar->QueryIntAttribute("width",&vChars[charid].width);
			XChar->QueryIntAttribute("height",&vChars[charid].height);
			vChars[charid].UV[0] =        x                       /(float)TexW ;
			vChars[charid].UV[1] = ((y+vChars[charid].height)/(float)TexH);
			vChars[charid].UV[2] =       (x+vChars[charid].width )/(float)TexW ;
			vChars[charid].UV[3] = ( y                       /(float)TexH);
			XChar->QueryIntAttribute("xoffset",&vChars[charid].xoffset);
			XChar->QueryIntAttribute("yoffset",&vChars[charid].yoffset);
			XChar->QueryIntAttribute("xadvance",&vChars[charid].xadvance);
			XChar->QueryIntAttribute("page",&vChars[charid].page);
			XChar->QueryIntAttribute("chnl",&vChars[charid].channel);
			XChar = XChar->NextSiblingElement();
		}while (XChar != NULL);
		printf("Loaded XML file of path : '%s' \n\n",cs_Path);
		return;
	}else{// if (doc.LoadFile())
		printf("Error Loading XML file of path : '%s' \n\n",cs_Path);
	}
}

void BitFont::AddString(char* cs_String,float x, float y,float depth,bool center){
	if (iCount >= MAX_CHARS * 4 || iCount2 >= MAX_CHARS * 6){return;}

	glm::mat4 proj = glm::ortho<float>(0,Utility::Width,Utility::Height,0);

	float tSize = 1.0f;
	offset = 0;
	if (center){
		x-=StrLen(cs_String)*1.5f;
	}
//Get SIZE
	if (size!=lineheight){
		tSize = size/(float)lineheight;
	}
	for (int i = 0; i < (int)strlen(cs_String); i++){
//Get the CHAR ID
		charid = cs_String[i];
//Draw Letter
		glm::vec4 v4t = glm::vec4(x + offset + vChars[charid].xoffset * tSize  ,y + vChars[charid].yoffset * tSize,depth ,1.0f);
		glm::vec2 v2s = glm::vec2(vChars[charid].width  * tSize, vChars[charid].height * tSize);
//insert new stuff at the end of our vector

		lVerticesToDraw[iCount + 0].position = proj * (v4t + glm::vec4(    0, v2s.y,0,0)); lVerticesToDraw[iCount + 0].colour = Colour; lVerticesToDraw[iCount + 0].texCoord = glm::vec2(vChars[charid].UV[0],vChars[charid].UV[1]);
		lVerticesToDraw[iCount + 1].position = proj * (v4t + glm::vec4(v2s.x, v2s.y,0,0)); lVerticesToDraw[iCount + 1].colour = Colour; lVerticesToDraw[iCount + 1].texCoord = glm::vec2(vChars[charid].UV[2],vChars[charid].UV[1]);
		lVerticesToDraw[iCount + 2].position = proj * (v4t + glm::vec4(v2s.x,     0,0,0)); lVerticesToDraw[iCount + 2].colour = Colour; lVerticesToDraw[iCount + 2].texCoord = glm::vec2(vChars[charid].UV[2],vChars[charid].UV[3]);
		lVerticesToDraw[iCount + 3].position = proj * (v4t + glm::vec4(    0,     0,0,0)); lVerticesToDraw[iCount + 3].colour = Colour; lVerticesToDraw[iCount + 3].texCoord = glm::vec2(vChars[charid].UV[0],vChars[charid].UV[3]);
		
		lIndexs[iCount2 + 0] = (iCount + ind[0] ); 
		lIndexs[iCount2 + 1] = (iCount + ind[1] ); 
		lIndexs[iCount2 + 2] = (iCount + ind[2] );
		lIndexs[iCount2 + 3] = (iCount + ind[3] ); 
		lIndexs[iCount2 + 4] = (iCount + ind[4] ); 
		lIndexs[iCount2 + 5] = (iCount + ind[5] );

		if (iCount  < MAX_CHARS * 4){iCount  += 4;}else{return;}
		if (iCount2 < MAX_CHARS * 6){iCount2 += 6;}else{return;}
//Move along for drawing
		offset+=(int)(vChars[charid].xadvance * tSize);
	}
}

void BitFont::AddInt(int value,float x, float y,float depth,bool center){
	sprintf(iBuff, "%i", value);//convert int to char array
	AddString(iBuff,x,y,depth,center);//pass char array containing int
}

void BitFont::AddFloat(float value,float x, float y,float depth,bool center){
	sprintf(iBuff, "%f", value);//convert float to char array
	AddString(iBuff,x,y,depth,center);//pass char array containing float 
}

int BitFont::StrLen(char* string){
	offset = 0;
	for (int i = 0; i < (int)strlen(string); i++){
		offset+=(int)(vChars[string[i]].xadvance * (size/(float)lineheight));
	}
	return offset;
}

void BitFont::Clear(){
	iCount = 0;
	iCount2 = 0;
}

void BitFont::Update() {
	if (iCount > 0 && iCount2 > 0){
		glBindBuffer(GL_ARRAY_BUFFER, uiVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, iCount * sizeof(BasicVertex), &lVerticesToDraw);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uiIBO);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, iCount2 * sizeof(unsigned int), &lIndexs);
	}
}

void BitFont::Draw(Shader* s_Shader) {
	if (iCount > 0 && iCount2 > 0){
		s_Shader->SetTexture("DiffuseTexture",4,FontTex->ID());
		glBindVertexArray( uiVAO );
		glDrawElements(GL_TRIANGLES, iCount2 , GL_UNSIGNED_INT, 0);
	}
}