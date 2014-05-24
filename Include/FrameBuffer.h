#pragma once

class FrameBuffer {
public:
	FrameBuffer(int i_count,unsigned int ui_Width,unsigned int ui_Height);
	~FrameBuffer(void);
	void Bind();
	static void Unbind();
	unsigned int GetTexture(unsigned int ui_ID);
	unsigned int GetDepthTexture();
private:
	unsigned int* Textures;
	int iMaxTextures,iCount;
	unsigned int m_fbo;
	unsigned int uiWidth,uiHeight;
};

