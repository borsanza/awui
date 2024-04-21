#pragma once

#include <awui/GOB/Engine/Cameras/PerspectiveCamera.h>
#include <awui/GOB/Engine/Scenes/Scene.h>
#include <awui/Windows/Forms/Control.h>

namespace awui::GOB::Engine {
	using namespace awui::GOB::Engine::Cameras;
	using namespace awui::Windows::Forms;

	class Renderer : public Control {
	  private:
		float m_angle;
		PerspectiveCamera *m_camera;
		Scene *m_scene;

	  public:
		Renderer();

		void DoRender(Scene &scene, Camera &camera);

		virtual void OnTick(float deltaSeconds) override;
		virtual void OnPaint(OpenGL::GL *gl) override;
	};
} // namespace awui::GOB::Engine
