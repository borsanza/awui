#pragma once

#include <awui/Windows/Forms/Control.h>
#include <awui/Windows/Forms/Label.h>

#include <functional>
#include <vector>

namespace awui::Windows::Forms {
	namespace Listeners {
		class IButtonListener;
	}

	class Button : public Control {
		private:
			Label m_label;
			String m_text;
			int m_nextId = 0;
			std::vector<Listeners::IButtonListener*> m_listeners;

			void Click();

		public:
			Button();
			virtual ~Button();

			virtual bool IsClass(Classes objectClass) const override;

			const String GetText();
			void SetText(const String str);

			virtual void OnMouseLeave();
			virtual void OnMouseDown(MouseEventArgs* e);
			virtual void OnMouseMove(MouseEventArgs* e);
			virtual void OnMouseUp(MouseEventArgs* e);
			virtual bool OnKeyPress(Keys::Enum key);
			virtual void OnPaint(OpenGL::GL * gl);
			virtual void SetForeColor(const Drawing::Color color);
			virtual void SetFont(const Drawing::Font font);
			int GetLabelWidth() const;

			void AddOnClickListener(Listeners::IButtonListener * listener);
			void RemoveOnClickListener(Listeners::IButtonListener* listener);
			void RemoveAllListeners();
	};
}