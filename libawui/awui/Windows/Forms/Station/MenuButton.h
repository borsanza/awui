#pragma once

#include <awui/Windows/Forms/Label.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			namespace Station {
				class NodeFile;
				class StationUI;

				class MenuButton : public Control {
					private:
						Label _label;
						NodeFile * _node;
						StationUI * _station;

					public:
						MenuButton(StationUI * station);
						virtual ~MenuButton();

						virtual int IsClass(Classes::Enum objectClass) const;

						const String GetText();
						void SetText(const String str);

						virtual void OnMouseLeave();
						virtual void OnMouseDown(MouseEventArgs* e);
						virtual void OnMouseMove(MouseEventArgs* e);
						virtual void OnPaint(OpenGL::GL* gl);
						virtual void SetForeColor(const Drawing::Color color);
						virtual void SetFont(const Drawing::Font * font);
						int GetLabelWidth() const;
						virtual bool OnRemoteKeyUp(int which, RemoteButtons::Enum button);

						void SetNodeFile(NodeFile * node);

						void CheckArcade();
				};
			}
		}
	}
}
