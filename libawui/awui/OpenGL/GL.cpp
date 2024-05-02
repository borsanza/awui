/**
 * awui/OpenGL/GL.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "GL.h"

#include <GL/glew.h>
#include <SDL_opengl.h>
#include <awui/Drawing/Image.h>
#include <awui/GOB/Engine/Math/Matrix4.h>
#include <awui/GOB/Engine/Shaders/Shader.h>
#include <awui/GOB/Engine/Shaders/Shaders.h>
#include <awui/Math.h>
#include <iostream>

using namespace awui::Drawing;
using namespace awui::GOB::Engine;
using namespace awui::OpenGL;

GL::GL() {
}

void GL::SetClippingBase(awui::Drawing::Rectangle rectangle) {
	this->rectangle1 = rectangle;
}

void GL::SetClipping(awui::Drawing::Rectangle rectangle) {
	this->rectangle2 = rectangle;
}

awui::Drawing::Rectangle GL::GetClippingBase() {
	return this->rectangle1;
}

awui::Drawing::Rectangle GL::GetClipping() {
	return this->rectangle2;
}

awui::Drawing::Rectangle GL::GetClippingResult() {
	return Rectangle::Intersect(this->rectangle1, this->rectangle2);
}

void GL::SetClipping() {
	awui::Drawing::Rectangle rect = this->GetClippingResult();

	glScissor(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
}

#define OFFSET 0.5f

void GL::DrawLine(int x1, int y1, int x2, int y2) {
	int xinc = 0;
	int yinc = 0;
	int width = Math::Abs(x2 - x1);
	int height = Math::Abs(y2 - y1);

	if (width >= height) {
		if (x2 >= x1)
			xinc = 1;
		else
			xinc = -1;
	}

	if (height >= width) {
		if (y2 >= y1)
			yinc = 1;
		else
			yinc = -1;
	}

	glBegin(GL_LINES);
	glVertex2f(x1 + OFFSET, y1 + OFFSET);
	glVertex2f(x2 + xinc + OFFSET, y2 + yinc + OFFSET);
	glEnd();
}

// GL_CCW
void GL::DrawRectangle(int x1, int y1, int x2, int y2) {
	glBegin(GL_LINE_LOOP);
	glVertex2f(x1 + OFFSET, y2 + OFFSET); // Left Bottom
	glVertex2f(x2 + OFFSET, y2 + OFFSET); // Right Bottom
	glVertex2f(x2 + OFFSET, y1 + OFFSET); // Right Top
	glVertex2f(x1 + OFFSET, y1 + OFFSET); // Left Top
	glEnd();
}

// GL_CCW
void GL::FillRectangle(int x1, int y1, int x2, int y2) {
	glBegin(GL_QUADS);
	glVertex2f(x1, y2 + 1.0f);		  // Left Bottom
	glVertex2f(x2 + 1.0f, y2 + 1.0f); // Right Bottom
	glVertex2f(x2 + 1.0f, y1);		  // Right Top
	glVertex2f(x1, y1);				  // Left Top
	glEnd();
}

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

// GL_CCW
void GL::DrawImageGL(awui::Drawing::Image *image, int x, int y) {
	image->Load();

	GLboolean needDepthTest = glIsEnabled(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);

	GLboolean needBlend = glIsEnabled(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, image->GetTexture());

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2i(x, y + image->GetHeight()); // Left Bottom
	glTexCoord2f(1.0f, 1.0f);
	glVertex2i(x + image->GetWidth(), y + image->GetHeight()); // Right Bottom
	glTexCoord2f(1.0f, 0.0f);
	glVertex2i(x + image->GetWidth(), y); // Right Top
	glTexCoord2f(0.0f, 0.0f);
	glVertex2i(x, y); // Left Top
	glEnd();

	if (!needBlend)
		glDisable(GL_BLEND);
	if (needDepthTest)
		glEnable(GL_DEPTH_TEST);
}

void GL::DrawImageGL33(awui::Drawing::Image *image, int x, int y, float windowWidth, float windowHeight) {
	Matrix4 identity = Matrix4::Identity();
	Shader *shader = Shaders::ShaderDefaultView();

	image->Load();

	GLboolean needDepthTest = glIsEnabled(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);

	GLboolean needBlend = glIsEnabled(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shader->Use();

	glBindTexture(GL_TEXTURE_2D, image->GetTexture());

	GLuint shaderProgram = shader->GetProgram();
	GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, identity.data());
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, identity.data());
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, identity.data());

	float xpos = (float) x;
	float ypos = (float) y;

	float vertices[] = {xpos, ypos + image->GetHeight(), 0.0f, 1.0f, xpos + image->GetWidth(), ypos + image->GetHeight(), 1.0f, 1.0f, xpos + image->GetWidth(), ypos, 1.0f, 0.0f, xpos, ypos, 0.0f, 0.0f};
	// Transforma las coordenadas a espacio normalizado de -1 a 1
	for (int i = 0; i < 16; i += 4) {
		vertices[i] = 2.0f * (vertices[i] / windowWidth) - 1.0f;
		vertices[i + 1] = 2.0f * (vertices[i + 1] / windowHeight) - 1.0f;
	}

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Posición de vértice
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
	glEnableVertexAttribArray(0);

	// Coordenadas de textura
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, image->GetTexture());
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Cambio a GL_TRIANGLE_FAN para cubrir el cuadrado
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	shader->Unuse();

	if (!needBlend)
		glDisable(GL_BLEND);
	if (needDepthTest)
		glEnable(GL_DEPTH_TEST);
}

// GL_CCW
void GL::DrawImageGL(awui::Drawing::Image *image, int x, int y, int width, int height) {
	image->Load();
	// Mas rapido guardandose solo el valor y recuperarlo despues
	GLboolean oldTexture = glIsEnabled(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_2D);

	GLboolean oldDepth = glIsEnabled(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);

	GLboolean oldBlend = glIsEnabled(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, image->GetTexture());

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2i(x, y + height); // Left Bottom
	glTexCoord2f(1.0f, 1.0f);
	glVertex2i(x + width, y + height); // Right Bottom
	glTexCoord2f(1.0f, 0.0f);
	glVertex2i(x + width, y); // Right Top
	glTexCoord2f(0.0f, 0.0f);
	glVertex2i(x, y); // Left Top
	glEnd();

	if (!oldBlend)
		glDisable(GL_BLEND);
	if (oldDepth)
		glEnable(GL_DEPTH_TEST);
	if (!oldTexture)
		glDisable(GL_TEXTURE_2D);
}

bool GL::CheckGLErrors(const char *label) {
	bool ret = false;

	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cerr << "GL Error '" << label << "': " << error << " " << gluErrorString(error) << std::endl;
		ret = true;
	}

	return ret;
}