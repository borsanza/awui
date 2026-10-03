#include "Renderer.h"

#include <awui/Console.h>
#include <awui/GOB/Engine/Cameras/PerspectiveCamera.h>
#include <awui/GOB/Engine/Geometries/BoxGeometry.h>
#include <awui/GOB/Engine/Geometries/PlaneGeometry.h>
#include <awui/GOB/Engine/Materials/MeshBasicMaterial.h>
#include <awui/GOB/Engine/Objects/Mesh.h>
#include <awui/GOB/Engine/Textures/Texture.h>
#include <awui/Math.h>
#include <awui/OpenGL/GL.h>
#include <awui/OpenGL/Shaders.h>
#include <cstddef>
#include <vector>

using namespace awui::Drawing;
using namespace awui::GOB::Engine;
using namespace awui::UI;
using namespace awui::OpenGL;
using namespace awui::GOB::Engine::Cameras;

namespace {
	// La posición pasa por la vista y la proyección de la cámara (los vértices ya vienen en coordenadas del mundo)
	const char *VertexShader = R"(
uniform mat4 u_projection;
uniform mat4 u_view;
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec4 a_color;
out vec2 v_texCoord;
out vec4 v_color;

void main() {
	v_texCoord = a_texCoord;
	v_color = a_color;
	gl_Position = u_projection * u_view * vec4(a_position, 1.0);
}
)";

	const char *FragmentShader = R"(
uniform sampler2D u_texture;
uniform int u_textured;
in vec2 v_texCoord;
in vec4 v_color;
out vec4 fragColor;

void main() {
	vec4 color = v_color;
	if (u_textured != 0)
		color *= texture(u_texture, v_texCoord);
	fragColor = color;
}
)";

	RenderList::Vertex AxisVertex(float x, float y, float z, uint8_t r, uint8_t g, uint8_t b) {
		return {x, y, z, 0.0f, 0.0f, r, g, b, 255};
	}
} // namespace

Renderer::Renderer() {
	m_angle = 0.0f;

	m_scene = new Scene();
	m_camera = new PerspectiveCamera(60, ((float) GetWidth()) / ((float) GetHeight()), 0.1, 1000);
	m_camera->SetPosition(5, 5, 10);
	m_camera->LookAt(0.5f, 0.5f, 0.5f);

	std::vector<Material *> materials = {
		new MeshBasicMaterial(0xff0000ff, false), // +X
		new MeshBasicMaterial(0x800000ff, false), // -X
		new MeshBasicMaterial(0x00ff00ff, false), // +Y
		new MeshBasicMaterial(0x008000ff, false), // -Y
		new MeshBasicMaterial(0x0000ffff, false), // +Z
		new MeshBasicMaterial(0x000080ff, false)  // -Z
	};

	int initMax = 320000;
	int max = initMax;
	// int max = 12;

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

	// Los ejes, del origen hacia el lado positivo (de oscuro a claro) y con una punta de flecha: X rojo, Y verde, Z
	// azul
	float dif = 0.05f;
	float size = 4.0f;
	m_axes = {
		AxisVertex(0, 0, 0, 128, 0, 0), AxisVertex(size, 0, 0, 255, 0, 0),
		AxisVertex(size, 0, 0, 255, 0, 0), AxisVertex(size - dif, dif, 0, 255, 0, 0),
		AxisVertex(size, 0, 0, 255, 0, 0), AxisVertex(size - dif, 0, dif, 255, 0, 0),
		AxisVertex(size, 0, 0, 255, 0, 0), AxisVertex(size - dif, 0, -dif, 255, 0, 0),
		AxisVertex(size, 0, 0, 255, 0, 0), AxisVertex(size - dif, -dif, 0, 255, 0, 0),

		AxisVertex(0, 0, 0, 0, 128, 0), AxisVertex(0, size, 0, 0, 255, 0),
		AxisVertex(0, size, 0, 0, 255, 0), AxisVertex(dif, size - dif, 0, 0, 255, 0),
		AxisVertex(0, size, 0, 0, 255, 0), AxisVertex(-dif, size - dif, 0, 0, 255, 0),
		AxisVertex(0, size, 0, 0, 255, 0), AxisVertex(0, size - dif, dif, 0, 255, 0),
		AxisVertex(0, size, 0, 0, 255, 0), AxisVertex(0, size - dif, -dif, 0, 255, 0),

		AxisVertex(0, 0, 0, 0, 0, 128), AxisVertex(0, 0, size, 0, 0, 255),
		AxisVertex(0, 0, size, 0, 0, 255), AxisVertex(dif, 0, size - dif, 0, 0, 255),
		AxisVertex(0, 0, size, 0, 0, 255), AxisVertex(-dif, 0, size - dif, 0, 0, 255),
		AxisVertex(0, 0, size, 0, 0, 255), AxisVertex(0, dif, size - dif, 0, 0, 255),
		AxisVertex(0, 0, size, 0, 0, 255), AxisVertex(0, -dif, size - dif, 0, 0, 255),
	};

	// PlaneGeometry *geometry = new PlaneGeometry(10, 10);
	// Mesh *cube = new Mesh(geometry, materials);
	// cube->SetPosition(10, 0.0f, 10);
	// cube->SetScale(2.0f, 1.4f, 1);
	// cube->SetRotation(15, 0, 10);
	// m_scene->Add(cube);
}

bool Renderer::Initialize() {
	if (m_initialized || m_failed)
		return m_initialized;

	m_failed = true; // Hasta que todo vaya bien: si falla, no se vuelve a intentar en cada frame

	m_program = Shaders::BuildProgram(VertexShader, FragmentShader, "Renderer");
	if (!m_program)
		return false;

	m_projectionLocation = Shaders::GetUniformLocation(m_program, "u_projection");
	m_viewLocation = Shaders::GetUniformLocation(m_program, "u_view");
	m_texturedLocation = Shaders::GetUniformLocation(m_program, "u_textured");
	m_textureLocation = Shaders::GetUniformLocation(m_program, "u_texture");

	CreateBuffer(m_scene3D);
	CreateBuffer(m_axes3D);
	Shaders::BufferData(GL_ARRAY_BUFFER, m_axes.size() * sizeof(RenderList::Vertex), m_axes.data(), GL_STATIC_DRAW);

	m_initialized = true;
	m_failed = false;
	return true;
}

// Un buffer de vértices con su formato: posición, coordenada de textura y color (bytes, de 0 a 1 en el shader). Lo
// deja enlazado
void Renderer::CreateBuffer(Buffer &buffer) {
	Shaders::GenVertexArrays(1, &buffer.vertexArray);
	Shaders::GenBuffers(1, &buffer.vertexBuffer);
	Shaders::BindVertexArray(buffer.vertexArray);
	Shaders::BindBuffer(GL_ARRAY_BUFFER, buffer.vertexBuffer);
	Shaders::EnableVertexAttribArray(0);
	Shaders::VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RenderList::Vertex), (const void *) offsetof(RenderList::Vertex, x));
	Shaders::EnableVertexAttribArray(1);
	Shaders::VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(RenderList::Vertex), (const void *) offsetof(RenderList::Vertex, u));
	Shaders::EnableVertexAttribArray(2);
	Shaders::VertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(RenderList::Vertex), (const void *) offsetof(RenderList::Vertex, r));
}

void Renderer::DoRender(Scene &scene, Camera &camera) {
	Matrix4 projection = camera.GetProjectionMatrix();
	Matrix4 view = camera.GetViewMatrix();

	Shaders::UseProgram(m_program);
	Shaders::UniformMatrix4fv(m_projectionLocation, 1, GL_FALSE, projection.data());
	Shaders::UniformMatrix4fv(m_viewLocation, 1, GL_FALSE, view.data());
	Shaders::Uniform1i(m_textureLocation, 0);
	Shaders::ActiveTexture(GL_TEXTURE0);

	// Caras traseras ocultas y profundidad; sin mezcla, que los colores son opacos
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glDisable(GL_BLEND);

	// Los triángulos de la escena se calculan y se suben a OpenGL solo si ha cambiado (se han añadido o quitado
	// objetos): cada vértice se transforma una sola vez (Vector3::ApplyTransform), así que no se mueven
	Shaders::BindVertexArray(m_scene3D.vertexArray);
	if (!m_sceneBuilt || (m_sceneChanges != Object3D::GetChanges())) {
		m_list.Clear();
		scene.PreRender(Matrix4::Identity(), m_list);
		m_sceneChanges = Object3D::GetChanges();
		m_sceneBuilt = true;

		const std::vector<RenderList::Vertex> &vertices = m_list.GetVertices();
		Shaders::BindBuffer(GL_ARRAY_BUFFER, m_scene3D.vertexBuffer);
		Shaders::BufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(RenderList::Vertex), vertices.data(), GL_STATIC_DRAW);
	}

	for (const RenderList::Batch &batch : m_list.GetBatches()) {
		GLuint texture = batch.texture ? batch.texture->GetTexture() : 0;

		// Lo que lleva textura se pinta encima, sin profundidad y mezclando con su transparencia (como antes de los
		// shaders). Si la textura no se ha podido cargar, solo con el color
		if (texture) {
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		Shaders::Uniform1i(m_texturedLocation, texture ? 1 : 0);
		glDrawArrays(GL_TRIANGLES, batch.first, batch.count);

		if (texture) {
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
		}
	}

	Shaders::BindVertexArray(m_axes3D.vertexArray);
	Shaders::Uniform1i(m_texturedLocation, 0);
	glDrawArrays(GL_LINES, 0, (GLsizei) m_axes.size());

	// Como lo espera el pintado 2D de la interfaz
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
}

void awui::GOB::Engine::Renderer::OnTick(float deltaSeconds) {
	m_angle += 0.1f * deltaSeconds;
}

void Renderer::SetClearColor(uint32_t color) {
	m_clearColor = Color(color);
}

void Renderer::OnPaint(OpenGL::GL *gl) {
	if (!Initialize())
		return;

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(GetLeft(), GetTop(), GetWidth(), GetHeight());

	glClearColor(m_clearColor.GetR() / 255.0f, m_clearColor.GetG() / 255.0f, m_clearColor.GetB() / 255.0f, m_clearColor.GetA() / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_camera->SetAspectRatio(((float) GetWidth()) / ((float) GetHeight()));
	m_camera->SetPosition(50.5f + -6.0f + 6.0f * Math::Cos(m_angle), 0.5f + Math::Cos(m_angle) * 5.0f, 50.5f + -4.0f + 8.0f * Math::Sin(m_angle));

	DoRender(*m_scene, *m_camera);

	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}
