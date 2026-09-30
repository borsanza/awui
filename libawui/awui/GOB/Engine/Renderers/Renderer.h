#pragma once

#include <awui/Drawing/Color.h>
#include <awui/GOB/Engine/Cameras/PerspectiveCamera.h>
#include <awui/GOB/Engine/Scenes/Scene.h>
#include <awui/Windows/Forms/Control.h>

namespace awui::GOB::Engine {

	class Renderer : public Windows::Forms::Control {
	  private:
		float m_angle;
		Cameras::PerspectiveCamera *m_camera;
		Scene *m_scene;
		Drawing::Color m_clearColor = Drawing::Color(0.0f, 0.0f, 0.0f, 1.0f);

	  public:
		Renderer();

		void DoRender(Scene &scene, Cameras::Camera &camera);

		void SetClearColor(uint32_t color);

		virtual void OnTick(float deltaSeconds) override;
		virtual void OnPaint(OpenGL::GL *gl) override;
	};
} // namespace awui::GOB::Engine
