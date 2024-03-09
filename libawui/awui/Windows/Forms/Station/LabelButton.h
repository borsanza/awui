#pragma once

#include <awui/Windows/Forms/Label.h>

namespace awui::Windows::Forms::Station {
	class LabelButton : public Label {
		private:
			bool m_lastSelected;
			float m_time;

		public:
			LabelButton();
			virtual ~LabelButton() = default;

			virtual void OnMouseUp(MouseEventArgs* e);
			virtual void OnMouseDown(MouseEventArgs* e);
			virtual void OnTick(float deltaSeconds);
	};
}
