#include "DE_FrameBuffer.h"

DE_FrameBuffer::DE_FrameBuffer() : framebuffer(0), texColorBuffer(0), rbo(0), texBufferSize(float2::zero)
{
}

DE_FrameBuffer::~DE_FrameBuffer()
{
	ClearBuffer();
}

void DE_FrameBuffer::ClearBuffer()
{
	if (framebuffer != 0)
		glDeleteFramebuffers(1, (GLuint*)&framebuffer);

	if (texColorBuffer != 0)
		glDeleteTextures(1, (GLuint*)&texColorBuffer);

	if (rbo != 0)
		glDeleteRenderbuffers(1, (GLuint*)&rbo);
}

void DE_FrameBuffer::ReGenerateBuffer(int w, int h, bool MSAA, int msaaSamples)
{
	texBufferSize = float2(w, h);

	ClearBuffer();

	glGenFramebuffers(1, (GLuint*)&framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, (GLuint*)&texColorBuffer);

	auto textureTypr = (MSAA) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	glBindTexture(textureTypr, texColorBuffer);
	(MSAA) ? glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaaSamples, GL_RGB, w, h, GL_TRUE) : glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(textureTypr, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(textureTypr, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(textureTypr, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTypr, texColorBuffer, 0);



	glGenRenderbuffers(1, (GLuint*)&rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	if (MSAA)
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_DEPTH24_STENCIL8, w, h);
	else
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG(LogType::L_ERROR, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
