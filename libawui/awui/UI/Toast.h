#pragma once

#include <awui/UI/Control.h>

namespace awui {
	namespace Drawing {
		class Image;
	}

	namespace UI {
		// Aviso breve en pantalla ("Estado guardado", "Ranura 2"...): un recuadro redondeado que sube desde el borde
		// inferior, se queda un momento y vuelve a bajar desvaneciéndose. Uno nuevo sustituye al que
		// se esté viendo. Cada Form tiene uno (Control::ShowNotification)
		class Toast : public Control {
		  private:
			Drawing::Image *m_image; // Recuadro con el texto, ya dibujado (con su sombra)
			float m_elapsed;		 // Segundos desde que se mostró; negativo: no hay aviso
			float m_opacity;
			int m_anchorLeft;	 // Dónde se queda quieto: borde izquierdo...
			int m_anchorBottom;	 // ... y borde inferior (lo fija el formulario)

			void UpdatePosition(float offset);

		  public:
			static constexpr float InSeconds = 0.35f;
			static constexpr float VisibleSeconds = 2.0f;
			static constexpr float OutSeconds = 0.3f;

			static constexpr int PaddingX = 22;
			static constexpr int PaddingY = 12;
			static constexpr int AccentWidth = 7; // Franja de color a la izquierda
			static constexpr int Radius = 12;
			static constexpr int Shadow = 8; // Margen para la sombra alrededor del recuadro

			Toast();
			virtual ~Toast();

			void Show(const String &text);
			inline bool IsShowing() const { return m_elapsed >= 0.0f; }

			// Posición de reposo: borde izquierdo y borde inferior del recuadro, en coordenadas del padre
			void SetAnchor(int left, int bottom);

			virtual void OnTick(float deltaSeconds) override;
			virtual void OnPaint(OpenGL::GL *gl) override;
		};
	} // namespace UI
} // namespace awui
