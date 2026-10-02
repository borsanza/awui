#pragma once

namespace awui::UI {
	class Form;

	// Tipo de contexto de OpenGL que se pide al crear la ventana
	enum class OpenGLProfile {
		Core,		   // OpenGL 3.3 moderno (sin el modo inmediato); si no lo hay, OpenGL ES 3.0. Por defecto
		ES,			   // OpenGL ES 3.0 (Raspberry Pi, Android); si no lo hay, OpenGL 3.3
		Compatibility, // OpenGL 3.3 con el modo inmediato antiguo: solo para lo que aún lo use (gameOfBlocks)
	};

	class Application {
		static int s_quit;

	  private:
		static void ProcessEvents();

	  public:
		Application() = delete; // Solo métodos estáticos

		static void Run(Form *form);

		// Antes de Run. La variable de entorno AWUI_GL_PROFILE (core, es o compat) manda sobre lo que se pida aquí,
		// para probar. No son inline: en Windows el ejecutable y la DLL tendrían cada uno su copia de la variable
		static void SetOpenGLProfile(OpenGLProfile profile);
		static OpenGLProfile GetOpenGLProfile();

		static void Quit();
	};
} // namespace awui::UI
