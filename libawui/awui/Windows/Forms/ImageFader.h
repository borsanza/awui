#pragma once

#include <awui/Drawing/ColorF.h>
#include <awui/Windows/Forms/Control.h>

namespace awui {
   	namespace OpenGL {
		class GL;
	}

	namespace Windows {
		namespace Forms {
            class Bitmap;

			class ImageFader : public Control {
				public:
					// Definir el enum class para los estados de transición de la imagen
					enum class State {
						Paused,  // No hay transición en progreso
						FadeOut, // Transición de desvanecimiento hacia fuera
						FadeIn   // Transición de desvanecimiento hacia dentro
					};
				private:
					Bitmap * m_imageShowing;
					Bitmap * m_imageToShow;
					State m_state;
					Drawing::ColorF m_color;
					float m_percentage;
					float m_speedFadeOut;
					float m_speedFadeIn;

				public:
					ImageFader();
					virtual ~ImageFader();

					virtual bool IsClass(Classes objectClass) const override;

                    void SetImage(Bitmap * image);
					void SetColor(Drawing::ColorF color);
					void SetSpeedFadeOut(float speed) { m_speedFadeOut = speed; }
					void SetSpeedFadeIn(float speed) { m_speedFadeIn = speed; }

					virtual void OnTick(float deltaSeconds) override;

					virtual void OnPaint(OpenGL::GL* gl) override;
			};
		}
	}
}
