#pragma once

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class Panel;
			class Splitter;

			class SplitContainer : public Control {
				public:
					struct Orientation {
						enum Enum {
							Horizontal = 0,
							Vertical = 1,
						};
					};

				private:
					Splitter * splitter;
					Panel * panel1;
					Panel * panel2;

					float splitterDistance;
					int splitterIncrement;
					int splitterWidth;
					Orientation::Enum orientation;

					void RecalculatePositions();

					void SetLocationW(Control * control, int pos);
					void SetSizeW(Control* control, int size, int substract);
					int GetSizeW(Control * control) const;
					int GetMinimumSizeW(Control * control) const;

				public:
					SplitContainer();
					virtual ~SplitContainer();

					virtual bool IsClass(Classes objectClass) const override;

					Panel * GetPanel1();
					Panel * GetPanel2();

					float GetSplitterDistance() const;
					void SetSplitterDistance(int distance);

					int GetSplitterIncrement() const;
					void SetSplitterIncrement(int increment);

					int GetSplitterWidth() const;
					void SetSplitterWidth(int width);

					Orientation::Enum GetOrientation() const;
					void SetOrientation(Orientation::Enum orientation);

					virtual void Layout();
			};
		}
	}
}
