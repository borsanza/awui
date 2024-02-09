#pragma once

#include <awui/Windows/Forms/Control.h>
#include <awui/Windows/Forms/SplitContainer.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class Splitter : public Control {
				private:
						SplitContainer::Orientation::Enum orientation;
						int mouseActive;

				public:
					Splitter();
					virtual ~Splitter();

					virtual bool IsClass(Classes objectClass) const override;

					SplitContainer::Orientation::Enum GetOrientation();
					void SetOrientation(SplitContainer::Orientation::Enum orientation);

					virtual void OnMouseDown(MouseEventArgs * e);
					virtual void OnMouseMove(MouseEventArgs * e);
					virtual void OnMouseUp(MouseEventArgs * e);
					virtual void OnMouseEnter();
					virtual void OnMouseLeave();
			};
		}
	}
}
