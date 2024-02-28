#pragma once

#include <awui/Drawing/Font.h>
#include <awui/Windows/Forms/Station/LabelButton.h>

namespace awui::Windows::Forms::Station {
	class NodeFile;
	class StationUI;

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
