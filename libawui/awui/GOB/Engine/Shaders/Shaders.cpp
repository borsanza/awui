#include "Shaders.h"

#include <awui/GOB/Engine/Shaders/Shader.h>

using namespace awui::GOB::Engine;

Shader *Shaders::m_shader_default_texture = nullptr;

Shader *Shaders::ShaderDefaultView() {
	if (m_shader_default_texture == nullptr) {
		m_shader_default_texture = new Shader("shader-view.vert", "shader-view.frag");
	}

	return m_shader_default_texture;
}
