#pragma once

#include <awui/Windows/Forms/Control.h>
#include <awui/Windows/Forms/Station/LabelButton.h>

namespace awui::Windows::Forms::Station::Settings {
	class ConfigButton : public Control {
		private:
			LabelButton m_label;
			bool m_group;

		public:
			ConfigButton();
			virtual ~ConfigButton();

			virtual bool IsClass(Classes objectClass) const override;

			const String GetText();
			void SetText(const String str);

			virtual void OnPaint(OpenGL::GL* gl);
			virtual void SetForeColor(const Drawing::Color color);
			virtual void SetFont(const Drawing::Font font);
			int GetLabelWidth() const;

			inline void SetGroup(bool mode) { m_group = mode; }
			inline bool IsGroup() const { return m_group; }

			virtual void OnResize();
	};
}
