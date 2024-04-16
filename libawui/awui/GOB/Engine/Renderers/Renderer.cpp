#include "Renderer.h"

#include <GL/glu.h>
#include <SDL_opengl.h>
#include <awui/GOB/Engine/Cameras/PerspectiveCamera.h>
#include <awui/OpenGL/GL.h>

using namespace awui::GOB::Engine;
using namespace awui::Windows::Forms;
using namespace awui::OpenGL;

Renderer::Renderer() {
	m_offsetZ = 0.0f;
}

void Renderer::OnMenu(Control *sender) {
}

void Renderer::OnOk(Control *sender) {
}

void Renderer::OnExit(Control *sender) {
}

void Renderer::Render(Scene &scene, Camera &camera) {

	camera.SetViewMatrix();
	camera.SetProjectionMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_QUADS);

	glColor3ub(128, 0, 0);
	// Left
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glColor3ub(255, 0, 0);
	// Right
	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);

	glColor3ub(0, 255, 0);
	// Top
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);

	glColor3ub(0, 128, 0);
	// Bottom
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);

	glColor3ub(0, 0, 255);
	// Front
	glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, 1.0f);

	glColor3ub(0, 0, 128);
	// Back
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();
}

void awui::GOB::Engine::Renderer::OnTick(float deltaSeconds) {
	m_offsetZ -= 1.0f * deltaSeconds;
}

void Renderer::OnPaint(OpenGL::GL *gl) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// Guardar el estado de la matriz de vista
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	Scene scene;
	PerspectiveCamera camera(60, ((float) this->GetWidth()) / ((float) this->GetHeight()), 0.1, 1000);
	camera.SetPosition(Vector3(2, 2, 8 + m_offsetZ));
	camera.SetTarget(Vector3(0, 0, 0));
	Render(scene, camera);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// Restaurar el estado de la matriz de proyección
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}
