#pragma once

#include <awui/Windows/Forms/Label.h>

#include <functional>

namespace awui::Windows::Forms {
	namespace Listeners {
		class IRemoteListener;
		class IExitListener;
	} // namespace Listeners

	class Button : public Control {
	  private:
		Label m_label;
		String m_text;
		int m_nextId = 0;
		std::vector<Listeners::IRemoteListener *> m_buttonListeners;
		std::vector<Listeners::IExitListener *> m_exitListeners;

		void Click();

	  public:
		Button();
		virtual ~Button();

		virtual bool IsClass(Classes objectClass) const override;

		const String GetText();
		void SetText(const String str);

		virtual void OnMouseDown(MouseEventArgs *e);
		virtual bool OnRemoteKeyUp(int which, RemoteButtons::Enum button);
		virtual void OnPaint(OpenGL::GL *gl);
		virtual void SetForeColor(const Drawing::Color color);
		virtual void SetFont(const Drawing::Font font);
		int GetLabelWidth() const;

		void AddOnClickListener(Listeners::IRemoteListener *listener);
		void AddOnExitListener(Listeners::IExitListener *listener);
		void RemoveOnClickListener(Listeners::IRemoteListener *listener);
		void RemoveOnExitListener(Listeners::IExitListener *listener);
		void RemoveAllListeners();
	};
} // namespace awui::Windows::Forms
