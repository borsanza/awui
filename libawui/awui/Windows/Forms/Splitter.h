// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Windows/Forms/Control.h>

#include <awui/Windows/Forms/SplitContainer.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class Splitter : public Control {
			private:
					SplitContainer::Orientation orientation;
					int mouseActive;

			public:
				Splitter();
				~Splitter();

				virtual int IsClass(awui::Classes objectClass) const;

				SplitContainer::Orientation GetOrientation();
				void SetOrientation(SplitContainer::Orientation orientation);

				virtual void OnMouseDown(MouseEventArgs * e);
				virtual void OnMouseMove(MouseEventArgs * e);
				virtual void OnMouseUp(MouseEventArgs * e);
				virtual void OnMouseEnter();
				virtual void OnMouseLeave();
			};
		}
	}
}
