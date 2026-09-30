#pragma once

#include <awui/UI/Control.h>
#include <awui/UI/SplitContainer.h>

namespace awui::UI {
	class Splitter : public Control {
	  private:
		SplitContainer::Orientation m_orientation;
		bool m_mouseActive;

	  public:
		Splitter();
		virtual ~Splitter() = default;

		SplitContainer::Orientation GetOrientation() const;
		void SetOrientation(SplitContainer::Orientation orientation);

		virtual void OnMouseDown(Events::MouseEventArgs *e) override;
		virtual void OnMouseMove(Events::MouseEventArgs *e) override;
		virtual void OnMouseUp(Events::MouseEventArgs *e) override;
		virtual void OnMouseEnter() override;
		virtual void OnMouseLeave() override;
	};
} // namespace awui::UI
