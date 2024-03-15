#pragma once

#include <awui/Windows/Forms/Control.h>
#include <awui/Windows/Forms/SplitContainer.h>

namespace awui::Windows::Forms {
	class Splitter : public Control {
	  private:
		SplitContainer::Orientation m_orientation;
		bool m_mouseActive;

	  public:
		Splitter();
		virtual ~Splitter() = default;

		virtual bool IsClass(Classes objectClass) const override;

		SplitContainer::Orientation GetOrientation() const;
		void SetOrientation(SplitContainer::Orientation orientation);

		virtual void OnMouseDown(MouseEventArgs *e) override;
		virtual void OnMouseMove(MouseEventArgs *e) override;
		virtual void OnMouseUp(MouseEventArgs *e) override;
		virtual void OnMouseEnter() override;
		virtual void OnMouseLeave() override;
	};
} // namespace awui::Windows::Forms
