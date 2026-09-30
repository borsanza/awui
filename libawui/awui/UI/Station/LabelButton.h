#pragma once

#include <awui/UI/Label.h>

namespace awui::UI::Station {
	class LabelButton : public Label {
	  private:
		bool m_lastSelected;
		float m_time;

	  public:
		LabelButton();
		virtual ~LabelButton() = default;

		virtual void OnMouseUp(Events::MouseEventArgs *e);
		virtual void OnMouseDown(Events::MouseEventArgs *e);
		virtual void OnTick(float deltaSeconds);
	};
} // namespace awui::UI::Station
