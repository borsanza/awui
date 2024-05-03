#include "Renderer.h"

#include <GL/glew.h>
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

// #define SIZE 39000000
#define SIZE 1800000

using namespace awui;
using namespace awui::GOB::Engine;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms;

Renderer::Renderer() {
	m_initialized = false;
}

void Renderer::Initialize() {
	m_initialized = true;

	m_angle = 0.0f;

	m_scene = new Scene();
	m_camera = new PerspectiveCamera(60, ((float) this->GetWidth()) / ((float) this->GetHeight()), 0.1, 1000);

	std::vector<Material *> materials = {
		new MeshBasicMaterial(new Texture("./images/block-pattern-0.png", Texture::TEXTURE_NEAREST_MIPMAP_NEAREST, Texture::TEXTURE_NEAREST), false), // +X
		new MeshBasicMaterial(new Texture("./images/block-pattern-1.png", Texture::TEXTURE_NEAREST_MIPMAP_NEAREST, Texture::TEXTURE_NEAREST), false), // -X
		new MeshBasicMaterial(new Texture("./images/block-pattern-2.png", Texture::TEXTURE_NEAREST_MIPMAP_NEAREST, Texture::TEXTURE_NEAREST), false), // +Y
		new MeshBasicMaterial(new Texture("./images/block-pattern-3.png", Texture::TEXTURE_NEAREST_MIPMAP_NEAREST, Texture::TEXTURE_NEAREST), false), // -Y
		new MeshBasicMaterial(new Texture("./images/block-pattern-4.png", Texture::TEXTURE_NEAREST_MIPMAP_NEAREST, Texture::TEXTURE_NEAREST), false), // +Z
		new MeshBasicMaterial(new Texture("./images/block-pattern-5.png", Texture::TEXTURE_NEAREST_MIPMAP_NEAREST, Texture::TEXTURE_NEAREST), false), // -Z
	};

	Texture *test = new Texture("./images/block-dirt.png", Texture::TEXTURE_NEAREST, Texture::TEXTURE_NEAREST);
	m_basic = new MeshBasicMaterial(test, false);

	int initMax;
	// initMax = 25000;
	// initMax = 250000;
	initMax = 12;
	int max = initMax;

	int line;
	for (line = 0; true; line++) {
		if (max <= 0)
			break;
		for (int lado = 0; lado <= 1; lado++) {
			if (max <= 0)
				break;
			for (int iy = 0; iy < line + lado; iy++) {
				if (max <= 0)
					break;
				BoxGeometry *geometry = new BoxGeometry(1, 1, 1);
				Mesh *cube = new Mesh(geometry, (iy % 2 == 0) ? materials : materials);
				cube->SetPosition(lado ? iy : line, 0.0f, lado ? line : iy);
				cube->SetScale(0.5f, 0.5f, 0.5f);
				// cube->SetRotation(iy + line, iy + line, iy + line);
				m_scene->Add(cube);
				max -= 12;
			}
		}
	}

	m_centerX = line / 2.0f;
	m_centerY = line / 2.0f;
	Console::WriteLine("Center XY: %.0f", line / 2.0f);
	Console::WriteLine("Triangles: %d", initMax);
	m_camera->SetPosition(m_centerX, m_centerX * 1.1f, 0.001f + 0);
	m_camera->LookAt(m_centerX, 1.0f, m_centerY);

	// PlaneGeometry *geometry = new PlaneGeometry(10, 10);
	// Mesh *cube = new Mesh(geometry, materials);
	// cube->SetPosition(10, 0.0f, 10);
	// cube->SetScale(2.0f, 1.4f, 1);
	// cube->SetRotation(15, 0, 10);
	// m_scene->Add(cube);

	int grid_size = (int) Math::Sqrt((float) SIZE); // 1000x1000 grid para simplificar, cada punto será un vértice
	std::vector<GLfloat> vertices;
	vertices.reserve(3 * grid_size * grid_size * 5); // Reserva espacio para 1,000,000 vértices con 5 componentes cada uno (x, y, z, u, v)

	float x, y, z, u, v;
	// Ejemplo de generación de datos: llenando el buffer con datos en forma de cuadrícula
	for (float ix = 0; ix < grid_size; ++ix) {
		for (float iy = 0; iy < grid_size; ++iy) {
			vertices.push_back(ix + 1.0f);
			vertices.push_back(0.0f);
			vertices.push_back(iy + 1.0f);
			vertices.push_back(1.0f);
			vertices.push_back(1.0f);

			vertices.push_back(ix + 1.0f);
			vertices.push_back(0.0f);
			vertices.push_back(iy);
			vertices.push_back(1.0f);
			vertices.push_back(0.0f);

			vertices.push_back(ix);
			vertices.push_back(0.0f);
			vertices.push_back(iy);
			vertices.push_back(0.0f);
			vertices.push_back(0.0f);
		}
	}

	glGenVertexArrays(1, &m_vao); // Generar un VAO
	glBindVertexArray(m_vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);				// Generar un buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // Vincular el buffer como un buffer de array

	// Copiar los datos de vértices al buffer
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	// Configurar el pointer de atributos de vértices para posiciones
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
	glEnableVertexAttribArray(0);

	// Configurar el pointer de atributos de vértices para coordenadas de textura
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Desvincular el VBO
}

void Renderer::DoRender(Scene &scene, Camera &camera) {
	// GL::CheckGLErrors("Renderer::DoRender(1)");

	Matrix4 view = camera.GetViewMatrix();
	Matrix4 projection = camera.GetProjectionMatrix();

	Matrix4 identity = Matrix4::Identity();

	// glBegin(GL_TRIANGLES);
	//  scene.PreRender(identity);
	// glEnd();

	// TODO: Quitarlo en el futuro
	glDisable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);

	m_basic->ApplyMaterial();

	GLuint shaderProgram = m_basic->GetTexture()->GetProgram();
	GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, identity.data());
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.data());
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.data());

	//	glColor3f(1.0, 0.0, 1.0);
	//	glBegin(GL_TRIANGLES);
	// glVertex3f(150.0f, 4.0, 150.0);
	// glVertex3f(150.0f, 4.0, 30.0);
	// glVertex3f(30.0f, 4.0, 30.0);
	//	glEnd();

	for (int i = 1; i <= 2; ++i) {
		glBindVertexArray(m_vao); // Suponiendo que has configurado un VAO
		int size = (int) Math::Sqrt((float) SIZE);
		glDrawArrays(GL_TRIANGLES, 0, size * size * 3);
		glBindVertexArray(0);
	}
	m_basic->UnApplyMaterial();
	/*
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_TRIANGLES);
		glVertex3f(30.0f, 4.0, 30.0);
		glVertex3f(30.0f, 4.0, 150.0);
		glVertex3f(150.0f, 4.0, 150.0);
		glEnd();

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
		*/
}

void Renderer::OnTick(float deltaSeconds) {
	m_angle += 0.3f * deltaSeconds;
}

void Renderer::SetClearColor(uint32_t color) {
	m_clearColor = Color(color);
}

void Renderer::OnPaint(OpenGL::GL *gl) {
	// GL::CheckGLErrors("Renderer::OnPaint(1)");
	if (!m_initialized) {
		Initialize();
	}

	glEnable(GL_DEPTH_TEST);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(this->GetLeft(), this->GetTop(), this->GetWidth(), this->GetHeight());

	glClearColor(m_clearColor.GetR() / 255.0f, m_clearColor.GetG() / 255.0f, m_clearColor.GetB() / 255.0f, m_clearColor.GetA() / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_camera->SetAspectRatio(((float) this->GetWidth()) / ((float) this->GetHeight()));
	m_camera->SetPosition(m_centerX + -6.0f + 6.0f * Math::Cos(m_angle), 7.0f + Math::Cos(m_angle) * 4.0f, m_centerY + -4.0f + 8.0f * Math::Sin(m_angle));

	DoRender(*m_scene, *m_camera);

	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}
