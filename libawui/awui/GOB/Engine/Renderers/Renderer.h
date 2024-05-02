#pragma once

#include <awui/Core/Color.h>
#include <awui/GOB/Engine/Cameras/PerspectiveCamera.h>
#include <awui/GOB/Engine/Scenes/Scene.h>
#include <awui/Windows/Forms/Control.h>

typedef unsigned int GLuint;

namespace awui::GOB::Engine {
	class MeshBasicMaterial;

	using namespace awui::GOB::Engine::Cameras;
	using namespace awui::Windows::Forms;

	class Renderer : public Control {
	  private:
		float m_angle;
		PerspectiveCamera *m_camera;
		Scene *m_scene;
		Color m_clearColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
		float m_centerX, m_centerY;

		bool m_initialized;
		MeshBasicMaterial *m_basic;
		GLuint m_vao;

		void Initialize();

	  public:
		Renderer();

		void DoRender(Scene &scene, Camera &camera);

		void SetClearColor(uint32_t color);

		virtual void OnTick(float deltaSeconds) override;
		virtual void OnPaint(OpenGL::GL *gl) override;
	};
} // namespace awui::GOB::Engine
