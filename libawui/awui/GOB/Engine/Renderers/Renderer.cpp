#include "Renderer.h"

#include <GL/glu.h>
#include <SDL_opengl.h>
#include <awui/Console.h>
#include <awui/GOB/Engine/Cameras/PerspectiveCamera.h>
#include <awui/GOB/Engine/Geometries/BoxGeometry.h>
#include <awui/GOB/Engine/Geometries/PlaneGeometry.h>
#include <awui/GOB/Engine/Materials/MeshBasicMaterial.h>
#include <awui/GOB/Engine/Objects/Mesh.h>
#include <awui/GOB/Engine/Textures/Texture.h>
#include <awui/Math.h>
#include <awui/OpenGL/GL.h>
#include <vector>

using namespace awui::GOB::Engine;
using namespace awui::Windows::Forms;
using namespace awui::OpenGL;

Renderer::Renderer() {
	m_angle = 0.0f;

	m_scene = new Scene();
	m_camera = new PerspectiveCamera(60, ((float) this->GetWidth()) / ((float) this->GetHeight()), 0.1, 1000);
	m_camera->SetPosition(5, 5, 10);
	m_camera->LookAt(0.5f, 0.5f, 0.5f);

	// new MeshBasicMaterial(0xff0000ff, false), // +X
	// new MeshBasicMaterial(0x800000ff, false), // -X
	// new MeshBasicMaterial(0x00ff00ff, false), // +Y
	// new MeshBasicMaterial(0x008000ff, false), // -Y
	// new MeshBasicMaterial(0x0000ffff, false), // +Z
	// new MeshBasicMaterial(0x000080ff, false)  // -Z

	//	Texture *texture = new Texture("./images/chip8.jpg", Texture::TEXTURE_NEAREST, Texture::TEXTURE_NEAREST);
	/*
	std::vector<Material *> materials = {
		new MeshBasicMaterial(new Texture("./images/button2.png", Texture::TEXTURE_NEAREST, Texture::TEXTURE_NEAREST), false), // +Y
		new MeshBasicMaterial(new Texture("./images/button2.png", Texture::TEXTURE_NEAREST, Texture::TEXTURE_NEAREST), false), // +Y
		new MeshBasicMaterial(texture, false),																				   // +Y
		new MeshBasicMaterial(new Texture("./images/button2.png", Texture::TEXTURE_NEAREST, Texture::TEXTURE_NEAREST), false), // +Y
		new MeshBasicMaterial(new Texture("./images/button2.png", Texture::TEXTURE_NEAREST, Texture::TEXTURE_NEAREST), false), // +Y
		new MeshBasicMaterial(new Texture("./images/button2.png", Texture::TEXTURE_NEAREST, Texture::TEXTURE_NEAREST), false), // +Y
	};
	*/
	std::vector<Material *> materials = {
		new MeshBasicMaterial(0xff0000ff, false), // +X
		new MeshBasicMaterial(0x800000ff, false), // -X
		new MeshBasicMaterial(0x00ff00ff, false), // +Y
		new MeshBasicMaterial(0x008000ff, false), // -Y
		new MeshBasicMaterial(0x0000ffff, false), // +Z
		new MeshBasicMaterial(0x000080ff, false), // -Z
	};

	int initMax = 270000;
	// int initMax = 12;
	int max = initMax;

	int line;
	for (line = 0; true; line++) {
		if (max <= 0) {
			break;
		}

		for (int lado = 0; lado <= 1; lado++) {
			for (int iy = 0; iy < line + lado; iy++) {
				if (max <= 0) {
					break;
				}

				BoxGeometry *geometry = new BoxGeometry(1, 1, 1);
				Mesh *cube = new Mesh(geometry, materials);
				cube->SetPosition(lado ? iy : line, 0.0f, lado ? line : iy);
				cube->SetScale(0.5f, 0.5f, 0.5f);
				//  cube->SetRotation(iy + line, iy + line, iy + line);
				m_scene->Add(cube);
				max -= 12;
			}
		}
	}

	Console::WriteLine("Center XY: %.0f", line / 2.0f);
	Console::WriteLine("Triangles: %d", initMax);
	m_camera->SetPosition(line / 2.0f, line * 0.666, 0.001f + 0);
	m_camera->LookAt(line / 2.0f, 0.0f, line * 0.333);

	// PlaneGeometry *geometry = new PlaneGeometry(10, 10);
	// Mesh *cube = new Mesh(geometry, materials);
	// cube->SetPosition(10, 0.0f, 10);
	// cube->SetScale(2.0f, 1.4f, 1);
	// cube->SetRotation(15, 0, 10);
	// m_scene->Add(cube);
}

void Renderer::DoRender(Scene &scene, Camera &camera) {
	camera.SetViewMatrix();
	camera.SetProjectionMatrix();

	Matrix4 identity = Matrix4::Identity();
	scene.PreRender(identity);

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
}

void awui::GOB::Engine::Renderer::OnTick(float deltaSeconds) {
	m_angle += 0.5f * deltaSeconds;
}

void Renderer::SetClearColor(uint32_t color) {
	m_clearColor = Color(color);
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

	glClearColor(m_clearColor.GetR() / 255.0f, m_clearColor.GetG() / 255.0f, m_clearColor.GetB() / 255.0f, m_clearColor.GetA() / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	m_camera->SetAspectRatio(((float) this->GetWidth()) / ((float) this->GetHeight()));
	m_camera->SetPosition(50.5f + -6.0f + 6.0f * Math::Cos(m_angle), 0.5f + Math::Cos(m_angle) * 5.0f, 50.5f + -4.0f + 8.0f * Math::Sin(m_angle));

	DoRender(*m_scene, *m_camera);

	glFlush();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}
