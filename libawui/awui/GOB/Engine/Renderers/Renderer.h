#pragma once

#include <awui/GOB/Engine/Cameras/Camera.h>
#include <awui/GOB/Engine/Scenes/Scene.h>
#include <awui/Windows/Forms/Control.h>
#include <awui/Windows/Forms/Listeners/IExitListener.h>
#include <awui/Windows/Forms/Listeners/IRemoteListener.h>

namespace awui {
	namespace GOB {
		namespace Engine {
			using namespace awui::GOB::Engine::Cameras;
			using namespace awui::Windows::Forms;
			using namespace awui::Windows::Forms::Listeners;

			class Renderer : public Control, public IRemoteListener, public IExitListener {
			  private:
				float m_offsetZ;

			  public:
				Renderer();
				virtual void OnMenu(Control *sender) override;
				virtual void OnOk(Control *sender) override;
				virtual void OnExit(Control *sender) override;

				void Render(Scene &scene, Camera &camera);

				virtual void OnTick(float deltaSeconds) override;
				virtual void OnPaint(OpenGL::GL *gl) override;
			};
		} // namespace Engine
	}	  // namespace GOB
} // namespace awui
