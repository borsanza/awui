#pragma once

namespace awui::GOB::Engine {
	class Shader;

	class Shaders {
	  private:
		static Shader *m_shader_default_texture;

	  public:
		static Shader *ShaderDefaultView();
	};
} // namespace awui::GOB::Engine