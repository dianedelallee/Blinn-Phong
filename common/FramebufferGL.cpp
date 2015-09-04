#include "FramebufferGL.hpp"


int  build_framebuffer(FramebufferGL & fb, int width, int height, int outCount)
{
	fb.colorTexId = new GLuint[outCount];
	fb.drawBuffers = new GLenum[outCount];
	glGenTextures(outCount, fb.colorTexId);
	for (int i = 0; i != outCount; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, fb.colorTexId[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glGenTextures(1, &fb.depthTexId);
	glBindTexture(GL_TEXTURE_2D, fb.depthTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1, &fb.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);
	for (int i = 0; i != outCount; ++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, fb.colorTexId[i], 0);
		fb.drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fb.depthTexId, 0);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return -1;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	fb.width = width;
	fb.height = height;
	fb.outCount = outCount;

	glBindTexture(GL_TEXTURE_2D, 0);
	return 0;
}

int  destroy_framebuffer(FramebufferGL & fb)
{
	glDeleteFramebuffers(1, &(fb.fbo));
	glDeleteTextures(fb.outCount, fb.colorTexId);		
	glDeleteTextures(1, &(fb.depthTexId));		

	if (fb.colorTexId)
		delete[] fb.colorTexId;

	if (fb.drawBuffers)
		delete[] fb.drawBuffers;

	return 0;
}
