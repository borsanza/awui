#pragma once

#include <awui/Drawing/Color.h>
#include <awui/GOB/Engine/Cameras/PerspectiveCamera.h>
#include <awui/GOB/Engine/Renderers/RenderList.h>
#include <awui/GOB/Engine/Scenes/Scene.h>
#include <awui/UI/Control.h>
#include <vector>

typedef unsigned int GLuint;

namespace awui::GOB::Engine {

	// Pinta la escena con shaders: igual con OpenGL 3.3 que con OpenGL ES 3.0
	class Renderer : public UI::Control {
	  private:
		float m_angle;
		Cameras::PerspectiveCamera *m_camera;
		Scene *m_scene;
		Drawing::Color m_clearColor = Drawing::Color(0.0f, 0.0f, 0.0f, 1.0f);

		// Vértices en OpenGL, con su formato
		struct Buffer {
			GLuint vertexArray = 0;
			GLuint vertexBuffer = 0;
		};

		bool m_initialized = false;
		bool m_failed = false;
		GLuint m_program = 0;
		Buffer m_scene3D;
		Buffer m_axes3D;
		bool m_sceneBuilt = false;
		unsigned int m_sceneChanges = 0; // Object3D::GetChanges() cuando se subió la escena
		int m_projectionLocation = -1;
		int m_viewLocation = -1;
		int m_texturedLocation = -1;
		int m_textureLocation = -1;
		RenderList m_list;						 // Los triángulos de la escena
		std::vector<RenderList::Vertex> m_axes; // Los ejes X, Y y Z, como líneas

		bool Initialize();
		void CreateBuffer(Buffer &buffer);

	  public:
		Renderer();

		void DoRender(Scene &scene, Cameras::Camera &camera);

		void SetClearColor(uint32_t color);

		virtual void OnTick(float deltaSeconds) override;
		virtual void OnPaint(OpenGL::GL *gl) override;
	};
} // namespace awui::GOB::Engine
