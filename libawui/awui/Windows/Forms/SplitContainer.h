#pragma once

#include <awui/Windows/Forms/Control.h>

namespace awui::Windows::Forms {
	class Panel;
	class Splitter;

	class SplitContainer : public Control {
		public:
			enum class Orientation {
				Horizontal,
				Vertical,
			};

		private:
			Splitter * m_splitter;
			Panel * m_panel1;
			Panel * m_panel2;

			float m_splitterDistance;
			int m_splitterIncrement;
			int m_splitterWidth;
			Orientation m_orientation;

			void RecalculatePositions();

			void SetLocationW(Control * control, int pos);
			void SetSizeW(Control* control, int size, int substract);
			int GetSizeW(Control * control) const;
			int GetMinimumSizeW(Control * control) const;

		public:
			SplitContainer();
			virtual ~SplitContainer() = default;

			virtual bool IsClass(Classes objectClass) const override;

			Panel * GetPanel1() const;
			Panel * GetPanel2() const;

			float GetSplitterDistance() const;
			void SetSplitterDistance(int distance);

			int GetSplitterIncrement() const;
			void SetSplitterIncrement(int increment);

			int GetSplitterWidth() const;
			void SetSplitterWidth(int width);

			Orientation GetOrientation() const;
			void SetOrientation(Orientation orientation);

			virtual void Layout() override;
	};
}
