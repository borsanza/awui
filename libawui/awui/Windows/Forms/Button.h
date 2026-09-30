#pragma once

#include <awui/Windows/Forms/Label.h>

#include <functional>

namespace awui::Windows::Forms {
	namespace Events {
		class IRemoteListener;
		class IExitListener;
	} // namespace Events

	class Button : public Control {
	  private:
		Label m_label;
		String m_text;
		int m_nextId = 0;
		std::vector<Events::IRemoteListener *> m_buttonListeners;
		std::vector<Events::IExitListener *> m_exitListeners;

		void Click();

	  public:
		Button();
		virtual ~Button();

		String GetText() const;
		void SetText(const String str);

		virtual void OnMouseDown(Events::MouseEventArgs *e);
		virtual bool OnRemoteKeyUp(int which, Input::RemoteButtons::Enum button);
		virtual void OnPaint(OpenGL::GL *gl);
		virtual void SetForeColor(const Drawing::Color color);
		virtual void SetFont(const Drawing::Font font);
		int GetLabelWidth() const;

		void AddOnClickListener(Events::IRemoteListener *listener);
		void AddOnExitListener(Events::IExitListener *listener);
		void RemoveOnClickListener(Events::IRemoteListener *listener);
		void RemoveOnExitListener(Events::IExitListener *listener);
		void RemoveAllListeners();
	};
} // namespace awui::Windows::Forms
