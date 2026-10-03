#pragma once

#include <awui/Drawing/Color.h>
#include <awui/GOB/Engine/Cameras/PerspectiveCamera.h>
#include <awui/GOB/Engine/Renderers/RenderList.h>
#include <awui/GOB/Engine/Scenes/Scene.h>
#include <awui/UI/Control.h>
#include <vector>

typedef unsigned int GLuint;

namespace awui::GOB::Engine {
	// Control que pinta una escena 3D vista desde una cámara, con shaders: igual con OpenGL 3.3 que con OpenGL ES 3.0.
	//
	// La geometría de lo que no se mueve se sube a OpenGL una vez y solo se rehace cuando la escena cambia (ver
	// Object3D); la de los objetos dinámicos se calcula y se sube en cada frame
	class Renderer : public UI::Control {
	  private:
		// Vértices en OpenGL, con su formato
		struct Buffer {
			GLuint vertexArray = 0;
			GLuint vertexBuffer = 0;
		};

		// Una capa de la escena: lo fijo o lo dinámico
		struct Layer {
			RenderList list;
			std::vector<RenderList::Vertex> wire; // Los triángulos como líneas (tres por triángulo), para el modo malla
			Buffer triangles;
			Buffer lines;
			Buffer wireBuffer;
		};

		Scene *m_scene;						 // No son suyas
		Cameras::PerspectiveCamera *m_camera;
		Drawing::Color m_clearColor = Drawing::Color(0.0f, 0.0f, 0.0f, 1.0f);
		bool m_wireframe;

		bool m_initialized = false;
		bool m_failed = false;
		GLuint m_program = 0;
		int m_projectionLocation = -1;
		int m_viewLocation = -1;
		int m_texturedLocation = -1;
		int m_textureLocation = -1;

		Layer m_static;
		Layer m_dynamic;
		bool m_staticBuilt = false;
		unsigned int m_staticChanges = 0; // Object3D::GetChanges() cuando se subió lo fijo

		bool Initialize();
		void CreateBuffer(Buffer &buffer);
		void Build(Layer &layer, bool dynamic);
		void Draw(Layer &layer);

	  public:
		Renderer();

		// La escena y la cámara que se pintan (no pasan a ser del Renderer)
		void SetScene(Scene *scene);
		void SetCamera(Cameras::PerspectiveCamera *camera);
		inline Cameras::PerspectiveCamera *GetCamera() const { return m_camera; }

		void SetClearColor(uint32_t color);

		// Modo malla: solo las aristas de los triángulos
		void SetWireframe(bool wireframe);
		inline bool GetWireframe() const { return m_wireframe; }

		// Para estadísticas: lo que se pinta en cada frame
		int GetTriangleCount() const;
		int GetDrawCalls() const;

		virtual void OnPaint(OpenGL::GL *gl) override;
	};
} // namespace awui::GOB::Engine
