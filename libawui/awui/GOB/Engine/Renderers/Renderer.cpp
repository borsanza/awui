#include "Renderer.h"

#include <GL/glu.h>
#include <SDL_opengl.h>
#include <awui/GOB/Engine/Cameras/PerspectiveCamera.h>
#include <awui/OpenGL/GL.h>

using namespace awui::GOB::Engine;
using namespace awui::Windows::Forms;
using namespace awui::OpenGL;

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
	glColor3ub(255, 0, 0);
	glBegin(GL_QUADS);
	glVertex3f(0, 0, 0);
	glVertex3f(1.0f, 0, 0);
	glVertex3f(1.0f, 1.0f, 0);
	glVertex3f(0, 1.0f, 0);
	glEnd();
}

void Renderer::OnPaint(OpenGL::GL *gl) {
	static float offsetZ = 0.0f;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// Guardar el estado de la matriz de vista
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	Scene scene;
	PerspectiveCamera camera(60, ((float) this->GetWidth()) / ((float) this->GetHeight()), 0.1, 1000);
	camera.SetPosition(Vector3(0, 2, 8 + offsetZ));
	offsetZ -= 0.05f;
	camera.SetTarget(Vector3(0, 0, 0));
	Render(scene, camera);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// Restaurar el estado de la matriz de proyección
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}