#pragma once

#include <awui/Drawing/Font.h>
#include <awui/Windows/Forms/Label.h>

namespace awui::Windows::Forms::Station {
	class NodeFile;
	class StationUI;

	class LabelButton : public Label {
		private:
			bool m_lastSelected;
			float m_time;

		public:
			LabelButton();
			virtual ~LabelButton();

			virtual void OnMouseDown(MouseEventArgs* e);
			virtual void OnTick(float deltaTime);
	};

	class MenuButton : public Control {
		private:
			LabelButton m_label;
			NodeFile * m_node;
			StationUI * m_station;

		public:
			MenuButton(StationUI * station);
			virtual ~MenuButton();

			virtual bool IsClass(Classes objectClass) const override;

			const String GetText();
			void SetText(const String str);

			virtual void OnMouseLeave();
			virtual void OnMouseDown(MouseEventArgs* e);
			virtual void OnPaint(OpenGL::GL* gl);
			virtual void SetForeColor(const Drawing::Color color);
			virtual void SetFont(const Drawing::Font font);
			int GetLabelWidth() const;
			virtual bool OnRemoteKeyUp(int which, RemoteButtons::Enum button);

			void SetNodeFile(NodeFile * node);

			void CheckArcade();

			virtual void OnResize();
	};
}
