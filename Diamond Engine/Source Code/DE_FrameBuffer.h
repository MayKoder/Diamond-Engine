#pragma once

#include"MathGeoLib/include/Math/float2.h"
#include"OpenGL.h"
#include"Globals.h"

class DE_FrameBuffer
{
public:
	DE_FrameBuffer();
	~DE_FrameBuffer();

	void ClearBuffer();

	void ReGenerateBuffer(int w, int h, bool MSAA, int msaaSamples);

	inline unsigned int GetFrameBuffer() { return framebuffer; }
	inline unsigned int GetTextureBuffer() { return texColorBuffer; }
	inline unsigned int GetRBO() { return rbo; }

	float2 texBufferSize;

private:

	unsigned int texColorBuffer;
	unsigned int framebuffer;
	unsigned int rbo;
};