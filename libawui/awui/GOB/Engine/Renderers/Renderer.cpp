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
} // namespace

Renderer::Renderer() {
	m_scene = nullptr;
	m_camera = nullptr;
	m_wireframe = false;
}

void Renderer::SetScene(Scene *scene) {
	m_scene = scene;
	m_staticBuilt = false;
}

void Renderer::SetCamera(PerspectiveCamera *camera) {
	m_camera = camera;
}

void Renderer::SetClearColor(uint32_t color) {
	m_clearColor = Color(color);
}

void Renderer::SetWireframe(bool wireframe) {
	if (m_wireframe != wireframe) {
		m_wireframe = wireframe;
		m_staticBuilt = false; // Las líneas de la malla solo se calculan si hacen falta
	}
}

int Renderer::GetTriangleCount() const {
	return (int) ((m_static.list.GetVertices().size() + m_dynamic.list.GetVertices().size()) / 3);
}

int Renderer::GetDrawCalls() const {
	return (int) (m_static.list.GetBatches().size() + m_dynamic.list.GetBatches().size());
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

	for (Layer *layer : {&m_static, &m_dynamic}) {
		CreateBuffer(layer->triangles);
		CreateBuffer(layer->lines);
		CreateBuffer(layer->wireBuffer);
	}

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

// Calcula la geometría de una capa (lo fijo o lo dinámico) y la sube a OpenGL
void Renderer::Build(Layer &layer, bool dynamic) {
	GLenum usage = dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW;

	layer.list.Clear();
	m_scene->Collect(Matrix4::Identity(), layer.list, dynamic);

	const std::vector<RenderList::Vertex> &vertices = layer.list.GetVertices();
	Shaders::BindBuffer(GL_ARRAY_BUFFER, layer.triangles.vertexBuffer);
	Shaders::BufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(RenderList::Vertex), vertices.data(), usage);

	const std::vector<RenderList::Vertex> &lines = layer.list.GetLines();
	Shaders::BindBuffer(GL_ARRAY_BUFFER, layer.lines.vertexBuffer);
	Shaders::BufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(RenderList::Vertex), lines.data(), usage);

	// Modo malla: OpenGL ES no tiene glPolygonMode, así que las aristas se pintan como líneas. Cada triángulo da
	// tres, en el mismo orden: el tramo de cada textura empieza en el doble de su primer vértice
	layer.wire.clear();
	if (m_wireframe) {
		layer.wire.reserve(vertices.size() * 2);
		for (size_t i = 0; i + 2 < vertices.size(); i += 3) {
			layer.wire.insert(layer.wire.end(), {vertices[i], vertices[i + 1], vertices[i + 1], vertices[i + 2], vertices[i + 2], vertices[i]});
		}

		Shaders::BindBuffer(GL_ARRAY_BUFFER, layer.wireBuffer.vertexBuffer);
		Shaders::BufferData(GL_ARRAY_BUFFER, layer.wire.size() * sizeof(RenderList::Vertex), layer.wire.data(), usage);
	}
}

void Renderer::Draw(Layer &layer) {
	Shaders::BindVertexArray(m_wireframe ? layer.wireBuffer.vertexArray : layer.triangles.vertexArray);
	for (const RenderList::Batch &batch : layer.list.GetBatches()) {
		// Si la textura no se ha podido cargar, solo con el color
		GLuint texture = batch.texture ? batch.texture->GetTexture() : 0;
		Shaders::Uniform1i(m_texturedLocation, texture ? 1 : 0);
		if (m_wireframe)
			glDrawArrays(GL_LINES, batch.first * 2, batch.count * 2);
		else
			glDrawArrays(GL_TRIANGLES, batch.first, batch.count);
	}

	if (!layer.list.GetLines().empty()) {
		Shaders::BindVertexArray(layer.lines.vertexArray);
		Shaders::Uniform1i(m_texturedLocation, 0);
		glDrawArrays(GL_LINES, 0, (GLsizei) layer.list.GetLines().size());
	}
}

void Renderer::OnPaint(OpenGL::GL *gl) {
	if (!m_scene || !m_camera || !Initialize())
		return;

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(GetLeft(), GetTop(), GetWidth(), GetHeight());

	glClearColor(m_clearColor.GetR() / 255.0f, m_clearColor.GetG() / 255.0f, m_clearColor.GetB() / 255.0f, m_clearColor.GetA() / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_camera->SetAspectRatio(((float) GetWidth()) / ((float) GetHeight()));
	Matrix4 projection = m_camera->GetProjectionMatrix();
	Matrix4 view = m_camera->GetViewMatrix();

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

	// Lo fijo, solo si la escena ha cambiado; lo dinámico, siempre
	if (!m_staticBuilt || (m_staticChanges != Object3D::GetChanges())) {
		m_staticChanges = Object3D::GetChanges();
		m_staticBuilt = true;
		Build(m_static, false);
	}
	Build(m_dynamic, true);

	Draw(m_static);
	Draw(m_dynamic);

	// Como lo espera el pintado 2D de la interfaz
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}
