#include "Renderer.h"

#include <GL/glu.h>
#include <SDL_opengl.h>
#include <awui/GOB/Engine/Cameras/PerspectiveCamera.h>
#include <awui/GOB/Engine/Geometries/BoxGeometry.h>
#include <awui/GOB/Engine/Objects/Mesh.h>
#include <awui/Math.h>
#include <awui/OpenGL/GL.h>

using namespace awui::GOB::Engine;
using namespace awui::Windows::Forms;
using namespace awui::OpenGL;

Renderer::Renderer() {
	m_angle = 0.0f;

	m_scene = new Scene();
	m_camera = new PerspectiveCamera(60, ((float) this->GetWidth()) / ((float) this->GetHeight()), 0.1, 1000);
	m_camera->SetPosition(5, 5, 10);
	m_camera->LookAt(0.5f, 0.5f, 0.5f);

	for (int iy = 0; iy < 10; iy++) {
		for (int ix = 0; ix < 10; ix++) {
			BoxGeometry *geometry = new BoxGeometry(1, 1, 1);
			Mesh *cube = new Mesh(geometry);
			cube->SetPosition(ix * 0.8f, iy * 0.8f, 0);
			cube->SetScale(ix / 10.0f, iy / 10.0f, 1);
			// cube->SetRotation(ix, 0, 0);
			m_scene->Add(cube);
		}
	}
	m_scene->SetPosition(0.8f, 0.8f, 0);
	m_scene->SetScale(2.0f, 2.0f, 1);
	// m_scene->SetRotation(10, 0, 0);
}

void Renderer::Render(Scene &scene, Camera &camera) {

	camera.SetViewMatrix();
	camera.SetProjectionMatrix();
	scene.Render();

	glBegin(GL_LINES);

	float dif = 0.05f;
	float size = 4.0f;
	// Eje X en rojo
	glColor3f(0.5, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(size, 0.0, 0.0);
	glVertex3f(size, 0.0, 0.0);
	glVertex3f(size - dif, dif, 0.0);
	glVertex3f(size, 0.0, 0.0);
	glVertex3f(size - dif, 0.0, dif);
	glVertex3f(size, 0.0, 0.0);
	glVertex3f(size - dif, 0.0, -dif);
	glVertex3f(size, 0.0, 0.0);
	glVertex3f(size - dif, -dif, 0.0);

	// Eje Y en verde
	glColor3f(0.0, 0.5, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, size, 0.0);
	glVertex3f(0.0, size, 0.0);
	glVertex3f(dif, size - dif, 0.0);
	glVertex3f(0.0, size, 0.0);
	glVertex3f(-dif, size - dif, 0.0);
	glVertex3f(0.0, size, 0.0);
	glVertex3f(0.0, size - dif, dif);
	glVertex3f(0.0, size, 0.0);
	glVertex3f(0.0, size - dif, -dif);

	// Eje Z en azul
	glColor3f(0.0, 0.0, 0.5);
	glVertex3f(0.0, 0.0, 0.0);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, size);
	glVertex3f(0.0, 0.0, size);
	glVertex3f(dif, 0.0, size - dif);
	glVertex3f(0.0, 0.0, size);
	glVertex3f(-dif, 0.0, size - dif);
	glVertex3f(0.0, 0.0, size);
	glVertex3f(0.0, dif, size - dif);
	glVertex3f(0.0, 0.0, size);
	glVertex3f(0.0, -dif, size - dif);
	glEnd();

	/*
		float xpos = 0;
		float xsize = 1;
		float ypos = 0;
		float ysize = 1;
		float zpos = 0;
		float zsize = 1;
		float left = xpos - xsize / 2.0f;
		float right = xpos + xsize / 2.0f;
		float top = ypos + xsize / 2.0f;
		float bottom = ypos - xsize / 2.0f;
		float front = zpos - zsize / 2.0f;
		float back = zpos + zsize / 2.0f;

		glBegin(GL_QUADS);

		// +X
		glColor3ub(255, 0, 0);
		glVertex3f(right, top, front);
		glVertex3f(right, top, back);
		glVertex3f(right, bottom, back);
		glVertex3f(right, bottom, front);

		// -X
		glColor3ub(128, 0, 0);
		glVertex3f(left, bottom, front);
		glVertex3f(left, bottom, back);
		glVertex3f(left, top, back);
		glVertex3f(left, top, front);

		// +Y
		glColor3ub(0, 255, 0);
		glVertex3f(left, top, front);
		glVertex3f(left, top, back);
		glVertex3f(right, top, back);
		glVertex3f(right, top, front);

		// -Y
		glColor3ub(0, 128, 0);
		glVertex3f(right, bottom, front);
		glVertex3f(right, bottom, back);
		glVertex3f(left, bottom, back);
		glVertex3f(left, bottom, front);

		// +Z
		glColor3ub(0, 0, 255);
		glVertex3f(left, bottom, back);
		glVertex3f(right, bottom, back);
		glVertex3f(right, top, back);
		glVertex3f(left, top, back);

		// -Z
		glColor3ub(0, 0, 128);
		glVertex3f(left, top, front);
		glVertex3f(right, top, front);
		glVertex3f(right, bottom, front);
		glVertex3f(left, bottom, front);

		glEnd();
		*/
}

void awui::GOB::Engine::Renderer::OnTick(float deltaSeconds) {
	m_angle -= 0.5f * deltaSeconds;
}

void Renderer::OnPaint(OpenGL::GL *gl) {
	glEnable(GL_DEPTH_TEST);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(this->GetLeft(), this->GetTop(), this->GetWidth(), this->GetHeight());

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	m_camera->SetAspectRatio(((float) this->GetWidth()) / ((float) this->GetHeight()));
	// m_camera->SetPosition(0.5f + -6.0f + 6.0f * Math::Cos(m_angle), 0.5f + Math::Cos(m_angle) * 2.0f, 0.5f + -4.0f + 8.0f * Math::Sin(m_angle));

	Render(*m_scene, *m_camera);

	glFlush();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}
