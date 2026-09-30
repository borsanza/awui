#pragma once

#include <awui/Drawing/GlyphMetrics.h>
#include <awui/UI/Control.h>

namespace awui {
	namespace Drawing {
		class Image;
	}

	namespace UI {
		// Aviso breve en pantalla ("Estado guardado", "Ranura 2"...): aparece con un fundido, se queda un momento y
		// desaparece. Uno nuevo sustituye al que se esté viendo. Cada Form tiene uno (Form::ShowNotification)
		class Toast : public Control {
		  private:
			Drawing::Image *m_image; // Texto ya dibujado
			Drawing::GlyphMetrics m_metrics;
			float m_elapsed; // Segundos desde que se mostró; negativo: no hay aviso
			float m_opacity;

		  public:
			static constexpr float FadeInSeconds = 0.15f;
			static constexpr float VisibleSeconds = 2.0f;
			static constexpr float FadeOutSeconds = 0.4f;
			static constexpr int PaddingX = 18;
			static constexpr int PaddingY = 10;

			Toast();
			virtual ~Toast();

			void Show(const String &text);
			inline bool IsShowing() const { return m_elapsed >= 0.0f; }

			virtual void OnTick(float deltaSeconds) override;
			virtual void OnPaint(OpenGL::GL *gl) override;
		};
	} // namespace UI
} // namespace awui
