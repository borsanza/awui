#pragma once

#include <awui/Drawing/Font.h>
#include <awui/UI/Control.h>

#include <vector>

namespace awui {
	namespace Drawing {
		class Image;
	}

	namespace UI::Emulators {
		// Ayuda de las teclas de un juego (F1): un recuadro en el centro con una sección por grupo (las del sistema,
		// las de todos) y en cada fila la tecla y lo que hace. Se dibuja una vez con cairo, como los avisos, y
		// aparece y desaparece fundiéndose
		class KeyHelp : public Control {
		  public:
			struct Row {
				String keys;
				String description;
			};

			struct Section {
				String title;
				std::vector<Row> rows;
			};

		  private:
			Drawing::Image *m_image;
			Drawing::Font m_titleFont;
			Drawing::Font m_sectionFont;
			Drawing::Font m_keyFont;
			Drawing::Font m_textFont;
			bool m_showing;
			float m_opacity;

		  public:
			static constexpr float FadeSeconds = 0.15f;
			static constexpr int Padding = 32;
			static constexpr int Radius = 16;
			static constexpr int ColumnGap = 36; // Entre la tecla y su descripción
			static constexpr int RowGap = 8;
			static constexpr int SectionGap = 22;

			KeyHelp();
			virtual ~KeyHelp();

			void Show(const String &title, const std::vector<Section> &sections);
			void Hide();
			inline bool IsShowing() const { return m_showing; }

			virtual void OnTick(float deltaSeconds) override;
			virtual void OnPaint(OpenGL::GL *gl) override;
		};
	} // namespace UI::Emulators
} // namespace awui
