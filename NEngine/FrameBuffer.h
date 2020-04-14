#pragma once
#include <GL/glew.h>
#include "GLAssert.h"

struct FrameBuffer
{
	unsigned int fbo;
	unsigned int fbTexture;

	int fbWidth;
	int fbHeight;

	void Build()
	{
		std::cout << "Attempting to create FrameBuffer with size: " << fbWidth << ":" << fbHeight << std::endl;

		// Frame Buffer Object
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glGenTextures(1, &fbTexture);
		glBindTexture(GL_TEXTURE_2D, fbTexture);

		// Color
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fbWidth, fbHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbTexture, 0);

		// Stencil and depth buffer
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fbWidth, fbHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		// Stencil and depth - TEXTURE

		//glTexImage2D(
		//	GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0,
		//	GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
		//);
		//
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		else
			std::cout << "Framebuffer complete" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Bind()
	{
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
		GLCall(glEnable(GL_DEPTH_TEST)); // enable depth testing (is disabled for rendering screen-space quad)

		// not needed if rendered directly
		glViewport(0, 0, fbWidth, fbHeight);
	}

	void Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void DrawPre()
	{
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		GLCall(glDisable(GL_DEPTH_TEST));

		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
	}

	void BindTexture()
	{
		GLCall(glBindTexture(GL_TEXTURE_2D, fbTexture));

	}

	void Release()
	{
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(1, &fbTexture);
	}
};